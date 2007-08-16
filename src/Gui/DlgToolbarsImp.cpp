/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <werner.wm.mayer@gmx.de>              *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#include "PreCompiled.h"

#ifndef __Qt4All__
# include "Qt4All.h"
#endif

#include "DlgToolbarsImp.h"
#include "Application.h"
#include "BitmapFactory.h"
#include "Tools.h"
#include "Command.h"
#include "ToolBarManager.h"
#include "Widgets.h"
#include "Workbench.h"
#include "WorkbenchManager.h"

using namespace Gui::Dialog;

/* TRANSLATOR Gui::Dialog::DlgCustomToolbars */

/**
 *  Constructs a DlgCustomToolbars which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgCustomToolbars::DlgCustomToolbars(DlgCustomToolbars::Type t, QWidget* parent)
    : CustomizeActionPage(parent), type(t)
{
    this->setupUi(this);

    CommandManager & cCmdMgr = Application::Instance->commandManager();
    std::map<std::string,Command*> sCommands = cCmdMgr.getCommands();

    // do a special sort before adding to the tree view
    QStringList items; items << "File" << "Edit" << "View" << "Standard-View" << "Tools" << "Window" << "Help" << "Macros";
    for (std::map<std::string,Command*>::iterator it = sCommands.begin(); it != sCommands.end(); ++it) {
        QString group = it->second->getGroupName();
        if (!items.contains(group))
            items << group;
    }

    int index = 0;
    for ( QStringList::Iterator It = items.begin(); It != items.end(); ++It, ++index ) {
        categoryBox->addItem(QObject::tr((*It).toAscii()));
        categoryBox->setItemData(index, QVariant(*It), Qt::UserRole);
    }

    // fills the combo box with all available workbenches
    QStringList work = Application::Instance->workbenches();
    work.sort();
    index = 0;
    for ( QStringList::Iterator it = work.begin(); it != work.end(); ++it, ++index ) {
        QPixmap px = Application::Instance->workbenchIcon( *it );
        if ( px.isNull() )
            workbenchBox->addItem( *it );
        else
            workbenchBox->addItem( px, *it );
        workbenchBox->setItemData(index, QVariant(*it), Qt::UserRole);
    }

    QStringList labels; 
    labels << "Icon" << "Command";
    commandTreeWidget->setHeaderLabels(labels);
    commandTreeWidget->header()->hide();
    labels.clear(); labels << "Command";
    toolbarTreeWidget->setHeaderLabels(labels);
    toolbarTreeWidget->header()->hide();

    on_categoryBox_activated(categoryBox->currentIndex());
    on_workbenchBox_activated(workbenchBox->currentIndex());
}

/** Destroys the object and frees any allocated resources */
DlgCustomToolbars::~DlgCustomToolbars()
{
}

void DlgCustomToolbars::on_categoryBox_activated(int index)
{
    QVariant data = categoryBox->itemData(index, Qt::UserRole);
    QString group = data.toString();
    commandTreeWidget->clear();

    CommandManager & cCmdMgr = Application::Instance->commandManager();
    std::vector<Command*> aCmds = cCmdMgr.getGroupCommands( group.toAscii() );
    for (std::vector<Command*>::iterator it = aCmds.begin(); it != aCmds.end(); ++it) {
        QTreeWidgetItem* item = new QTreeWidgetItem(commandTreeWidget);
        item->setText(1, QObject::tr((*it)->getMenuText()));
        item->setToolTip(1, QObject::tr((*it)->getToolTipText()));
        item->setData(1, Qt::UserRole, QString((*it)->getName()));
        item->setSizeHint(0, QSize(32, 32));
        item->setBackgroundColor(0, Qt::lightGray);
        if ((*it)->getPixmap())
            item->setIcon(0, BitmapFactory().pixmap((*it)->getPixmap()));
    }

    commandTreeWidget->resizeColumnToContents(0);
}

void DlgCustomToolbars::on_workbenchBox_activated(int index)
{
    QVariant data = workbenchBox->itemData(index, Qt::UserRole);
    QString group = data.toString();
    toolbarTreeWidget->clear();

    renameButton->setEnabled(false);
    deleteButton->setEnabled(false);

    QByteArray name = group.toAscii();
    importCustomToolbars(name);
}

