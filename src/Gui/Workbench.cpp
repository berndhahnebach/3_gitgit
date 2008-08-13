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

#include "Workbench.h"
#include "WorkbenchPy.h"
#include "PythonWorkbenchPy.h"
#include "MenuManager.h"
#include "ToolBarManager.h"
#include "DockWindowManager.h"
#include "Application.h"
#include "Action.h"
#include "Command.h"
#include "ToolBoxManager.h"
#include "Window.h"
#include "Selection.h"

#include <App/Application.h>
#include <App/DocumentObject.h>
#include <Base/Parameter.h>
#include <Base/Interpreter.h>

using namespace Gui;

/** \defgroup workbench Workbench Framework
 *
 * FreeCAD provides the possibility to have one or more workbenches for a module. A workbench changes the appearance of the main window
 * in that way that it defines toolbars, command bars, menus or the context menu (later on dockable windows, ...) that are shown to the user.
 *
 * The idea behind this concept is that the user should see only the functions that are required for the task that he is doing at this
 * moment and not to show dozens of unneeded functions which the user never uses.
 *
 * \section stepbystep Step by step
 * Here follows a short description of how an own workbench can be added to a module.
 *
 * \subsection newClass Inherit either from Workbench or StdWorkbench
 * First you have to subclass either Workbench or StdWorkbench and reimplement the methods setupMenuBar(), setupToolBars() and setupCommandBars().
 * The difference between both classes is that these methods of Workbench are pure virtual while StdWorkbench defines already the standard menus and
 * toolbars, such as the 'File', 'Edit', ..., 'Help' menus with their common functions.
 * If your class derives from Workbench then you have to define your menus, toolbars and command bars from scratch while deriving from StdWorkbench
 * you have the possibility to add your preferred functions or even remove some unneeded functions.
 * \code
 *
 * class MyWorkbench : public StdWorkbench
 * {
 *  ...
 * protected:
 *   MenuItem* setupMenuBar() const
 *   {
 *     MenuItem* root = StdWorkbench::setupMenuBar();
 *     // your changes
 *     return root;
 *   }
 *   ToolBarItem* setupToolBars() const
 *   {
 *     ToolBarItem* root = StdWorkbench::setupToolBars();
 *     // your changes
 *     return root;
 *   }
 *   ToolBarItem* setupCommandBars() const
 *   {
 *     ToolBarItem* root = StdWorkbench::setupCommandBars();
 *     // your changes
 *     return root;
 *   }
 * };
 *
 * \endcode
 * or
 * \code
 *
 * class MyWorkbench : public Workbench
 * {
 *  ...
 * protected:
 *   MenuItem* setupMenuBar() const
 *   {
 *     MenuItem* root = new MenuItem;
 *     // setup from scratch
 *     return root;
 *   }
 *   ToolBarItem* setupToolBars() const
 *   {
 *     ToolBarItem* root = new ToolBarItem;
 *     // setup from scratch
 *     return root;
 *   }
 *   ToolBarItem* setupCommandBars() const
 *   {
 *     ToolBarItem* root = new ToolBarItem;
 *     // setup from scratch
 *     return root;
 *   }
 * };
 * 
 * \endcode
 *
 * \subsection customizeWorkbench Customizing the workbench
 * If you want to customize your workbench by adding or removing items you can use the ToolBarItem class for customizing toolbars and the MenuItem class
 * for menus. Both classes behave basically the same.
 * To add a new menu item you can do it as follows
 * \code
 *   MenuItem* setupMenuBar() const
 *   {
 *     MenuItem* root = StdWorkbench::setupMenuBar();
 *     // create a sub menu
 *     MenuItem* mySub = new MenuItem; // note: no parent is given
 *     mySub->setCommand( "My &Submenu" );
 *     *mySub << "Std_Undo" << "Std_Redo";
 *
 *     // My menu
 *     MenuItem* myMenu = new MenuItem( root );
 *     myMenu->setCommand( "&My Menu" );
 *     // fill up the menu with some command items
 *     *myMenu << mySub << "Separator" << "Std_Cut" << "Std_Copy" << "Std_Paste" << "Separator" << "Std_Undo" << "Std_Redo";
 *   }
 * \endcode
 *
 * Toolbars can be cutomized the same way unless that you shouldn't create subitems (there are no subtoolbars).
 *
 * \subsection regWorkbench Register your workbench
 * Once you have implemented your workbench class you have to register it to make it known to the FreeCAD core system. You must make sure that the step
 * of registration is performed only once. A good place to do it is e.g. in the global function initMODULEGui in AppMODULEGui.cpp where MODULE stands
 * for the name of your module. Just add the line
 * \code
 * MODULEGui::MyWorkbench::init();
 * \endcode
 * somewhere there.
 *
 * \subsection itemWorkbench Create an item for your workbench
 * Though your workbench has been registered now,  at this stage you still cannot invoke it yet. Therefore you must create an item in the list of all visible
 * workbenches. To perform this step you must open your InitGui.py (a Python file) and do some adjustments. The file contains already a Python class
 * MODULEWorkbench that implements the Activate() method (it imports the needed library). You can also implement the GetIcon() method to set your own icon for
 * your workbench, if not, the default FreeCAD icon is taken, and finally the most important method GetClassName(). that represents the link between
 * Python and C++. This method must return the name of the associated C++ including namespace. In this case it must the string "ModuleGui::MyWorkbench".
 * At the end you can change the line from
 * \code
 * Gui.addWorkbench("MODULE design",MODULEWorkbench())
 * \endcode
 * to
 * \code
 * Gui.addWorkbench("My workbench",MODULEWorkbench())
 * \endcode
 * or whatever you want.
 * \note You must make sure to choose a unique name for your workbench (in this example "My workbench"). Since FreeCAD doesn't provide a mechanism for
 * this you have to care on your own.
 *
 * \section moredetails More details and limitations
 * One of the key concepts of the workbench framework is to load a module at runtime when the user needs some function that it
 * provides. So, if the user doesn't need a module it never gets loaded into RAM. This speeds up the startup procedure of
 * FreeCAD and saves memory.
 * At startup FreeCAD scans all module directories and invokes InitGui.py. So an item for a workbench gets created. If the user
 * clicks on such an item the matching module gets loaded, the C++ workbench gets registered and activated.
 *
 * The user is able to modify a workbench (Edit|Customize). E.g. he can add new toolbars or command bars and add his preferred
 * functions to them. But he has only full control over "his" toolbars, the default workbench items cannot be modified or even removed.
 *
 * At the moment only toolbars, command bars and menus are handled by a workbench. In later versions it is also planned to make dock windows
 * and showing special widgets workbench-dependent.
 *
 * FreeCAD provides also the possibility to define pure Python workbenches. Such workbenches are temporarily only and are lost after exiting
 * the FreeCAD session. But if you want to keep your Pyhton workbench you can write a macro and attach it with a user defined button or just
 * perform the macro during the next FreeCAD session.
 * Here follows a short example of how to create and embed a workbench in Python
 * \code
 * w=FreeCADGui.CreateWorkbench("My workbench")               # creates a standard workbench (the same as StdWorkbench in C++)
 * dir(w)                                                     # lists all available function of the object
 * list = ["Std_Test1", "Std_Test2", "Std_Test3"]             # creates a list of new functions
 * w.AppendMenu("Test functions", list)                       # creates a new menu with these functions
 * w.AppendToolbar("Test", list)                              # ... and also a new toolbar
 * FreeCADGui.addWorkbench("My workbench", 0)                 # Creates an item for our workbenmch now, the second argument can be 0 as we don't
 *                                                            # need our own workbench handler.
 * \endcode
 */

