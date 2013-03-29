/***************************************************************************
 *   Copyright (c) 2008 Jürgen Riegel (juergen.riegel@web.de)              *
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
# include <BRep_Tool.hxx>
# include <TopExp_Explorer.hxx>
# include <TopoDS.hxx>
# include <TopoDS_Edge.hxx>
# include <TopoDS_Shape.hxx>
# include <TopoDS_Face.hxx>
# include <TopExp.hxx>
# include <TopTools_ListOfShape.hxx>
# include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
# include <TopTools_IndexedMapOfShape.hxx>
# include <BRepAdaptor_Surface.hxx>
# include <QMessageBox>
#endif

#include <sstream>
#include <algorithm>

#include <App/DocumentObjectGroup.h>
#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/Control.h>
#include <Gui/Selection.h>
#include <Gui/MainWindow.h>
#include <Gui/FileDialog.h>
#include <Gui/SelectionFilter.h>

#include <Mod/Part/App/Part2DObject.h>
#include <Mod/PartDesign/App/Body.h>
#include <Mod/Sketcher/App/SketchObject.h>
#include <Mod/Sketcher/Gui/SketchOrientationDialog.h>
#include <Mod/PartDesign/App/FeatureGroove.h>
#include <Mod/PartDesign/App/FeatureRevolution.h>

#include "FeaturePickDialog.h"

using namespace std;

extern PartDesign::Body *ActivePartObject;


//===========================================================================
// Helper for Body
//===========================================================================

PartDesign::Body *getBody(void)
{
    if(!ActivePartObject){
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("No active Body"),
            QObject::tr("In order to use PartDesign you need an active Body object in the document. "
                        "Please make one active or create one. If you have a legacy document "
                        "with PartDesign objects without Body, use the transfer function in "
                        "PartDesign to put them into a Body."
                        ));
    }
    return ActivePartObject;

}


//===========================================================================
// PartDesign_Sketch
//===========================================================================

/* Sketch commands =======================================================*/
DEF_STD_CMD_A(CmdPartDesignNewSketch);

CmdPartDesignNewSketch::CmdPartDesignNewSketch()
  :Command("PartDesign_NewSketch")
{
    sAppModule      = "PartDesign";
    sGroup          = QT_TR_NOOP("PartDesign");
    sMenuText       = QT_TR_NOOP("Create sketch");
    sToolTipText    = QT_TR_NOOP("Create a new sketch");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_NewSketch";
}


void CmdPartDesignNewSketch::activated(int iMsg)
{
    PartDesign::Body *pcActiveBody = getBody();

    // No PartDesign feature without Body past FreeCAD 0.13
    if(!pcActiveBody) return;

    Gui::SelectionFilter SketchFilter("SELECT Sketcher::SketchObject COUNT 1");
    Gui::SelectionFilter FaceFilter  ("SELECT Part::Feature SUBELEMENT Face COUNT 1");

    if (SketchFilter.match()) {
        Sketcher::SketchObject *Sketch = static_cast<Sketcher::SketchObject*>(SketchFilter.Result[0][0].getObject());
        openCommand("Edit Sketch");
        doCommand(Gui,"Gui.activeDocument().setEdit('%s')",Sketch->getNameInDocument());
    }
    else if (FaceFilter.match()) {
        // get the selected object
        Part::Feature *part = static_cast<Part::Feature*>(FaceFilter.Result[0][0].getObject());
        Base::Placement ObjectPos = part->Placement.getValue();
        const std::vector<std::string> &sub = FaceFilter.Result[0][0].getSubNames();
        if (sub.size() > 1){
            // No assert for wrong user input!
            QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Several sub-elements selected"),
                QObject::tr("You have to select a single face as support for a sketch!"));
            return;
        }
        // get the selected sub shape (a Face)
        const Part::TopoShape &shape = part->Shape.getValue();
        TopoDS_Shape sh = shape.getSubShape(sub[0].c_str());
        const TopoDS_Face& face = TopoDS::Face(sh);
        if (face.IsNull()){
            // No assert for wrong user input!
            QMessageBox::warning(Gui::getMainWindow(), QObject::tr("No support face selected"),
                QObject::tr("You have to select a face as support for a sketch!"));
            return;
        }

        BRepAdaptor_Surface adapt(face);
        if (adapt.GetType() != GeomAbs_Plane){
            QMessageBox::warning(Gui::getMainWindow(), QObject::tr("No planar support"),
                QObject::tr("You need a planar face as support for a sketch!"));
            return;
        }

        std::string supportString = FaceFilter.Result[0][0].getAsPropertyLinkSubString();

        // create Sketch on Face
        std::string FeatName = getUniqueObjectName("Sketch");

        openCommand("Create a Sketch on Face");
        doCommand(Doc,"App.activeDocument().addObject('Sketcher::SketchObject','%s')",FeatName.c_str());
        doCommand(Doc,"App.activeDocument().%s.Model = App.activeDocument().%s.Model + [App.activeDocument().%s]",pcActiveBody->getNameInDocument(),pcActiveBody->getNameInDocument(),FeatName.c_str());
        doCommand(Doc,"App.activeDocument().%s.Tip = App.activeDocument().%s",pcActiveBody->getNameInDocument(),FeatName.c_str());
        doCommand(Gui,"App.activeDocument().%s.Support = %s",FeatName.c_str(),supportString.c_str());
        doCommand(Gui,"App.activeDocument().recompute()");  // recompute the sketch placement based on its support
        //doCommand(Gui,"Gui.activeDocument().activeView().setCamera('%s')",cam.c_str());
        doCommand(Gui,"Gui.activeDocument().setEdit('%s')",FeatName.c_str());
    }
    else {
        // ask user for orientation
        SketcherGui::SketchOrientationDialog Dlg;

        if (Dlg.exec() != QDialog::Accepted)
            return; // canceled
        Base::Vector3d p = Dlg.Pos.getPosition();
        Base::Rotation r = Dlg.Pos.getRotation();

        // do the right view direction
        std::string camstring;
        switch(Dlg.DirType){
            case 0:
                camstring = "#Inventor V2.1 ascii \\n OrthographicCamera {\\n viewportMapping ADJUST_CAMERA \\n position 0 0 87 \\n orientation 0 0 1  0 \\n nearDistance -112.88701 \\n farDistance 287.28702 \\n aspectRatio 1 \\n focalDistance 87 \\n height 143.52005 }";
                break;
            case 1:
                camstring = "#Inventor V2.1 ascii \\n OrthographicCamera {\\n viewportMapping ADJUST_CAMERA \\n position 0 0 -87 \\n orientation -1 0 0  3.1415927 \\n nearDistance -112.88701 \\n farDistance 287.28702 \\n aspectRatio 1 \\n focalDistance 87 \\n height 143.52005 }";
                break;
            case 2:
                camstring = "#Inventor V2.1 ascii \\n OrthographicCamera {\\n viewportMapping ADJUST_CAMERA\\n  position 0 -87 0 \\n  orientation -1 0 0  4.712389\\n  nearDistance -112.88701\\n  farDistance 287.28702\\n  aspectRatio 1\\n  focalDistance 87\\n  height 143.52005\\n\\n}";
                break;
            case 3:
                camstring = "#Inventor V2.1 ascii \\n OrthographicCamera {\\n viewportMapping ADJUST_CAMERA\\n  position 0 87 0 \\n  orientation 0 0.70710683 0.70710683  3.1415927\\n  nearDistance -112.88701\\n  farDistance 287.28702\\n  aspectRatio 1\\n  focalDistance 87\\n  height 143.52005\\n\\n}";
                break;
            case 4:
                camstring = "#Inventor V2.1 ascii \\n OrthographicCamera {\\n viewportMapping ADJUST_CAMERA\\n  position 87 0 0 \\n  orientation 0.57735026 0.57735026 0.57735026  2.0943952 \\n  nearDistance -112.887\\n  farDistance 287.28699\\n  aspectRatio 1\\n  focalDistance 87\\n  height 143.52005\\n\\n}";
                break;
            case 5:
                camstring = "#Inventor V2.1 ascii \\n OrthographicCamera {\\n viewportMapping ADJUST_CAMERA\\n  position -87 0 0 \\n  orientation -0.57735026 0.57735026 0.57735026  4.1887903 \\n  nearDistance -112.887\\n  farDistance 287.28699\\n  aspectRatio 1\\n  focalDistance 87\\n  height 143.52005\\n\\n}";
                break;
        }
        std::string FeatName = getUniqueObjectName("Sketch");

        openCommand("Create a new Sketch");
        doCommand(Doc,"App.activeDocument().addObject('Sketcher::SketchObject','%s')",FeatName.c_str());
        doCommand(Doc,"App.activeDocument().%s.Placement = App.Placement(App.Vector(%f,%f,%f),App.Rotation(%f,%f,%f,%f))",FeatName.c_str(),p.x,p.y,p.z,r[0],r[1],r[2],r[3]);
        doCommand(Doc,"App.activeDocument().%s.Model = App.activeDocument().%s.Model + [App.activeDocument().%s]",pcActiveBody->getNameInDocument(),pcActiveBody->getNameInDocument(),FeatName.c_str());
        doCommand(Doc,"App.activeDocument().%s.Tip = App.activeDocument().%s",pcActiveBody->getNameInDocument(),FeatName.c_str());
        doCommand(Gui,"Gui.activeDocument().activeView().setCamera('%s')",camstring.c_str());
        doCommand(Gui,"Gui.activeDocument().setEdit('%s')",FeatName.c_str());
    }

}