void DlgCustomToolbars::importCustomToolbars(const QByteArray& name)
{
    ParameterGrp::handle hGrp = App::GetApplication().GetUserParameter().GetGroup("BaseApp")->GetGroup("Workbench");
    const char* subgroup = (type == Toolbar ? "Toolbar" : "Toolboxbar");
    hGrp = hGrp->GetGroup(name.constData())->GetGroup(subgroup);

    std::vector<FCHandle<ParameterGrp> > hGrps = hGrp->GetGroups();
    CommandManager& rMgr = Application::Instance->commandManager();
    for (std::vector<FCHandle<ParameterGrp> >::iterator it = hGrps.begin(); it != hGrps.end(); ++it) {
        // create a toplevel item
        QTreeWidgetItem* toplevel = new QTreeWidgetItem(toolbarTreeWidget);
        toplevel->setText(0, (*it)->GetGroupName());
        bool active = hGrp->GetGroup((*it)->GetGroupName())->GetBool("Active", true);
        toplevel->setCheckState(0, (active ? Qt::Checked : Qt::Unchecked));

        // get the elements of the subgroups
        std::vector<std::pair<std::string,std::string> > items = hGrp->GetGroup((*it)->GetGroupName())->GetASCIIMap();
        for (std::vector<std::pair<std::string,std::string> >::iterator it2 = items.begin(); it2 != items.end(); ++it2) {
            Command* pCmd = rMgr.getCommandByName(it2->first.c_str());
            if (pCmd) {
                QString cmd = it2->first.c_str(); // command name
                QTreeWidgetItem* item = new QTreeWidgetItem(toplevel);
                item->setText(0, QObject::tr(pCmd->getMenuText()));
                item->setData(0, Qt::UserRole, QByteArray(it2->first.c_str()));
                if (pCmd->getPixmap())
                    item->setIcon(0, BitmapFactory().pixmap(pCmd->getPixmap()));
                item->setSizeHint(0, QSize(32, 32));
            }
        }
    }
}

void DlgCustomToolbars::exportCustomToolbars(const QByteArray& name)
{
    ParameterGrp::handle hGrp = App::GetApplication().GetUserParameter().GetGroup("BaseApp")->GetGroup("Workbench");
    const char* subgroup = (type == Toolbar ? "Toolbar" : "Toolboxbar");
    hGrp = hGrp->GetGroup(name.constData())->GetGroup(subgroup);
    hGrp->Clear();

    CommandManager& rMgr = Application::Instance->commandManager();
    for (int i=0; i<toolbarTreeWidget->topLevelItemCount(); i++) {
        QTreeWidgetItem* toplevel = toolbarTreeWidget->topLevelItem(i);
        QByteArray groupName = toplevel->text(0).toAscii();
        ParameterGrp::handle hToolGrp = hGrp->GetGroup(groupName.constData());
        hToolGrp->SetBool("Active", toplevel->checkState(0) == Qt::Checked);
        for (int j=0; j<toplevel->childCount(); j++) {
            QTreeWidgetItem* child = toplevel->child(j);
            QByteArray commandName = child->data(0, Qt::UserRole).toByteArray();
            Command* pCmd = rMgr.getCommandByName(commandName);
            if (pCmd) {
                hToolGrp->SetASCII(pCmd->getName(), pCmd->getAppModuleName());
            }
        }
    }
}

/** Enables/disables buttons for change */
void DlgCustomToolbars::on_commandTreeWidget_currentItemChanged(QTreeWidgetItem* item)
{
    bool canAdd = false;
    if (item && commandTreeWidget->isItemSelected(item)) {
        QTreeWidgetItem* current = toolbarTreeWidget->currentItem();
        if (current && !current->parent() && toolbarTreeWidget->isItemSelected(current)) {
            canAdd = true;
        }
    }

    moveActionRightButton->setEnabled(canAdd);
}

/** Enables/disables buttons for change */
void DlgCustomToolbars::on_toolbarTreeWidget_currentItemChanged(QTreeWidgetItem* item)
{
    bool canAdd = false;
    bool canRemove = false;
    bool canMoveUp = false;
    bool canMoveDown = false;
    bool canRename = false;
    bool canDelete = false;

    if (item && toolbarTreeWidget->isItemSelected(item)) {
        // must not be top-level
        QTreeWidgetItem* parent = item->parent();
        if (parent) {
            canRemove = true;
            if (parent->indexOfChild(item) > 0)
                canMoveUp = true;
            if (parent->indexOfChild(item) < parent->childCount()-1)
                canMoveDown = true;
        } else {
            canRename = true;
            canDelete = true;
            QTreeWidgetItem* current = commandTreeWidget->currentItem();
            if (current && commandTreeWidget->isItemSelected(current))
                canAdd = true;
        }
    }

    moveActionRightButton->setEnabled(canAdd);
    moveActionLeftButton->setEnabled(canRemove);
    moveActionUpButton->setEnabled(canMoveUp);
    moveActionDownButton->setEnabled(canMoveDown);
    renameButton->setEnabled(canRename);
    deleteButton->setEnabled(canDelete);
}