TYPESYSTEM_SOURCE_ABSTRACT(Gui::Workbench, Base::BaseClass)

Workbench::Workbench()
  : _name("")
{
}

Workbench::~Workbench()
{
}

std::string Workbench::name() const
{
    return _name;
}

void Workbench::setName(const std::string& name)
{
    _name = name;
}

void Workbench::setupCustomToolbars(ToolBarItem* root, const char* toolbar) const
{
    std::string name = this->name();
    ParameterGrp::handle hGrp = App::GetApplication().GetUserParameter().GetGroup("BaseApp")
        ->GetGroup("Workbench")->GetGroup(name.c_str())->GetGroup(toolbar);
  
    std::vector<Base::Reference<ParameterGrp> > hGrps = hGrp->GetGroups();
    CommandManager& rMgr = Application::Instance->commandManager();
    for (std::vector<Base::Reference<ParameterGrp> >::iterator it = hGrps.begin(); it != hGrps.end(); ++it) {
        bool active = (*it)->GetBool("Active", true);
        if (!active) // ignore this toolbar
            continue;
        ToolBarItem* bar = new ToolBarItem(root);
        bar->setCommand("Custom");
   
        // get the elements of the subgroups
        std::vector<std::pair<std::string,std::string> > items = hGrp->GetGroup((*it)->GetGroupName())->GetASCIIMap();
        for (std::vector<std::pair<std::string,std::string> >::iterator it2 = items.begin(); it2 != items.end(); ++it2) {
            if (it2->first == "Separator") {
                *bar << "Separator";
            } else if (it2->first == "Name") {
                bar->setCommand(it2->second);
            } else {
                Command* pCmd = rMgr.getCommandByName(it2->first.c_str());
                if (!pCmd) { // unknown command
                    // try to find out the appropriate module name
                    std::string pyMod = it2->second + "Gui";
                    try {
                        Base::Interpreter().loadModule(pyMod.c_str());
                    }
                    catch(const Base::Exception&) {
                    }

                    // Try again
                    pCmd = rMgr.getCommandByName(it2->first.c_str());
                }

                if (pCmd) {
                    *bar << it2->first; // command name
                }
            }
        }
    }
}