bool CmdPartDesignNewSketch::isActive(void)
{
    if (getActiveGuiDocument())
        return true;
    else
        return false;
}

//===========================================================================
// Common utility functions for SketchBased features
//===========================================================================

// Take a list of Part2DObjects and erase those which are not eligible for creating a
// SketchBased feature. If supportRequired is true, also erase those that cannot be used to define
// a Subtractive feature
void validateSketches(std::vector<App::DocumentObject*>& sketches, const bool supportRequired)
{
    std::vector<App::DocumentObject*>::iterator s = sketches.begin();

    while (s != sketches.end()) {
        // sketch is always part of the body first.
        //// Check whether this sketch is already being used by another feature
        //std::vector<App::DocumentObject*> ref = (*s)->getInList();
        //std::vector<App::DocumentObject*>::iterator r = ref.begin();
        //while (r != ref.end()) {
        //    if (!(*r)->getTypeId().isDerivedFrom(PartDesign::SketchBased().getClassTypeId())) {
        //        r = ref.erase(r);
        //        continue;
        //    }
        //    ++r;
        //}
        //if (!ref.empty()) {
        //    // TODO: Display some information message that this sketch was removed?
        //    s = sketches.erase(s);
        //    continue;
        //}

        // Check whether the sketch shape is valid
        Part::Part2DObject* sketch = static_cast<Part::Part2DObject*>(*s);
        const TopoDS_Shape& shape = sketch->Shape.getValue();
        if (shape.IsNull()) {
            s = sketches.erase(s);
            continue;
            // TODO: Display some information message that this sketch was removed?
        }

        // count free wires
        int ctWires=0;
        TopExp_Explorer ex;
        for (ex.Init(shape, TopAbs_WIRE); ex.More(); ex.Next()) {
            ctWires++;
        }
        if (ctWires == 0) {
            s = sketches.erase(s);
            continue;
            // TODO: Display some information message that this sketch was removed?
        }

        // Check for support
        if (supportRequired) {
            App::DocumentObject* support = sketch->Support.getValue();
            if (support == NULL) {
                s = sketches.erase(s);
                continue;
                // TODO: Display some information message that this sketch was removed?
            }
        }

        // All checks passed - go on to next candidate
        s++;
    }
}

void prepareSketchBased(Gui::Command* cmd, const std::string& which,
                        Part::Part2DObject*& sketch, std::string& FeatName)
{
    PartDesign::Body *pcActiveBody = getBody();
    if (!pcActiveBody) return;

    bool bNoSketchWasSelected = false;
    // Get a valid sketch from the user
    // First check selections
    std::vector<App::DocumentObject*> sketches = cmd->getSelection().getObjectsOfType(Part::Part2DObject::getClassTypeId());
    validateSketches(sketches, false);
    // Next let the user choose from a list of all eligible objects
    if (sketches.size() == 0) {
        sketches = cmd->getDocument()->getObjectsOfType(Part::Part2DObject::getClassTypeId());
        bNoSketchWasSelected = true;
    }
    if (sketches.size() == 0) {
        if (bNoSketchWasSelected) {
            QMessageBox::warning(Gui::getMainWindow(), QObject::tr("No valid sketches in this document"),
                QObject::tr("Please create a sketch or 2D object first."));
        } else {
            QMessageBox::warning(Gui::getMainWindow(), QObject::tr("No valid sketches selected"),
                QObject::tr("None of selected sketches/2D objects is valid for padding. Please select a valid sketch or 2D object that is not used by any other feature."));
        }
        return;
    }

    // If there is more than one selection/possibility, show dialog and let user pick sketch
    if (sketches.size() > 1) {
        PartDesignGui::FeaturePickDialog Dlg(sketches);
        if ((Dlg.exec() != QDialog::Accepted) || (sketches = Dlg.getFeatures()).empty())
            return; // Cancelled or nothing selected
    }

    sketch = static_cast<Part::Part2DObject*>(sketches.front());
    FeatName = cmd->getUniqueObjectName(which.c_str());

    cmd->openCommand((std::string("Make ") + which).c_str());
    cmd->doCommand(cmd->Doc,"App.activeDocument().addObject(\"PartDesign::%s\",\"%s\")",which.c_str(), FeatName.c_str());
    cmd->doCommand(cmd->Doc,"App.activeDocument().%s.Model = App.activeDocument().%s.Model + [App.activeDocument().%s]",pcActiveBody->getNameInDocument(),pcActiveBody->getNameInDocument(),FeatName.c_str());
    cmd->doCommand(cmd->Doc,"App.activeDocument().%s.Tip = App.activeDocument().%s",pcActiveBody->getNameInDocument(),FeatName.c_str());
    cmd->doCommand(cmd->Doc,"App.activeDocument().%s.Sketch = App.activeDocument().%s",FeatName.c_str(),sketch->getNameInDocument());
}

