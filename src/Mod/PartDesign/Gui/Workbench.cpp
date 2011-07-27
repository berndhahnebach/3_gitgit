/***************************************************************************
 *   Copyright (c) 2008 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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

#ifndef _PreComp_
# include <qobject.h>
#endif

#include "Workbench.h"
#include <Gui/MenuManager.h>
#include <Gui/ToolBarManager.h>
#include <Gui/Control.h>

using namespace PartDesignGui;

#if 0 // needed for Qt's lupdate utility
    qApp->translate("Workbench", "Part Design");
#endif

/// @namespace PartDesignGui @class Workbench
TYPESYSTEM_SOURCE(PartDesignGui::Workbench, Gui::StdWorkbench)

Workbench::Workbench()
{
}

Workbench::~Workbench()
{
}

void Workbench::activated()
{
    Gui::Workbench::activated();


    std::vector<Gui::TaskView::TaskWatcher*> Watcher;

    //Watcher.push_back(new Gui::TaskView::TaskWatcherCommands(
    //    "FROM Robot SELECT TrajectoryObject COUNT 1"
    //    "FROM Robot SELECT RobotObject COUNT 1",
    //    RobotAndTrac,
    //    "Trajectory tools",
    //    "Robot_InsertWaypoint"
    //));

    //Watcher.push_back(new TaskWatcherRobot);

    const char* Face[] = {
        "Sketcher_NewSketch",
        "PartDesign_Fillet",
        "PartDesign_Chamfer",
        0};
    Watcher.push_back(new Gui::TaskView::TaskWatcherCommands(
        "SELECT Part::Feature SUBELEMENT Face COUNT 1",
        Face,
        "Face tools",
        "Part_Box"
    ));

    const char* Sketch[] = {
        "Sketcher_NewSketch",
        "PartDesign_Pad",
        "PartDesign_Pocket",
        0};
    Watcher.push_back(new Gui::TaskView::TaskWatcherCommands(
        "SELECT Sketcher::SketchObject COUNT 1",
        Sketch,
        "Sketch tools",
        "Part_Box"
    ));

    const char* Empty[] = {
        "Sketcher_NewSketch",
        "Part_Box",
        "Part_Cylinder",
        0};
   Watcher.push_back(new Gui::TaskView::TaskWatcherCommandsEmptyDoc(
         Empty,
        "Create Geometry",
        "Part_Box"
    ));

    
    addTaskWatcher(Watcher);
    Gui::Control().showTaskView();
}

void Workbench::deactivated()
{
    Gui::Workbench::deactivated();
    removeTaskWatcher();

}

Gui::MenuItem* Workbench::setupMenuBar() const
{
    Gui::MenuItem* root = StdWorkbench::setupMenuBar();
    Gui::MenuItem* item = root->findItem("&Windows");

    Gui::MenuItem* geom = new Gui::MenuItem();
    geom->setCommand("Sketcher geoms");
    *geom /*<< "Sketcher_CreatePoint"*/
          << "Sketcher_CreateArc"
          << "Sketcher_CreateCircle"
          << "Sketcher_CreateLine"
          << "Sketcher_CreatePolyline"
          << "Sketcher_CreateRectangle"
          /*<< "Sketcher_CreateText"*/
          /*<< "Sketcher_CreateDraftLine"*/;

    Gui::MenuItem* cons = new Gui::MenuItem();
    cons->setCommand("Sketcher constraints");
    *cons /*<< "Sketcher_ConstrainLock"*/
          << "Sketcher_ConstrainCoincident"
          << "Sketcher_ConstrainVertical"
          << "Sketcher_ConstrainHorizontal"
          << "Sketcher_ConstrainDistance"
          << "Sketcher_ConstrainParallel"
          << "Sketcher_ConstrainTangent"
          << "Sketcher_ConstrainPerpendicular"

          ;

    Gui::MenuItem* part = new Gui::MenuItem;
    root->insertItem(item, part);
    part->setCommand("&Part Design");
    *part << "Sketcher_NewSketch" 
          << geom
          << cons
          << "Separator" 
          << "PartDesign_Pad" 
          << "PartDesign_Pocket"
          << "PartDesign_Fillet"
          << "PartDesign_Chamfer";

    return root;
}

Gui::ToolBarItem* Workbench::setupToolBars() const
{
    Gui::ToolBarItem* root = StdWorkbench::setupToolBars();
    Gui::ToolBarItem* part = new Gui::ToolBarItem(root);
    part->setCommand("Part Design");
    *part << "Sketcher_NewSketch" 
          << "Separator" 
          << "PartDesign_Pad" 
          << "PartDesign_Pocket" 
          << "PartDesign_Fillet"
          << "PartDesign_Chamfer";
       
    part = new Gui::ToolBarItem(root);
    part->setCommand("Sketcher geoms");
    *part /*<< "Sketcher_CreatePoint" */
		  << "Sketcher_CreateArc"
		  << "Sketcher_CreateCircle"
		  << "Sketcher_CreateLine"
		  << "Sketcher_CreatePolyline"
		  << "Sketcher_CreateRectangle"
		  /*<< "Sketcher_CreateText"*/
		  /*<< "Sketcher_CreateDraftLine"*/;

    part = new Gui::ToolBarItem(root);
    part->setCommand("Sketcher constraints");
    *part << "Sketcher_ConstrainLock"
          << "Sketcher_ConstrainCoincident"
          << "Sketcher_ConstrainVertical"
          << "Sketcher_ConstrainHorizontal"
          << "Sketcher_ConstrainDistance"
          << "Sketcher_ConstrainParallel"
          << "Sketcher_ConstrainTangent"
          << "Sketcher_ConstrainPerpendicular"
          ;

     return root;
}

Gui::ToolBarItem* Workbench::setupCommandBars() const
{
    // Part tools
    Gui::ToolBarItem* root = new Gui::ToolBarItem;
    return root;
}