void Workbench::setupCustomShortcuts() const
{
    // Assigns user defined accelerators
    ParameterGrp::handle hGrp = WindowParameter::getDefaultParameter();
    if ( hGrp->HasGroup("Shortcut") ) {
        hGrp = hGrp->GetGroup("Shortcut");
        // Get all user defined shortcuts
        const CommandManager& cCmdMgr = Application::Instance->commandManager();
        std::vector<std::pair<std::string,std::string> > items = hGrp->GetASCIIMap();
        for ( std::vector<std::pair<std::string,std::string> >::iterator it = items.begin(); it != items.end(); ++it )
        {
            Command* cmd = cCmdMgr.getCommandByName(it->first.c_str());
            if (cmd && cmd->getAction())
            {
                QString str = it->second.c_str();
                QKeySequence shortcut = str;
                cmd->getAction()->setShortcut(shortcut);
            }
        }
    }
}

void Workbench::setupContextMenu(const char* recipient,MenuItem* item) const
{
}

void Workbench::activated()
{
}

void Workbench::deactivated()
{
}

bool Workbench::activate()
{
    ToolBarItem* tb = setupToolBars();
    setupCustomToolbars(tb, "Toolbar");
    ToolBarManager::getInstance()->setup( tb );
    delete tb;

    ToolBarItem* cb = setupCommandBars();
    setupCustomToolbars(cb, "Toolboxbar");
    //ToolBoxManager::getInstance()->setup( cb );
    delete cb;

    DockWindowItems* dw = setupDockWindows();
    DockWindowManager::instance()->setup( dw );
    delete dw;

    MenuItem* mb = setupMenuBar();
    MenuManager::getInstance()->setup( mb );
    delete mb;

    setupCustomShortcuts();

    return true;
}

void Workbench::retranslate() const
{
    ToolBarManager::getInstance()->retranslate();
    //ToolBoxManager::getInstance()->retranslate();
    DockWindowManager::instance()->retranslate();
    MenuManager::getInstance()->retranslate();
}

PyObject* Workbench::getPyObject()
{
    return new WorkbenchPy(this);
}

// --------------------------------------------------------------------

TYPESYSTEM_SOURCE(Gui::StdWorkbench, Gui::Workbench)

StdWorkbench::StdWorkbench()
  : Workbench()
{
}

StdWorkbench::~StdWorkbench()
{
}

void StdWorkbench::setupContextMenu(const char* recipient,MenuItem* item) const
{
    if (strcmp(recipient,"View") == 0)
    {
        MenuItem* StdViews = new MenuItem;
        StdViews->setCommand( "Standard views" );

        *StdViews << "Std_ViewAxo" << "Separator" << "Std_ViewFront" << "Std_ViewTop" << "Std_ViewRight"
                  << "Std_ViewRear" << "Std_ViewBottom" << "Std_ViewLeft";

        *item << "Std_ViewFitAll" << "Std_ViewFitSelection" << StdViews
              << "Separator" << "Std_ViewDockUndockFullscreen";

        if ( Gui::Selection().countObjectsOfType(App::DocumentObject::getClassTypeId()) > 0 )
            *item << "Separator" << "Std_SetMaterial" << "Std_ToggleVisibility" << "Std_TreeSelection" 
                  << "Std_RandomColor" << "Separator" << "Std_Delete";
    }
    else if (strcmp(recipient,"Tree") == 0)
    {
        if ( Gui::Selection().countObjectsOfType(App::DocumentObject::getClassTypeId()) > 0 )
            *item << "Std_SetMaterial" << "Std_ToggleVisibility" 
                  << "Std_RandomColor" << "Separator" << "Std_Delete";
    }
}