void finishSketchBased(const Gui::Command* cmd, const Part::Part2DObject* sketch, const std::string& FeatName)
{
	App::DocumentObjectGroup* grp = sketch->getGroup();
    if (grp) {
        cmd->doCommand(cmd->Doc,"App.activeDocument().%s.addObject(App.activeDocument().%s)"
                     ,grp->getNameInDocument(),FeatName.c_str());
        cmd->doCommand(cmd->Doc,"App.activeDocument().%s.removeObject(App.activeDocument().%s)"
                     ,grp->getNameInDocument(),sketch->getNameInDocument());
    }

    cmd->updateActive();
    if (cmd->isActiveObjectValid()) {
        cmd->doCommand(cmd->Gui,"Gui.activeDocument().hide(\"%s\")", sketch->getNameInDocument());
	}
    // #0001721: use '0' as edit value to avoid switching off selection in
    // ViewProviderGeometryObject::setEditViewer
	cmd->doCommand(cmd->Gui,"Gui.activeDocument().setEdit('%s', 0)", FeatName.c_str());

    /*
    PartDesign::Body *pcActiveBody = getBody();
    if (pcActiveBody) {
        cmd->copyVisual(FeatName.c_str(), "ShapeColor", pcActiveBody->getNameInDocument());
        cmd->copyVisual(FeatName.c_str(), "LineColor", pcActiveBody->getNameInDocument());
        cmd->copyVisual(FeatName.c_str(), "PointColor", pcActiveBody->getNameInDocument());
    }*/
}

//===========================================================================
// PartDesign_Pad
//===========================================================================
DEF_STD_CMD_A(CmdPartDesignPad);

CmdPartDesignPad::CmdPartDesignPad()
  : Command("PartDesign_Pad")
{
    sAppModule    = "PartDesign";
    sGroup        = QT_TR_NOOP("PartDesign");
    sMenuText     = QT_TR_NOOP("Pad");
    sToolTipText  = QT_TR_NOOP("Pad a selected sketch");
    sWhatsThis    = "PartDesign_Pad";
    sStatusTip    = sToolTipText;
    sPixmap       = "PartDesign_Pad";
}

void CmdPartDesignPad::activated(int iMsg)
{
    Part::Part2DObject* sketch;
    std::string FeatName;
    prepareSketchBased(this, "Pad", sketch, FeatName);

    // specific parameters for Pad
    doCommand(Doc,"App.activeDocument().%s.Length = 10.0",FeatName.c_str());

    finishSketchBased(this, sketch, FeatName);
    adjustCameraPosition();
}

bool CmdPartDesignPad::isActive(void)
{
    return hasActiveDocument();
}

//===========================================================================
// PartDesign_Pocket
//===========================================================================
DEF_STD_CMD_A(CmdPartDesignPocket);

CmdPartDesignPocket::CmdPartDesignPocket()
  : Command("PartDesign_Pocket")
{
    sAppModule    = "PartDesign";
    sGroup        = QT_TR_NOOP("PartDesign");
    sMenuText     = QT_TR_NOOP("Pocket");
    sToolTipText  = QT_TR_NOOP("Create a pocket with the selected sketch");
    sWhatsThis    = "PartDesign_Pocket";
    sStatusTip    = sToolTipText;
    sPixmap       = "PartDesign_Pocket";
}

void CmdPartDesignPocket::activated(int iMsg)
{
    Part::Part2DObject* sketch;
    std::string FeatName;
    prepareSketchBased(this, "Pocket", sketch, FeatName);

    doCommand(Doc,"App.activeDocument().%s.Length = 5.0",FeatName.c_str());

    finishSketchBased(this, sketch, FeatName);
    adjustCameraPosition();
}

bool CmdPartDesignPocket::isActive(void)
{
    return hasActiveDocument();
}

//===========================================================================
// PartDesign_Revolution
//===========================================================================
DEF_STD_CMD_A(CmdPartDesignRevolution);

CmdPartDesignRevolution::CmdPartDesignRevolution()
  : Command("PartDesign_Revolution")
{
    sAppModule    = "PartDesign";
    sGroup        = QT_TR_NOOP("PartDesign");
    sMenuText     = QT_TR_NOOP("Revolution");
    sToolTipText  = QT_TR_NOOP("Revolve a selected sketch");
    sWhatsThis    = "PartDesign_Revolution";
    sStatusTip    = sToolTipText;
    sPixmap       = "PartDesign_Revolution";
}

void CmdPartDesignRevolution::activated(int iMsg)
{
    Part::Part2DObject* sketch;
    std::string FeatName;
    prepareSketchBased(this, "Revolution", sketch, FeatName);

    doCommand(Doc,"App.activeDocument().%s.ReferenceAxis = (App.activeDocument().%s,['V_Axis'])",
                                                                             FeatName.c_str(), sketch->getNameInDocument());
    doCommand(Doc,"App.activeDocument().%s.Angle = 360.0",FeatName.c_str());
    PartDesign::Revolution* pcRevolution = static_cast<PartDesign::Revolution*>(getDocument()->getObject(FeatName.c_str()));
    if (pcRevolution && pcRevolution->suggestReversed())
        doCommand(Doc,"App.activeDocument().%s.Reversed = 1",FeatName.c_str());

    finishSketchBased(this, sketch, FeatName);
    adjustCameraPosition();
}

bool CmdPartDesignRevolution::isActive(void)
{
    return hasActiveDocument();
}

//===========================================================================
// PartDesign_Groove
//===========================================================================
DEF_STD_CMD_A(CmdPartDesignGroove);

CmdPartDesignGroove::CmdPartDesignGroove()
  : Command("PartDesign_Groove")
{
    sAppModule    = "PartDesign";
    sGroup        = QT_TR_NOOP("PartDesign");
    sMenuText     = QT_TR_NOOP("Groove");
    sToolTipText  = QT_TR_NOOP("Groove a selected sketch");
    sWhatsThis    = "PartDesign_Groove";
    sStatusTip    = sToolTipText;
    sPixmap       = "PartDesign_Groove";
}

void CmdPartDesignGroove::activated(int iMsg)
{
    Part::Part2DObject* sketch;
    std::string FeatName;
    prepareSketchBased(this, "Groove", sketch, FeatName);

    doCommand(Doc,"App.activeDocument().%s.ReferenceAxis = (App.activeDocument().%s,['V_Axis'])",
                                                                             FeatName.c_str(), sketch->getNameInDocument());
    doCommand(Doc,"App.activeDocument().%s.Angle = 360.0",FeatName.c_str());
    PartDesign::Groove* pcGroove = static_cast<PartDesign::Groove*>(getDocument()->getObject(FeatName.c_str()));
    if (pcGroove && pcGroove->suggestReversed())
        doCommand(Doc,"App.activeDocument().%s.Reversed = 1",FeatName.c_str());

    finishSketchBased(this, sketch, FeatName);
    adjustCameraPosition();
}

