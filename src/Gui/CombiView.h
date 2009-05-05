/***************************************************************************
 *   Copyright (c) 2009 J�rgen Riegel <juergen.riegel@web.de>              *
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



#ifndef GUI_DOCKWND_COMBIVIEW_H
#define GUI_DOCKWND_COMBIVIEW_H


#ifndef __Qt4All__
# include "Qt4All.h"
#endif


#include "DockWindow.h"
#include "Selection.h"
#include "iisTaskPanel/include/iisTaskPanel"

namespace App {
  class PropertyContainer;
}

namespace Gui {
    class TreeWidget;

namespace PropertyEditor {

class EditableListView;
class EditableItem;
class PropertyEditor;

} // namespace PropertyEditor
} // namespace Gui


namespace Gui {
namespace DockWnd {

/** Combi View
  * is a combination of a tree, property and TaskPanel for 
  * integrated user action.
 */
class CombiView : public Gui::DockWindow, public Gui::SelectionSingleton::ObserverType
{
    Q_OBJECT

public:
    /**
     * A constructor.
     * A more elaborate description of the constructor.
     */
    CombiView(Gui::Document*  pcDocument, QWidget *parent=0);

    /**
     * A destructor.
     * A more elaborate description of the destructor.
    */
    virtual ~CombiView();

    /// Observer message from the Selection
    virtual void OnChange(Gui::SelectionSingleton::SubjectType &rCaller,
                          Gui::SelectionSingleton::MessageType Reason);


    bool onMsg(const char* pMsg);

    virtual const char *getName(void) const {return "CombiView";}

    /// get called when the document is changed or updated
    virtual void onUpdate(void);

private:
    QTabWidget                         * tabs,*proptertyTabs;
    Gui::TreeWidget                    * tree;
    iisTaskPanel                       * taskPanel;
    Gui::PropertyEditor::PropertyEditor* propertyEditorView;
    Gui::PropertyEditor::PropertyEditor* propertyEditorData;

};

} // namespace DockWnd
} // namespace Gui

#endif // GUI_DOCKWND_SELECTIONVIEW_H