MenuItem* StdWorkbench::setupMenuBar() const
{
    // Setup the default menu bar
    MenuItem* menuBar = new MenuItem;

    // File
    MenuItem* file = new MenuItem( menuBar );
    file->setCommand(QT_TR_NOOP("&File"));
    *file << "Std_New" << "Std_Open" << "Std_Import" << "Std_Save" << "Std_SaveAs"  << "Std_ProjectInfo" 
          << "Separator" << "Std_Print" << "Std_PrintPdf"
          << "Separator" << "Std_RecentFiles" << "Separator" << "Std_Quit";

    // Edit
    MenuItem* edit = new MenuItem( menuBar );
    edit->setCommand(QT_TR_NOOP("&Edit"));
    *edit << "Std_Undo" << "Std_Redo" << "Separator" << "Std_Cut" << "Std_Copy" << "Std_Paste" 
          << "Separator" << "Std_Refresh" << "Std_SelectAll" << "Std_Delete"
          << "Separator" << "Std_DlgPreferences";

    // Standard views
    MenuItem* stdviews = new MenuItem;
    stdviews->setCommand(QT_TR_NOOP("Standard views"));
    *stdviews << "Std_ViewFitAll" << "Std_ViewFitSelection" << "Std_ViewAxo"
              << "Separator" << "Std_ViewFront" << "Std_ViewRight"
              << "Std_ViewTop" << "Separator" << "Std_ViewRear" 
              << "Std_ViewLeft" << "Std_ViewBottom";

    // stereo
    MenuItem* view3d = new MenuItem;
    view3d->setCommand(QT_TR_NOOP("&3D View"));
    *view3d << "Std_ViewIvStereoRedGreen" << "Std_ViewIvStereoQuadBuff" 
            << "Std_ViewIvStereoInterleavedRows" << "Std_ViewIvStereoInterleavedColumns" 
            << "Std_ViewIvStereoOff" << "Separator" << "Std_ViewIvIssueCamPos";

    // zoom
    MenuItem* zoom = new MenuItem;
    zoom->setCommand(QT_TR_NOOP("&Zoom"));
    *zoom << "Std_ViewZoomIn" << "Std_ViewZoomOut" << "Separator" << "Std_ViewBoxZoom";

    // View
    MenuItem* view = new MenuItem( menuBar );
    view->setCommand(QT_TR_NOOP("&View"));
    *view << "Std_ViewCreate" << "Std_OrthographicCamera" << "Std_PerspectiveCamera" << "Separator" 
          << stdviews << "Std_FreezeViews" << "Separator" << view3d << zoom
          << "Std_ViewDockUndockFullscreen" << "Std_ToggleClipPlane" << "Separator"
          << "Std_SetMaterial" << "Std_ToggleVisibility" << "Std_RandomColor" << "Separator" 
          << "Std_MeasureDistance" << "Separator" 
          << "Std_Workbench" << "Std_ToolBarMenu" << "Std_DockViewMenu" << "Separator" 
          << "Std_ViewStatusBar" << "Std_UserInterface";

    // Tools
    MenuItem* tool = new MenuItem( menuBar );
    tool->setCommand(QT_TR_NOOP("&Tools"));
    *tool << "Std_CommandLine" << "Std_DlgParameter" << "Separator" << "Std_DlgMacroRecord"
          << "Std_DlgMacroStop" << "Std_DlgMacroExecute" << "Std_DlgMacroExecuteDirect" 
          << "Separator" << "Std_ViewScreenShot" << "Separator" << "Std_DlgCustomize";

    // Windows
    MenuItem* wnd = new MenuItem( menuBar );
    wnd->setCommand(QT_TR_NOOP("&Windows"));
    *wnd << "Std_CloseActiveWindow" << "Std_CloseAllWindows" << "Separator" << "Std_ActivateNextWindow"
         << "Std_ActivatePrevWindow" << "Separator" << "Std_TileWindows" << "Std_CascadeWindows"
         << "Std_ArrangeIcons" << "Separator" << "Std_WindowsMenu" << "Std_Windows";

    // Separator
    MenuItem* sep = new MenuItem( menuBar );
    sep->setCommand( "Separator" );

    // Help
    MenuItem* helpWebsites = new MenuItem;
    helpWebsites->setCommand(QT_TR_NOOP("&Online-help"));
    *helpWebsites << "Std_OnlineHelpWebsite" << "Std_FreeCADWebsite" << "Std_PythonWebsite";
    
    MenuItem* help = new MenuItem( menuBar );
    help->setCommand(QT_TR_NOOP("&Help"));
    *help << "Std_OnlineHelp" << "Std_OnlineHelpPython" << "Std_PythonHelp"
          << helpWebsites  << "Std_TipOfTheDay"<< "Separator" << "Std_About"
          << "Std_AboutQt" << "Separator" << "Std_WhatsThis" << "Std_DescriptionMode";

    return menuBar;
}