bool CmdPartDesignGroove::isActive(void)
{
    return hasActiveDocument();
}

//===========================================================================
// PartDesign_Fillet
//===========================================================================
DEF_STD_CMD_A(CmdPartDesignFillet);

CmdPartDesignFillet::CmdPartDesignFillet()
  :Command("PartDesign_Fillet")
{
    sAppModule    = "PartDesign";
    sGroup        = QT_TR_NOOP("PartDesign");
    sMenuText     = QT_TR_NOOP("Fillet");
    sToolTipText  = QT_TR_NOOP("Make a fillet on an edge, face or body");
    sWhatsThis    = "PartDesign_Fillet";
    sStatusTip    = sToolTipText;
    sPixmap       = "PartDesign_Fillet";
}

void CmdPartDesignFillet::activated(int iMsg)
{
    std::vector<Gui::SelectionObject> selection = getSelection().getSelectionEx();

    if (selection.size() != 1) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select an edge, face or body. Only one body is allowed."));
        return;
    }

    if (!selection[0].isObjectTypeOf(Part::Feature::getClassTypeId())){
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong object type"),
            QObject::tr("Fillet works only on parts."));
        return;
    }

    Part::Feature *base = static_cast<Part::Feature*>(selection[0].getObject());

    const Part::TopoShape& TopShape = base->Shape.getShape();
    if (TopShape._Shape.IsNull()){
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
                QObject::tr("Shape of selected part is empty."));
        return;
    }

    TopTools_IndexedMapOfShape mapOfEdges;
    TopTools_IndexedDataMapOfShapeListOfShape mapEdgeFace;
    TopExp::MapShapesAndAncestors(TopShape._Shape, TopAbs_EDGE, TopAbs_FACE, mapEdgeFace);
    TopExp::MapShapes(TopShape._Shape, TopAbs_EDGE, mapOfEdges);

    std::vector<std::string> SubNames = std::vector<std::string>(selection[0].getSubNames());

    unsigned int i = 0;

    while(i < SubNames.size())
    {
        std::string aSubName = static_cast<std::string>(SubNames.at(i));

        if (aSubName.size() > 4 && aSubName.substr(0,4) == "Edge") {
            TopoDS_Edge edge = TopoDS::Edge(TopShape.getSubShape(aSubName.c_str()));
            const TopTools_ListOfShape& los = mapEdgeFace.FindFromKey(edge);

            if(los.Extent() != 2)
            {
                SubNames.erase(SubNames.begin()+i);
                continue;
            }

            const TopoDS_Shape& face1 = los.First();
            const TopoDS_Shape& face2 = los.Last();
            GeomAbs_Shape cont = BRep_Tool::Continuity(TopoDS::Edge(edge),
                                                       TopoDS::Face(face1),
                                                       TopoDS::Face(face2));
            if (cont != GeomAbs_C0) {
                SubNames.erase(SubNames.begin()+i);
                continue;
            }

            i++;
        }
        else if(aSubName.size() > 4 && aSubName.substr(0,4) == "Face") {
            TopoDS_Face face = TopoDS::Face(TopShape.getSubShape(aSubName.c_str()));

            TopTools_IndexedMapOfShape mapOfFaces;
            TopExp::MapShapes(face, TopAbs_EDGE, mapOfFaces);

            for(int j = 1; j <= mapOfFaces.Extent(); ++j) {
                TopoDS_Edge edge = TopoDS::Edge(mapOfFaces.FindKey(j));

                int id = mapOfEdges.FindIndex(edge);

                std::stringstream buf;
                buf << "Edge";
                buf << id;

                if(std::find(SubNames.begin(),SubNames.end(),buf.str()) == SubNames.end())
                {
                    SubNames.push_back(buf.str());
                }

            }

            SubNames.erase(SubNames.begin()+i);
        }
        // empty name or any other sub-element
        else {
            SubNames.erase(SubNames.begin()+i);
        }
    }

    if (SubNames.size() == 0) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
        QObject::tr("No fillet possible on selected faces/edges."));
        return;
    }

    std::string SelString;
    SelString += "(App.";
    SelString += "ActiveDocument";//getObject()->getDocument()->getName();
    SelString += ".";
    SelString += selection[0].getFeatName();
    SelString += ",[";
    for(std::vector<std::string>::const_iterator it = SubNames.begin();it!=SubNames.end();++it){
        SelString += "\"";
        SelString += *it;
        SelString += "\"";
        if(it != --SubNames.end())
            SelString += ",";
    }
    SelString += "])";

    std::string FeatName = getUniqueObjectName("Fillet");

    openCommand("Make Fillet");
    doCommand(Doc,"App.activeDocument().addObject(\"PartDesign::Fillet\",\"%s\")",FeatName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Base = %s",FeatName.c_str(),SelString.c_str());
    doCommand(Gui,"Gui.Selection.clearSelection()");
    doCommand(Gui,"Gui.activeDocument().hide(\"%s\")",selection[0].getFeatName());
    doCommand(Gui,"Gui.activeDocument().setEdit('%s')",FeatName.c_str());
    App::DocumentObjectGroup* grp = base->getGroup();
    if (grp) {
        doCommand(Doc,"App.activeDocument().%s.addObject(App.activeDocument().%s)"
                     ,grp->getNameInDocument(),FeatName.c_str());
    }

    copyVisual(FeatName.c_str(), "ShapeColor", selection[0].getFeatName());
    copyVisual(FeatName.c_str(), "LineColor",  selection[0].getFeatName());
    copyVisual(FeatName.c_str(), "PointColor", selection[0].getFeatName());
}

bool CmdPartDesignFillet::isActive(void)
{
    return hasActiveDocument();
}

//===========================================================================
// PartDesign_Chamfer
//===========================================================================
DEF_STD_CMD_A(CmdPartDesignChamfer);

CmdPartDesignChamfer::CmdPartDesignChamfer()
  :Command("PartDesign_Chamfer")
{
    sAppModule    = "PartDesign";
    sGroup        = QT_TR_NOOP("PartDesign");
    sMenuText     = QT_TR_NOOP("Chamfer");
    sToolTipText  = QT_TR_NOOP("Chamfer the selected edges of a shape");
    sWhatsThis    = "PartDesign_Chamfer";
    sStatusTip    = sToolTipText;
    sPixmap       = "PartDesign_Chamfer";
}

