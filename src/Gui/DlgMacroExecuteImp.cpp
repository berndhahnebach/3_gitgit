/** \file DlgMacroExecuteImp.cpp
 *  \brief  
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 *   
 */

/***************************************************************************
 *   (c) J�rgen Riegel (juergen.riegel@web.de) 2002                        *   
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License (LGPL)   *
 *   as published by the Free Software Foundation; either version 2 of     *
 *   the License, or (at your option) any later version.                   *
 *   for detail see the LICENCE text file.                                 *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        * 
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with FreeCAD; if not, write to the Free Software        * 
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
 *   USA                                                                   *
 *                                                                         *
 *   Juergen Riegel 2002                                                   *
 ***************************************************************************/


/** Precompiled header stuff
 *  on some compilers the precompiled header option gain significant compile 
 *  time! So every external header (libs and system) should included in 
 *  Precompiled.h. For systems without precompilation the header needed are
 *  included in the else fork.
 */
#include "PreCompiled.h"
#ifndef _PreComp_

#endif


#include "DlgMacroExecuteImp.h"
#include "../App/Application.h"
#include "Application.h"
#include "Macro.h"
#include "ScintillaQt.h"

/* 
 *  Constructs a DlgMacroExecuteImp which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgMacroExecuteImp::DlgMacroExecuteImp( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : DlgMacroExecute( parent, name, modal, fl ),FCWindowParameter(name)
{
	// retrive the macro path from parameter or use the home path as default
	_cMacroPath = GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Macro/")->GetASCII("MacroPath",GetApplication().GetHomePath());

	ComboBoxPath->insertItem(_cMacroPath.c_str());
	// Fill the List box
	FillList();
}

/*  
 *  Destroys the object and frees any allocated resources
 */
DlgMacroExecuteImp::~DlgMacroExecuteImp()
{
    // no need to delete child widgets, Qt does it all for us
}

void DlgMacroExecuteImp::FillList(void)
{
    // lists all files in macro path
    QDir d( _cMacroPath.c_str(),"*.FCMacro" );

	// clean old entries
	MacrosListView->clear();
	// fill up with the directory
    for (unsigned int i=0; i<d.count(); i++ )
        new QListViewItem( MacrosListView, d[i], "FCMacro file" );

}

void DlgMacroExecuteImp::OnNewListItemPicked(QListViewItem* pcListViewItem)
{
    LineEditMacroName->setText(pcListViewItem->text(0));
	
	ExecuteButton->setEnabled(true);
	EditButton->setEnabled(true);
	DeleteButton->setEnabled(true);

}

void DlgMacroExecuteImp::OnListDoubleClicked(QListViewItem* pcListViewItem)
{
    LineEditMacroName->setText(pcListViewItem->text(0));
	accept();
	ApplicationWindow::Instance->GetMacroMngr()->Run(FCMacroManager::File,(_cMacroPath + LineEditMacroName->text().latin1()).c_str());
}


/* 
 * public slot
 */
void DlgMacroExecuteImp::OnExecute()
{
	if(!(LineEditMacroName->text().isEmpty() ) )
	{
		accept();
		ApplicationWindow::Instance->GetMacroMngr()->Run(FCMacroManager::File,(_cMacroPath + LineEditMacroName->text().latin1()).c_str());
	}
}
/* 
 * public slot
 */
void DlgMacroExecuteImp::OnNewFolder()
{
    QString fn = QFileDialog::getExistingDirectory (_cMacroPath.c_str(), ApplicationWindow::Instance);
	if (!fn.isEmpty())
	{
		_cMacroPath = fn.latin1();
		// combo box
		ComboBoxPath->insertItem(fn);
		// save the path in the parameters
		GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Macro/")->SetASCII("MacroPath",fn.latin1());
		// fill the list box
		FillList();
	}
}

void DlgMacroExecuteImp::OnCreate()
{
    qWarning( "DlgMacroExecuteImp::OnNewListItemPicked(QListViewItem*) not yet implemented!" ); 
}

void DlgMacroExecuteImp::OnEdit()
{
  FCScintillaDoc* doc = new FCScintillaDoc;
  doc->CreateView("Editor");
  accept();
//    qWarning( "DlgMacroExecuteImp::OnNewListItemPicked(QListViewItem*) not yet implemented!" ); 
}

void DlgMacroExecuteImp::OnDelete()
{
    qWarning( "DlgMacroExecuteImp::OnNewListItemPicked(QListViewItem*) not yet implemented!" ); 
}







// compile the mocs and Dialog
#include "DlgMacroExecute.cpp"
#include "moc_DlgMacroExecute.cpp"
#include "moc_DlgMacroExecuteImp.cpp"