ToolBarItem* StdWorkbench::setupToolBars() const
{
    ToolBarItem* root = new ToolBarItem;

    // File
    ToolBarItem* file = new ToolBarItem( root );
    file->setCommand(QT_TR_NOOP("File"));
    *file << "Std_New" << "Std_Open" << "Std_Save" << "Std_Print" << "Separator" << "Std_Cut"
          << "Std_Copy" << "Std_Paste" << "Separator" << "Std_Undo" << "Std_Redo" << "Separator"
          << "Std_Refresh" << "Separator" << "Std_Workbench" << "Std_WhatsThis";

    // Macro
    ToolBarItem* macro = new ToolBarItem( root );
    macro->setCommand(QT_TR_NOOP("Macro"));
    *macro << "Std_DlgMacroRecord" << "Std_DlgMacroStop" << "Std_DlgMacroExecute"
           << "Std_DlgMacroExecuteDirect";

    // View
    ToolBarItem* view = new ToolBarItem( root );
    view->setCommand(QT_TR_NOOP("View"));
    *view << "Std_ViewFitAll" << "Separator" << "Std_ViewAxo" << "Separator" << "Std_ViewFront" 
          << "Std_ViewRight" << "Std_ViewTop" << "Separator" << "Std_ViewRear" << "Std_ViewLeft" 
          << "Std_ViewBottom";

    return root;
}

ToolBarItem* StdWorkbench::setupCommandBars() const
{
    ToolBarItem* root = new ToolBarItem;

    // View
    ToolBarItem* view = new ToolBarItem( root );
    view->setCommand(QT_TR_NOOP("Standard views"));
    *view << "Std_ViewFitAll" << "Std_ViewFitSelection" << "Std_ViewAxo" << "Separator"
          << "Std_ViewFront" << "Std_ViewRight" << "Std_ViewTop" << "Separator"
          << "Std_ViewRear" << "Std_ViewLeft" << "Std_ViewBottom";
    // Special Ops
    ToolBarItem* macro = new ToolBarItem( root );
    macro->setCommand(QT_TR_NOOP("Special Ops"));
    *macro << "Std_DlgParameter" << "Std_DlgPreferences" << "Std_DlgMacroRecord" << "Std_DlgMacroStop" 
           << "Std_DlgMacroExecute" << "Std_DlgCustomize" << "Std_CommandLine";


    return root;
}

DockWindowItems* StdWorkbench::setupDockWindows() const
{
    DockWindowItems* root = new DockWindowItems();
    root->addDockWidget("Std_ToolBox", Qt::RightDockWidgetArea);
    //root->addDockWidget("Std_HelpView", Qt::RightDockWidgetArea);
    root->addDockWidget("Std_TreeView", Qt::LeftDockWidgetArea);
    root->addDockWidget("Std_PropertyView", Qt::LeftDockWidgetArea);
    root->addDockWidget("Std_SelectionView", Qt::LeftDockWidgetArea);
    root->addDockWidget("Std_ReportView", Qt::BottomDockWidgetArea);
    //root->addDockWidget("Std_PythonView", Qt::BottomDockWidgetArea);
    return root;
}

// --------------------------------------------------------------------

TYPESYSTEM_SOURCE(Gui::NoneWorkbench, Gui::StdWorkbench)