void CmdPartDesignChamfer::activated(int iMsg)
{
    std::vector<Gui::SelectionObject> selection = getSelection().getSelectionEx();

    if (selection.size() != 1) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select an edge, face or body. Only one body is allowed."));
        return;
    }

    if (!selection[0].isObjectTypeOf(Part::Feature::getClassTypeId())){
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong object type"),
            QObject::tr("Chamfer works only on parts."));
        return;
    }

    Part::Feature *base = static_cast<Part::Feature*>(selection[0].getObject());

    const Part::TopoShape& TopShape = base->Shape.getShape();

    if (TopShape._Shape.IsNull()){
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Shape of selected part is empty."));
        return;
    }

    TopTools_IndexedMapOfShape mapOfEdges;
    TopTools_IndexedDataMapOfShapeListOfShape mapEdgeFace;
    TopExp::MapShapesAndAncestors(TopShape._Shape, TopAbs_EDGE, TopAbs_FACE, mapEdgeFace);
    TopExp::MapShapes(TopShape._Shape, TopAbs_EDGE, mapOfEdges);

    std::vector<std::string> SubNames = std::vector<std::string>(selection[0].getSubNames());

    unsigned int i = 0;

    while(i < SubNames.size())
    {
        std::string aSubName = static_cast<std::string>(SubNames.at(i));

        if (aSubName.size() > 4 && aSubName.substr(0,4) == "Edge") {
            TopoDS_Edge edge = TopoDS::Edge(TopShape.getSubShape(aSubName.c_str()));
            const TopTools_ListOfShape& los = mapEdgeFace.FindFromKey(edge);

            if(los.Extent() != 2)
            {
                SubNames.erase(SubNames.begin()+i);
                continue;
            }

            const TopoDS_Shape& face1 = los.First();
            const TopoDS_Shape& face2 = los.Last();
            GeomAbs_Shape cont = BRep_Tool::Continuity(TopoDS::Edge(edge),
                                                       TopoDS::Face(face1),
                                                       TopoDS::Face(face2));
            if (cont != GeomAbs_C0) {
                SubNames.erase(SubNames.begin()+i);
                continue;
            }

            i++;
        }
        else if(aSubName.size() > 4 && aSubName.substr(0,4) == "Face") {
            TopoDS_Face face = TopoDS::Face(TopShape.getSubShape(aSubName.c_str()));

            TopTools_IndexedMapOfShape mapOfFaces;
            TopExp::MapShapes(face, TopAbs_EDGE, mapOfFaces);

            for(int j = 1; j <= mapOfFaces.Extent(); ++j) {
                TopoDS_Edge edge = TopoDS::Edge(mapOfFaces.FindKey(j));

                int id = mapOfEdges.FindIndex(edge);

                std::stringstream buf;
                buf << "Edge";
                buf << id;

                if(std::find(SubNames.begin(),SubNames.end(),buf.str()) == SubNames.end())
                {
                    SubNames.push_back(buf.str());
                }

            }

            SubNames.erase(SubNames.begin()+i);
        }
        // empty name or any other sub-element
        else {
            SubNames.erase(SubNames.begin()+i);
        }
    }

    if (SubNames.size() == 0) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
        QObject::tr("No chamfer possible on selected faces/edges."));
        return;
    }

    std::string SelString;
    SelString += "(App.";
    SelString += "ActiveDocument";//getObject()->getDocument()->getName();
    SelString += ".";
    SelString += selection[0].getFeatName();
    SelString += ",[";
    for(std::vector<std::string>::const_iterator it = SubNames.begin();it!=SubNames.end();++it){
        SelString += "\"";
        SelString += *it;
        SelString += "\"";
        if(it != --SubNames.end())
            SelString += ",";
    }
    SelString += "])";

    std::string FeatName = getUniqueObjectName("Chamfer");

    openCommand("Make Chamfer");
    doCommand(Doc,"App.activeDocument().addObject(\"PartDesign::Chamfer\",\"%s\")",FeatName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Base = %s",FeatName.c_str(),SelString.c_str());
    doCommand(Gui,"Gui.Selection.clearSelection()");
    doCommand(Gui,"Gui.activeDocument().hide(\"%s\")",selection[0].getFeatName());
    doCommand(Gui,"Gui.activeDocument().setEdit('%s')",FeatName.c_str());
    App::DocumentObjectGroup* grp = base->getGroup();
    if (grp) {
        doCommand(Doc,"App.activeDocument().%s.addObject(App.activeDocument().%s)"
                     ,grp->getNameInDocument(),FeatName.c_str());
    }

    copyVisual(FeatName.c_str(), "ShapeColor", selection[0].getFeatName());
    copyVisual(FeatName.c_str(), "LineColor",  selection[0].getFeatName());
    copyVisual(FeatName.c_str(), "PointColor", selection[0].getFeatName());
}

bool CmdPartDesignChamfer::isActive(void)
{
    return hasActiveDocument();
}

//===========================================================================
// PartDesign_Draft
//===========================================================================
DEF_STD_CMD_A(CmdPartDesignDraft);

CmdPartDesignDraft::CmdPartDesignDraft()
  :Command("PartDesign_Draft")
{
    sAppModule    = "PartDesign";
    sGroup        = QT_TR_NOOP("PartDesign");
    sMenuText     = QT_TR_NOOP("Draft");
    sToolTipText  = QT_TR_NOOP("Make a draft on a face");
    sWhatsThis    = "PartDesign_Draft";
    sStatusTip    = sToolTipText;
    sPixmap       = "PartDesign_Draft";
}

void CmdPartDesignDraft::activated(int iMsg)
{
    std::vector<Gui::SelectionObject> selection = getSelection().getSelectionEx();

    if (selection.size() < 1) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select one or more faces."));
        return;
    }

    if (!selection[0].isObjectTypeOf(Part::Feature::getClassTypeId())){
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong object type"),
            QObject::tr("Draft works only on parts."));
        return;
    }

    Part::Feature *base = static_cast<Part::Feature*>(selection[0].getObject());

    const Part::TopoShape& TopShape = base->Shape.getShape();
    if (TopShape._Shape.IsNull()){
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Shape of selected Part is empty."));
        return;
    }

    std::vector<std::string> SubNames = std::vector<std::string>(selection[0].getSubNames());
    unsigned int i = 0;

    while(i < SubNames.size())
    {
        std::string aSubName = static_cast<std::string>(SubNames.at(i));

        if(aSubName.size() > 4 && aSubName.substr(0,4) == "Face") {
            // Check for valid face types
            TopoDS_Face face = TopoDS::Face(TopShape.getSubShape(aSubName.c_str()));
            BRepAdaptor_Surface sf(face);
            if ((sf.GetType() != GeomAbs_Plane) && (sf.GetType() != GeomAbs_Cylinder) && (sf.GetType() != GeomAbs_Cone))
                SubNames.erase(SubNames.begin()+i);
        } else {
            // empty name or any other sub-element
            SubNames.erase(SubNames.begin()+i);
        }

        i++;
    }

    if (SubNames.size() == 0) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
        QObject::tr("No draft possible on selected faces."));
        return;
    }

    std::string SelString;
    SelString += "(App.";
    SelString += "ActiveDocument";
    SelString += ".";
    SelString += selection[0].getFeatName();
    SelString += ",[";
    for(std::vector<std::string>::const_iterator it = SubNames.begin();it!=SubNames.end();++it){
        SelString += "\"";
        SelString += *it;
        SelString += "\"";
        if(it != --SubNames.end())
            SelString += ",";
    }
    SelString += "])";

    std::string FeatName = getUniqueObjectName("Draft");

    // We don't create any defaults for neutral plane and pull direction, but Draft::execute()
    // will choose them.
    // Note: When the body feature is there, the best thing would be to get pull direction and
    // neutral plane from the preceding feature in the tree. Or even store them as default in
    // the Body feature itself
    openCommand("Make Draft");
    doCommand(Doc,"App.activeDocument().addObject(\"PartDesign::Draft\",\"%s\")",FeatName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Base = %s",FeatName.c_str(),SelString.c_str());
    doCommand(Doc,"App.activeDocument().%s.Angle = %f",FeatName.c_str(), 1.5);
    updateActive();
    if (isActiveObjectValid()) {
        doCommand(Gui,"Gui.activeDocument().hide(\"%s\")",selection[0].getFeatName());
    }
    doCommand(Gui,"Gui.activeDocument().setEdit('%s')",FeatName.c_str());
    App::DocumentObjectGroup* grp = base->getGroup();
    if (grp) {
        doCommand(Doc,"App.activeDocument().%s.addObject(App.activeDocument().%s)"
                     ,grp->getNameInDocument(),FeatName.c_str());
    }

    copyVisual(FeatName.c_str(), "ShapeColor", selection[0].getFeatName());
    copyVisual(FeatName.c_str(), "LineColor",  selection[0].getFeatName());
    copyVisual(FeatName.c_str(), "PointColor", selection[0].getFeatName());
}

