/***************************************************************************
 *   Copyright (c) 2002 J�rgen Riegel <juergen.riegel@web.de>              *
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



#ifndef GUI_DOCKWND_PROPERTYVIEW_H
#define GUI_DOCKWND_PROPERTYVIEW_H


#ifndef __Qt4All__
# include "Qt4All.h"
#endif


#include "DockWindow.h"
#include "Selection.h"

class QPixmap;

namespace App {
  class PropertyContainer;
}

namespace Gui {
namespace PropertyEditor {

class EditableListView;
class EditableItem;
class PropertyEditor;

} // namespace PropertyEditor
} // namespace Gui

namespace Gui {
namespace DockWnd {

/** A test class. A more elaborate class description.
 */
class PropertyView :public Gui::DockWindow, public Gui::SelectionSingleton::ObserverType
{
    Q_OBJECT

public:
    /**
     * A constructor.
     * A more elaborate description of the constructor.
     */
    PropertyView(Gui::Document*  pcDocument, QWidget *parent=0);

    /**
     * A destructor.
     * A more elaborate description of the destructor.
    */
    virtual ~PropertyView();

    /// Observer message from the Selection
    virtual void OnChange(Gui::SelectionSingleton::SubjectType &rCaller,
                          Gui::SelectionSingleton::MessageType Reason);


    bool onMsg(const char* pMsg);

    virtual const char *getName(void) const {return "PropertyView";}

    /// get called when the document is changed or updated
    virtual void onUpdate(void);

    static QPixmap *pcLabelOpen, *pcLabelClosed, *pcAttribute;

    Gui::PropertyEditor::PropertyEditor* propertyEditorView;
    Gui::PropertyEditor::PropertyEditor* propertyEditorData;

protected:
    void changeEvent(QEvent *e);

private:
    QTabWidget* tabs;
};

} // namespace DockWnd
} // namespace Gui

#endif // GUI_DOCKWND_PROPERTYVIEW_H