NoneWorkbench::NoneWorkbench()
  : StdWorkbench()
{
}

NoneWorkbench::~NoneWorkbench()
{
}

void NoneWorkbench::setupContextMenu(const char* recipient,MenuItem* item) const
{
}

MenuItem* NoneWorkbench::setupMenuBar() const
{
    // Setup the default menu bar
    MenuItem* menuBar = new MenuItem;

    // File
    MenuItem* file = new MenuItem( menuBar );
    file->setCommand("&File");
    *file << "Std_Quit";

    // View
    MenuItem* view = new MenuItem( menuBar );
    view->setCommand("&View");
    *view << "Std_Workbench";

    // Separator
    MenuItem* sep = new MenuItem( menuBar );
    sep->setCommand("Separator");

    // Help
    MenuItem* help = new MenuItem( menuBar );
    help->setCommand("&Help");
    *help << "Std_OnlineHelp" << "Std_About" << "Std_AboutQt";

    return menuBar;
}

ToolBarItem* NoneWorkbench::setupToolBars() const
{
    ToolBarItem* root = new ToolBarItem;
    return root;
}

ToolBarItem* NoneWorkbench::setupCommandBars() const
{
    ToolBarItem* root = new ToolBarItem;
    return root;
}

DockWindowItems* NoneWorkbench::setupDockWindows() const
{
    return new DockWindowItems();
}

// --------------------------------------------------------------------

TYPESYSTEM_SOURCE(Gui::TestWorkbench, Gui::Workbench)

TestWorkbench::TestWorkbench()
  : StdWorkbench()
{
}

TestWorkbench::~TestWorkbench()
{
}

MenuItem* TestWorkbench::setupMenuBar() const
{
    // Setup the default menu bar
    MenuItem* menuBar = StdWorkbench::setupMenuBar();

    MenuItem* item = menuBar->findItem("&Help");
    item->removeItem(item->findItem("Std_WhatsThis"));

    // Test commands
    MenuItem* test = new MenuItem;
    menuBar->insertItem( item, test );
    test->setCommand( "Test &Commands" );
    *test << "Std_Test1" << "Std_Test2" << "Std_Test3" << "Std_Test4" << "Std_Test5"
          << "Std_Test6" << "Std_Test7" << "Std_Test8";

    // Inventor View
    MenuItem* opiv = new MenuItem;
    menuBar->insertItem( item, opiv );
    opiv->setCommand("&Inventor View");
    *opiv << "Std_ViewExample1" << "Std_ViewExample2" << "Std_ViewExample3";

    return menuBar;
}

ToolBarItem* TestWorkbench::setupToolBars() const
{
    return 0;
}

ToolBarItem* TestWorkbench::setupCommandBars() const
{
    return 0;
}

// -----------------------------------------------------------------------

TYPESYSTEM_SOURCE(Gui::PythonWorkbench, Gui::Workbench)

PythonWorkbench::PythonWorkbench() : _workbenchPy(0)
{
    _menuBar = StdWorkbench::setupMenuBar();
    _contextMenu = new MenuItem;
    _toolBar = StdWorkbench::setupToolBars();
    _commandBar = new ToolBarItem;
}

PythonWorkbench::~PythonWorkbench()
{
    delete _menuBar;
    delete _contextMenu;
    delete _toolBar;
    delete _commandBar;
    if (_workbenchPy)
    {
        _workbenchPy->setInvalid();
        _workbenchPy->DecRef();
    }
}

PyObject* PythonWorkbench::getPyObject()
{
    if (!_workbenchPy)
    {
        _workbenchPy = new PythonWorkbenchPy(this);
    }

    // Increment every time when this object is returned
    _workbenchPy->IncRef();

    return _workbenchPy;
}

MenuItem* PythonWorkbench::setupMenuBar() const
{
    return _menuBar->copy();
}

ToolBarItem* PythonWorkbench::setupToolBars() const
{
    return _toolBar->copy();
}

ToolBarItem* PythonWorkbench::setupCommandBars() const
{
    return _commandBar->copy();
}

void PythonWorkbench::setupContextMenu(const char* recipient, MenuItem* item) const
{
    StdWorkbench::setupContextMenu(recipient, item);
    QList<MenuItem*> items = _contextMenu->getItems();
    for (QList<MenuItem*>::Iterator it = items.begin(); it != items.end(); ++it) {
        item->appendItem((*it)->copy());
    }
}