bool CmdPartDesignDraft::isActive(void)
{
    return hasActiveDocument();
}

//===========================================================================
// PartDesign_Mirrored
//===========================================================================
DEF_STD_CMD_A(CmdPartDesignMirrored);

CmdPartDesignMirrored::CmdPartDesignMirrored()
  : Command("PartDesign_Mirrored")
{
    sAppModule    = "PartDesign";
    sGroup        = QT_TR_NOOP("PartDesign");
    sMenuText     = QT_TR_NOOP("Mirrored");
    sToolTipText  = QT_TR_NOOP("Create a mirrored feature");
    sWhatsThis    = "PartDesign_Mirrored";
    sStatusTip    = sToolTipText;
    sPixmap       = "PartDesign_Mirrored";
}

void CmdPartDesignMirrored::activated(int iMsg)
{
    // Get a valid original from the user
    // First check selections
    std::vector<App::DocumentObject*> features = getSelection().getObjectsOfType(PartDesign::Additive::getClassTypeId());
    std::vector<App::DocumentObject*> subtractive = getSelection().getObjectsOfType(PartDesign::Subtractive::getClassTypeId());
    features.insert(features.end(), subtractive.begin(), subtractive.end());
    // Next create a list of all eligible objects
    if (features.size() == 0) {
        features = getDocument()->getObjectsOfType(PartDesign::Additive::getClassTypeId());
        subtractive = getDocument()->getObjectsOfType(PartDesign::Subtractive::getClassTypeId());
        features.insert(features.end(), subtractive.begin(), subtractive.end());
        // If there is more than one selected or eligible object, show dialog and let user pick one
        if (features.size() > 1) {
            PartDesignGui::FeaturePickDialog Dlg(features);
            if ((Dlg.exec() != QDialog::Accepted) || (features = Dlg.getFeatures()).empty())
                return; // Cancelled or nothing selected
        } else {
            QMessageBox::warning(Gui::getMainWindow(), QObject::tr("No valid features in this document"),
                QObject::tr("Please create a subtractive or additive feature first."));
            return;
        }
    }

    std::string FeatName = getUniqueObjectName("Mirrored");

    std::stringstream str;
    std::vector<std::string> tempSelNames;
    str << "App.activeDocument()." << FeatName << ".Originals = [";
    for (std::vector<App::DocumentObject*>::iterator it = features.begin(); it != features.end(); ++it){
        str << "App.activeDocument()." << (*it)->getNameInDocument() << ",";
        tempSelNames.push_back((*it)->getNameInDocument());
    }
    str << "]";

    openCommand("Mirrored");
    doCommand(Doc,"App.activeDocument().addObject(\"PartDesign::Mirrored\",\"%s\")",FeatName.c_str());
    // FIXME: There seems to be kind of a race condition here, leading to sporadic errors like
    // Exception (Thu Sep  6 11:52:01 2012): 'App.Document' object has no attribute 'Mirrored'
    updateActive(); // Helps to ensure that the object already exists when the next command comes up
    doCommand(Doc,str.str().c_str());
    Part::Part2DObject *sketch = (static_cast<PartDesign::SketchBased*>(features.front()))->getVerifiedSketch();
    if (sketch)
        doCommand(Doc,"App.activeDocument().%s.MirrorPlane = (App.activeDocument().%s, [\"V_Axis\"])",
                  FeatName.c_str(), sketch->getNameInDocument());
    for (std::vector<std::string>::iterator it = tempSelNames.begin(); it != tempSelNames.end(); ++it)
        doCommand(Gui,"Gui.activeDocument().%s.Visibility=False",it->c_str());

    doCommand(Gui,"Gui.activeDocument().setEdit('%s')",FeatName.c_str());

    copyVisual(FeatName.c_str(), "ShapeColor", tempSelNames.front().c_str());
    copyVisual(FeatName.c_str(), "DisplayMode", tempSelNames.front().c_str());
}

bool CmdPartDesignMirrored::isActive(void)
{
    return hasActiveDocument();
}

//===========================================================================
// PartDesign_LinearPattern
//===========================================================================
DEF_STD_CMD_A(CmdPartDesignLinearPattern);

CmdPartDesignLinearPattern::CmdPartDesignLinearPattern()
  : Command("PartDesign_LinearPattern")
{
    sAppModule    = "PartDesign";
    sGroup        = QT_TR_NOOP("PartDesign");
    sMenuText     = QT_TR_NOOP("LinearPattern");
    sToolTipText  = QT_TR_NOOP("Create a linear pattern feature");
    sWhatsThis    = "PartDesign_LinearPattern";
    sStatusTip    = sToolTipText;
    sPixmap       = "PartDesign_LinearPattern";
}