/** Adds a new action */
void DlgCustomToolbars::on_moveActionRightButton_clicked()
{
    QTreeWidgetItem* item = commandTreeWidget->currentItem();
    if (item) {
        QTreeWidgetItem* current = toolbarTreeWidget->currentItem();
        if (current && !current->parent() && toolbarTreeWidget->isItemSelected(current)) {
            QTreeWidgetItem* copy = new QTreeWidgetItem(current);
            copy->setText(0, item->text(1));
            copy->setIcon(0, item->icon(0));
            QByteArray data = item->data(1, Qt::UserRole).toString().toAscii();
            copy->setData(0, Qt::UserRole, data);
            copy->setSizeHint(0, QSize(32, 32));
        }
    }

    QVariant data = workbenchBox->itemData(workbenchBox->currentIndex(), Qt::UserRole);
    QString group = data.toString();
    exportCustomToolbars(group.toAscii());
}

/** Removes an action */
void DlgCustomToolbars::on_moveActionLeftButton_clicked()
{
    QTreeWidgetItem* item = toolbarTreeWidget->currentItem();
    if (item && item->parent() && toolbarTreeWidget->isItemSelected(item)) {
        QTreeWidgetItem* parent = item->parent();
        int index = parent->indexOfChild(item);
        parent->takeChild(index);
        delete item;
    }

    QVariant data = workbenchBox->itemData(workbenchBox->currentIndex(), Qt::UserRole);
    QString group = data.toString();
    exportCustomToolbars(group.toAscii());
}

/** Noves up an action */
void DlgCustomToolbars::on_moveActionUpButton_clicked()
{
    QTreeWidgetItem* item = toolbarTreeWidget->currentItem();
    if (item && item->parent() && toolbarTreeWidget->isItemSelected(item)) {
        QTreeWidgetItem* parent = item->parent();
        int index = parent->indexOfChild(item);
        parent->takeChild(index);
        parent->insertChild(index-1, item);
        toolbarTreeWidget->setCurrentItem(item);
    }

    QVariant data = workbenchBox->itemData(workbenchBox->currentIndex(), Qt::UserRole);
    QString group = data.toString();
    exportCustomToolbars(group.toAscii());
}

/** Moves down an action */
void DlgCustomToolbars::on_moveActionDownButton_clicked()
{
    QTreeWidgetItem* item = toolbarTreeWidget->currentItem();
    if (item && item->parent() && toolbarTreeWidget->isItemSelected(item)) {
        QTreeWidgetItem* parent = item->parent();
        int index = parent->indexOfChild(item);
        parent->takeChild(index);
        parent->insertChild(index+1, item);
        toolbarTreeWidget->setCurrentItem(item);
    }

    QVariant data = workbenchBox->itemData(workbenchBox->currentIndex(), Qt::UserRole);
    QString group = data.toString();
    exportCustomToolbars(group.toAscii());
}

void DlgCustomToolbars::on_newButton_clicked()
{
    bool ok;
    QString text = QString("Custom%1").arg(toolbarTreeWidget->topLevelItemCount()+1);
    text = QInputDialog::getText(this, tr("New toolbar"), tr("Toolbar name:"), QLineEdit::Normal, text, &ok);
    if (ok) {
        // Check for duplicated name
        for (int i=0; i<toolbarTreeWidget->topLevelItemCount(); i++) {
            QTreeWidgetItem* toplevel = toolbarTreeWidget->topLevelItem(i);
            QString groupName = toplevel->text(0);
            if (groupName == text) {
                QMessageBox::warning(this, tr("Duplicated name"), tr("The toolbar name '%1' is already used").arg(text));
                return;
            }
        }

        QTreeWidgetItem* item = new QTreeWidgetItem(toolbarTreeWidget);
        item->setText(0, text);
        item->setCheckState(0, Qt::Checked);
        toolbarTreeWidget->setItemExpanded(item, true);

        QVariant data = workbenchBox->itemData(workbenchBox->currentIndex(), Qt::UserRole);
        QString group = data.toString();
        exportCustomToolbars(group.toAscii());
    }
}