void PythonWorkbench::appendMenu(const std::list<std::string>& menu, const std::list<std::string>& items) const
{
    if ( menu.empty() || items.empty() )
        return;

    std::list<std::string>::const_iterator jt=menu.begin();
    MenuItem* item = _menuBar->findItem( *jt );
    if (!item)
    {
        Gui::MenuItem* wnd = _menuBar->findItem( "&Windows" );
        item = new MenuItem;
        item->setCommand( *jt );
        _menuBar->insertItem( wnd, item );
    }

    // create sub menus
    for ( jt++; jt != menu.end(); jt++ )
    {
        MenuItem* subitem = item->findItem( *jt );
        if ( !subitem )
        {
            subitem = new MenuItem(item);
            subitem->setCommand( *jt );
        }
        item = subitem;
    }

    for (std::list<std::string>::const_iterator it = items.begin(); it != items.end(); ++it)
        *item << *it;
}

void PythonWorkbench::removeMenu(const std::string& menu) const
{
    MenuItem* item = _menuBar->findItem(menu);
    if ( item ) {
        _menuBar->removeItem(item);
        delete item;
    }
}

std::list<std::string> PythonWorkbench::listMenus() const
{
    std::list<std::string> menus;
    QList<MenuItem*> items = _menuBar->getItems();
    for ( QList<MenuItem*>::ConstIterator it = items.begin(); it != items.end(); ++it )
        menus.push_back((*it)->command());
    return menus;
}

void PythonWorkbench::appendContextMenu(const std::list<std::string>& menu, const std::list<std::string>& items) const
{
    MenuItem* item = _contextMenu;
    for (std::list<std::string>::const_iterator jt=menu.begin();jt!=menu.end();++jt) {
        MenuItem* subitem = item->findItem(*jt);
        if (!subitem) {
            subitem = new MenuItem(item);
            subitem->setCommand(*jt);
        }
        item = subitem;
    }

    for (std::list<std::string>::const_iterator it = items.begin(); it != items.end(); ++it)
        *item << *it;
}

void PythonWorkbench::removeContextMenu(const std::string& menu) const
{
    MenuItem* item = _contextMenu->findItem(menu);
    if (item) {
        _contextMenu->removeItem(item);
        delete item;
    }
}

void PythonWorkbench::clearContextMenu()
{
    _contextMenu->clear();
}

void PythonWorkbench::appendToolbar(const std::string& bar, const std::list<std::string>& items) const
{
    ToolBarItem* item = _toolBar->findItem(bar);
    if (!item)
    {
        item = new ToolBarItem(_toolBar);
        item->setCommand(bar);
    }

    for (std::list<std::string>::const_iterator it = items.begin(); it != items.end(); ++it)
        *item << *it;
}

void PythonWorkbench::removeToolbar(const std::string& bar) const
{
    ToolBarItem* item = _toolBar->findItem(bar);
    if (item) {
        _toolBar->removeItem(item);
        delete item;
    }
}

std::list<std::string> PythonWorkbench::listToolbars() const
{
    std::list<std::string> bars;
    QList<ToolBarItem*> items = _toolBar->getItems();
    for (QList<ToolBarItem*>::ConstIterator item = items.begin(); item != items.end(); ++item)
        bars.push_back((*item)->command());
    return bars;
}

void PythonWorkbench::appendCommandbar(const std::string& bar, const std::list<std::string>& items) const
{
    ToolBarItem* item = _commandBar->findItem( bar );
    if ( !item )
    {
        item = new ToolBarItem(_commandBar);
        item->setCommand(bar);
    }

    for (std::list<std::string>::const_iterator it = items.begin(); it != items.end(); ++it)
        *item << *it;
}

void PythonWorkbench::removeCommandbar(const std::string& bar) const
{
    ToolBarItem* item = _commandBar->findItem(bar);
    if ( item ) {
        _commandBar->removeItem(item);
        delete item;
    }
}

std::list<std::string> PythonWorkbench::listCommandbars() const
{
    std::list<std::string> bars;
    QList<ToolBarItem*> items = _commandBar->getItems();
    for (QList<ToolBarItem*>::ConstIterator item = items.begin(); item != items.end(); ++item)
        bars.push_back((*item)->command());
    return bars;
}