void CmdPartDesignLinearPattern::activated(int iMsg)
{
    // Get a valid original from the user
    // First check selections
    std::vector<App::DocumentObject*> features = getSelection().getObjectsOfType(PartDesign::Additive::getClassTypeId());
    std::vector<App::DocumentObject*> subtractive = getSelection().getObjectsOfType(PartDesign::Subtractive::getClassTypeId());
    features.insert(features.end(), subtractive.begin(), subtractive.end());
    // Next create a list of all eligible objects
    if (features.size() == 0) {
        features = getDocument()->getObjectsOfType(PartDesign::Additive::getClassTypeId());
        subtractive = getDocument()->getObjectsOfType(PartDesign::Subtractive::getClassTypeId());
        features.insert(features.end(), subtractive.begin(), subtractive.end());
        // If there is more than one selected or eligible object, show dialog and let user pick one
        if (features.size() > 1) {
            PartDesignGui::FeaturePickDialog Dlg(features);
            if ((Dlg.exec() != QDialog::Accepted) || (features = Dlg.getFeatures()).empty())
                return; // Cancelled or nothing selected
        } else {
            QMessageBox::warning(Gui::getMainWindow(), QObject::tr("No valid features in this document"),
                QObject::tr("Please create a subtractive or additive feature first, please."));
            return;
        }
    }

    std::string FeatName = getUniqueObjectName("LinearPattern");

    std::stringstream str;
    std::vector<std::string> tempSelNames;
    str << "App.activeDocument()." << FeatName << ".Originals = [";
    for (std::vector<App::DocumentObject*>::iterator it = features.begin(); it != features.end(); ++it){
        str << "App.activeDocument()." << (*it)->getNameInDocument() << ",";
        tempSelNames.push_back((*it)->getNameInDocument());
    }
    str << "]";

    openCommand("LinearPattern");
    doCommand(Doc,"App.activeDocument().addObject(\"PartDesign::LinearPattern\",\"%s\")",FeatName.c_str());
    updateActive();
    doCommand(Doc,str.str().c_str());
    Part::Part2DObject *sketch = (static_cast<PartDesign::SketchBased*>(features.front()))->getVerifiedSketch();
    if (sketch)
        doCommand(Doc,"App.activeDocument().%s.Direction = (App.activeDocument().%s, [\"H_Axis\"])",
                  FeatName.c_str(), sketch->getNameInDocument());
    doCommand(Doc,"App.activeDocument().%s.Length = 100", FeatName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Occurrences = 2", FeatName.c_str());
    for (std::vector<std::string>::iterator it = tempSelNames.begin(); it != tempSelNames.end(); ++it)
        doCommand(Gui,"Gui.activeDocument().%s.Visibility=False",it->c_str());

    doCommand(Gui,"Gui.activeDocument().setEdit('%s')",FeatName.c_str());
    App::DocumentObjectGroup* grp = sketch->getGroup();
    if (grp) {
        doCommand(Doc,"App.activeDocument().%s.addObject(App.activeDocument().%s)"
                     ,grp->getNameInDocument(),FeatName.c_str());
        doCommand(Doc,"App.activeDocument().%s.removeObject(App.activeDocument().%s)"
                     ,grp->getNameInDocument(),sketch->getNameInDocument());
    }

    copyVisual(FeatName.c_str(), "ShapeColor", tempSelNames.front().c_str());
    copyVisual(FeatName.c_str(), "DisplayMode", tempSelNames.front().c_str());
}

bool CmdPartDesignLinearPattern::isActive(void)
{
    return hasActiveDocument();
}

//===========================================================================
// PartDesign_PolarPattern
//===========================================================================
DEF_STD_CMD_A(CmdPartDesignPolarPattern);

CmdPartDesignPolarPattern::CmdPartDesignPolarPattern()
  : Command("PartDesign_PolarPattern")
{
    sAppModule    = "PartDesign";
    sGroup        = QT_TR_NOOP("PartDesign");
    sMenuText     = QT_TR_NOOP("PolarPattern");
    sToolTipText  = QT_TR_NOOP("Create a polar pattern feature");
    sWhatsThis    = "PartDesign_PolarPattern";
    sStatusTip    = sToolTipText;
    sPixmap       = "PartDesign_PolarPattern";
}

void CmdPartDesignPolarPattern::activated(int iMsg)
{
    // Get a valid original from the user
    // First check selections
    std::vector<App::DocumentObject*> features = getSelection().getObjectsOfType(PartDesign::Additive::getClassTypeId());
    std::vector<App::DocumentObject*> subtractive = getSelection().getObjectsOfType(PartDesign::Subtractive::getClassTypeId());
    features.insert(features.end(), subtractive.begin(), subtractive.end());
    // Next create a list of all eligible objects
    if (features.size() == 0) {
        features = getDocument()->getObjectsOfType(PartDesign::Additive::getClassTypeId());
        subtractive = getDocument()->getObjectsOfType(PartDesign::Subtractive::getClassTypeId());
        features.insert(features.end(), subtractive.begin(), subtractive.end());
        // If there is more than one selected or eligible object, show dialog and let user pick one
        if (features.size() > 1) {
            PartDesignGui::FeaturePickDialog Dlg(features);
            if ((Dlg.exec() != QDialog::Accepted) || (features = Dlg.getFeatures()).empty())
                return; // Cancelled or nothing selected
        } else {
            QMessageBox::warning(Gui::getMainWindow(), QObject::tr("No valid features in this document"),
                QObject::tr("Please create a subtractive or additive feature first, please."));
            return;
        }
    }

    std::string FeatName = getUniqueObjectName("PolarPattern");

    std::stringstream str;
    std::vector<std::string> tempSelNames;
    str << "App.activeDocument()." << FeatName << ".Originals = [";
    for (std::vector<App::DocumentObject*>::iterator it = features.begin(); it != features.end(); ++it){
        str << "App.activeDocument()." << (*it)->getNameInDocument() << ",";
        tempSelNames.push_back((*it)->getNameInDocument());
    }
    str << "]";

    openCommand("PolarPattern");
    doCommand(Doc,"App.activeDocument().addObject(\"PartDesign::PolarPattern\",\"%s\")",FeatName.c_str());
    updateActive();
    doCommand(Doc,str.str().c_str());
    Part::Part2DObject *sketch = (static_cast<PartDesign::SketchBased*>(features.front()))->getVerifiedSketch();
    if (sketch)
        doCommand(Doc,"App.activeDocument().%s.Axis = (App.activeDocument().%s, [\"N_Axis\"])",
                  FeatName.c_str(), sketch->getNameInDocument());
    doCommand(Doc,"App.activeDocument().%s.Angle = 360", FeatName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Occurrences = 2", FeatName.c_str());
    for (std::vector<std::string>::iterator it = tempSelNames.begin(); it != tempSelNames.end(); ++it)
        doCommand(Gui,"Gui.activeDocument().%s.Visibility=False",it->c_str());

    doCommand(Gui,"Gui.activeDocument().setEdit('%s')",FeatName.c_str());
    App::DocumentObjectGroup* grp = sketch->getGroup();
    if (grp) {
        doCommand(Doc,"App.activeDocument().%s.addObject(App.activeDocument().%s)"
                     ,grp->getNameInDocument(),FeatName.c_str());
        doCommand(Doc,"App.activeDocument().%s.removeObject(App.activeDocument().%s)"
                     ,grp->getNameInDocument(),sketch->getNameInDocument());
    }

    copyVisual(FeatName.c_str(), "ShapeColor", tempSelNames.front().c_str());
    copyVisual(FeatName.c_str(), "DisplayMode", tempSelNames.front().c_str());
}

bool CmdPartDesignPolarPattern::isActive(void)
{
    return hasActiveDocument();
}

//===========================================================================
// PartDesign_Scaled
//===========================================================================
DEF_STD_CMD_A(CmdPartDesignScaled);

CmdPartDesignScaled::CmdPartDesignScaled()
  : Command("PartDesign_Scaled")
{
    sAppModule    = "PartDesign";
    sGroup        = QT_TR_NOOP("PartDesign");
    sMenuText     = QT_TR_NOOP("Scaled");
    sToolTipText  = QT_TR_NOOP("Create a scaled feature");
    sWhatsThis    = "PartDesign_Scaled";
    sStatusTip    = sToolTipText;
    sPixmap       = "PartDesign_Scaled";
}

void CmdPartDesignScaled::activated(int iMsg)
{
    // Get a valid original from the user
    // First check selections
    std::vector<App::DocumentObject*> features = getSelection().getObjectsOfType(PartDesign::Additive::getClassTypeId());
    std::vector<App::DocumentObject*> subtractive = getSelection().getObjectsOfType(PartDesign::Subtractive::getClassTypeId());
    features.insert(features.end(), subtractive.begin(), subtractive.end());
    // Next create a list of all eligible objects
    if (features.size() == 0) {
        features = getDocument()->getObjectsOfType(PartDesign::Additive::getClassTypeId());
        subtractive = getDocument()->getObjectsOfType(PartDesign::Subtractive::getClassTypeId());
        features.insert(features.end(), subtractive.begin(), subtractive.end());
        // If there is more than one selected or eligible object, show dialog and let user pick one
        if (features.size() > 1) {
            PartDesignGui::FeaturePickDialog Dlg(features);
            if ((Dlg.exec() != QDialog::Accepted) || (features = Dlg.getFeatures()).empty())
                return; // Cancelled or nothing selected
        } else {
            QMessageBox::warning(Gui::getMainWindow(), QObject::tr("No valid features in this document"),
                QObject::tr("Please create a subtractive or additive feature first, please."));
            return;
        }
    }

    std::string FeatName = getUniqueObjectName("Scaled");

    std::stringstream str;
    std::vector<std::string> tempSelNames;
    str << "App.activeDocument()." << FeatName << ".Originals = [";
    for (std::vector<App::DocumentObject*>::iterator it = features.begin(); it != features.end(); ++it){
        str << "App.activeDocument()." << (*it)->getNameInDocument() << ",";
        tempSelNames.push_back((*it)->getNameInDocument());
    }
    str << "]";

    openCommand("Scaled");
    doCommand(Doc,"App.activeDocument().addObject(\"PartDesign::Scaled\",\"%s\")",FeatName.c_str());
    updateActive();
    doCommand(Doc,str.str().c_str());
    doCommand(Doc,"App.activeDocument().%s.Factor = 2", FeatName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Occurrences = 2", FeatName.c_str());
    for (std::vector<std::string>::iterator it = tempSelNames.begin(); it != tempSelNames.end(); ++it)
        doCommand(Gui,"Gui.activeDocument().%s.Visibility=False",it->c_str());

    doCommand(Gui,"Gui.activeDocument().setEdit('%s')",FeatName.c_str());

    copyVisual(FeatName.c_str(), "ShapeColor", tempSelNames.front().c_str());
    copyVisual(FeatName.c_str(), "DisplayMode", tempSelNames.front().c_str());
}

bool CmdPartDesignScaled::isActive(void)
{
    return hasActiveDocument();
}

//===========================================================================
// PartDesign_MultiTransform
//===========================================================================
DEF_STD_CMD_A(CmdPartDesignMultiTransform);

CmdPartDesignMultiTransform::CmdPartDesignMultiTransform()
  : Command("PartDesign_MultiTransform")
{
    sAppModule    = "PartDesign";
    sGroup        = QT_TR_NOOP("PartDesign");
    sMenuText     = QT_TR_NOOP("MultiTransform");
    sToolTipText  = QT_TR_NOOP("Create a multitransform feature");
    sWhatsThis    = "PartDesign_MultiTransform";
    sStatusTip    = sToolTipText;
    sPixmap       = "PartDesign_MultiTransform";
}

void CmdPartDesignMultiTransform::activated(int iMsg)
{
    // Get a valid original from the user
    // First check selections
    std::vector<App::DocumentObject*> features = getSelection().getObjectsOfType(PartDesign::Additive::getClassTypeId());
    std::vector<App::DocumentObject*> subtractive = getSelection().getObjectsOfType(PartDesign::Subtractive::getClassTypeId());
    features.insert(features.end(), subtractive.begin(), subtractive.end());
    // Next create a list of all eligible objects
    if (features.size() == 0) {
        features = getDocument()->getObjectsOfType(PartDesign::Additive::getClassTypeId());
        subtractive = getDocument()->getObjectsOfType(PartDesign::Subtractive::getClassTypeId());
        features.insert(features.end(), subtractive.begin(), subtractive.end());
        // If there is more than one selected or eligible object, show dialog and let user pick one
        if (features.size() > 1) {
            PartDesignGui::FeaturePickDialog Dlg(features);
            if ((Dlg.exec() != QDialog::Accepted) || (features = Dlg.getFeatures()).empty())
                return; // Cancelled or nothing selected
        } else {
            QMessageBox::warning(Gui::getMainWindow(), QObject::tr("No valid features in this document"),
                QObject::tr("Please create a subtractive or additive feature first, please."));
            return;
        }
    }

    std::string FeatName = getUniqueObjectName("MultiTransform");

    std::stringstream str;
    std::vector<std::string> tempSelNames;
    str << "App.activeDocument()." << FeatName << ".Originals = [";
    for (std::vector<App::DocumentObject*>::iterator it = features.begin(); it != features.end(); ++it){
        str << "App.activeDocument()." << (*it)->getNameInDocument() << ",";
        tempSelNames.push_back((*it)->getNameInDocument());
    }
    str << "]";

    openCommand("MultiTransform");
    doCommand(Doc,"App.activeDocument().addObject(\"PartDesign::MultiTransform\",\"%s\")",FeatName.c_str());
    updateActive();
    doCommand(Doc,str.str().c_str());

    doCommand(Gui,"Gui.activeDocument().setEdit('%s')",FeatName.c_str());

    copyVisual(FeatName.c_str(), "ShapeColor", tempSelNames.front().c_str());
    copyVisual(FeatName.c_str(), "DisplayMode", tempSelNames.front().c_str());
}

bool CmdPartDesignMultiTransform::isActive(void)
{
    return hasActiveDocument();
}


//===========================================================================
// Initialization
//===========================================================================

void CreatePartDesignCommands(void)
{
    Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();

    rcCmdMgr.addCommand(new CmdPartDesignPad());
    rcCmdMgr.addCommand(new CmdPartDesignPocket());
    rcCmdMgr.addCommand(new CmdPartDesignRevolution());
    rcCmdMgr.addCommand(new CmdPartDesignGroove());
    rcCmdMgr.addCommand(new CmdPartDesignFillet());
    rcCmdMgr.addCommand(new CmdPartDesignDraft());
    rcCmdMgr.addCommand(new CmdPartDesignNewSketch());
    rcCmdMgr.addCommand(new CmdPartDesignChamfer());
    rcCmdMgr.addCommand(new CmdPartDesignMirrored());
    rcCmdMgr.addCommand(new CmdPartDesignLinearPattern());
    rcCmdMgr.addCommand(new CmdPartDesignPolarPattern());
    //rcCmdMgr.addCommand(new CmdPartDesignScaled());
    rcCmdMgr.addCommand(new CmdPartDesignMultiTransform());
 }