void DlgCustomToolbars::on_deleteButton_clicked()
{
    QTreeWidgetItem* item = toolbarTreeWidget->currentItem();
    if (item && !item->parent() && toolbarTreeWidget->isItemSelected(item)) {
        int index = toolbarTreeWidget->indexOfTopLevelItem(item);
        toolbarTreeWidget->takeTopLevelItem(index);
        delete item;
    }

    QVariant data = workbenchBox->itemData(workbenchBox->currentIndex(), Qt::UserRole);
    QString group = data.toString();
    exportCustomToolbars(group.toAscii());
}

void DlgCustomToolbars::on_renameButton_clicked()
{
    bool renamed = false;
    QTreeWidgetItem* item = toolbarTreeWidget->currentItem();
    if (item && !item->parent() && toolbarTreeWidget->isItemSelected(item)) {
        bool ok;
        QString text = item->text(0);
        text = QInputDialog::getText(this, tr("Rename toolbar"), tr("Toolbar name:"), QLineEdit::Normal, text, &ok);
        if (ok) {
            // Check for duplicated name
            for (int i=0; i<toolbarTreeWidget->topLevelItemCount(); i++) {
                QTreeWidgetItem* toplevel = toolbarTreeWidget->topLevelItem(i);
                QString groupName = toplevel->text(0);
                if (groupName == text && toplevel != item) {
                    QMessageBox::warning(this, tr("Duplicated name"), tr("The toolbar name '%1' is already used").arg(text));
                    return;
                }
            }

            item->setText(0, text);
            renamed = true;
        }
    }

    if (renamed) {
        QVariant data = workbenchBox->itemData(workbenchBox->currentIndex(), Qt::UserRole);
        QString group = data.toString();
        exportCustomToolbars(group.toAscii());
    }
}

void DlgCustomToolbars::onAddMacroAction(const QString& macro)
{
    QVariant data = categoryBox->itemData(categoryBox->currentIndex(), Qt::UserRole);
    QString group = data.toString();
    if (group == "Macros")
    {
        CommandManager & cCmdMgr = Application::Instance->commandManager();
        Command* pCmd = cCmdMgr.getCommandByName(macro.toAscii());

        QTreeWidgetItem* item = new QTreeWidgetItem(commandTreeWidget);
        item->setText(1, pCmd->getMenuText());
        item->setToolTip(1, pCmd->getToolTipText());
        item->setData(1, Qt::UserRole, QString(pCmd->getName()));
        item->setSizeHint(0, QSize(32, 32));
        item->setBackgroundColor(0, Qt::lightGray);
        if (pCmd->getPixmap())
            item->setIcon(0, BitmapFactory().pixmap(pCmd->getPixmap()));
    }
}

void DlgCustomToolbars::onRemoveMacroAction(const QString& macro)
{
    QVariant data = categoryBox->itemData(categoryBox->currentIndex(), Qt::UserRole);
    QString group = data.toString();
    if (group == "Macros")
    {
        for (int i=0; i<commandTreeWidget->topLevelItemCount(); i++) {
            QTreeWidgetItem* item = commandTreeWidget->topLevelItem(i);
            QString command = item->data(1, Qt::UserRole).toString();
            if (command == macro) {
                commandTreeWidget->takeTopLevelItem(i);
                delete item;
                break;
            }
        }
    }
}

// -------------------------------------------------------------

/* TRANSLATOR Gui::Dialog::DlgCustomToolbarsImp */

/**
 *  Constructs a DlgCustomToolbarsImp which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgCustomToolbarsImp::DlgCustomToolbarsImp( QWidget* parent )
    : DlgCustomToolbars(DlgCustomToolbars::Toolbar, parent)
{
}

/** Destroys the object and frees any allocated resources */
DlgCustomToolbarsImp::~DlgCustomToolbarsImp()
{
}


/* TRANSLATOR Gui::Dialog::DlgCustomToolBoxbarsImp */

/**
 *  Constructs a DlgCustomToolBoxbarsImp which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgCustomToolBoxbarsImp::DlgCustomToolBoxbarsImp( QWidget* parent )
    : DlgCustomToolbars(DlgCustomToolbars::Toolboxbar, parent)
{
    setWindowTitle( tr( "Toolbox bars" ) );
}

/** Destroys the object and frees any allocated resources */
DlgCustomToolBoxbarsImp::~DlgCustomToolBoxbarsImp()
{
}

#include "moc_DlgToolbarsImp.cpp"
