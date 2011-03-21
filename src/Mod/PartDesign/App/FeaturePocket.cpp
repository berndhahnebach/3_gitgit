/***************************************************************************
 *   Copyright (c) 2010 Juergen Riegel <FreeCAD@juergen-riegel.net>        *
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
# include <Bnd_Box.hxx>
# include <gp_Pln.hxx>
# include <BRep_Builder.hxx>
# include <BRepBndLib.hxx>
# include <BRepPrimAPI_MakePrism.hxx>
# include <BRepBuilderAPI_Copy.hxx>
# include <BRepBuilderAPI_MakeFace.hxx>
# include <Geom_Plane.hxx>
# include <Handle_Geom_Surface.hxx>
# include <TopoDS.hxx>
# include <TopoDS_Face.hxx>
# include <TopoDS_Wire.hxx>
# include <TopExp_Explorer.hxx>
# include <BRepAlgoAPI_Cut.hxx>
#endif

#include <Base/Placement.h>
#include <Mod/Part/App/Part2DObject.h>

#include "FeaturePocket.h"


using namespace PartDesign;

namespace PartDesign {


// sort bounding boxes according to diagonal length
struct Wire_Compare {
    bool operator() (const TopoDS_Wire& w1, const TopoDS_Wire& w2)
    {
        Bnd_Box box1, box2;
        BRepBndLib::Add(w1, box1);
        box1.SetGap(0.0);

        BRepBndLib::Add(w2, box2);
        box2.SetGap(0.0);
        
        return box1.SquareExtent() < box2.SquareExtent();
    }
};
}

const char* Pocket::TypeEnums[]= {"Length","UpToLast","UpToFirst",NULL};

PROPERTY_SOURCE(PartDesign::Pocket, PartDesign::SketchBased)

Pocket::Pocket()
{
    ADD_PROPERTY(Type,((long)0));
    Type.setEnums(TypeEnums);
    ADD_PROPERTY(Length,(100.0));
}

short Pocket::mustExecute() const
{
    if (Sketch.isTouched() ||
        Length.isTouched())
        return 1;
    return 0;
}

App::DocumentObjectExecReturn *Pocket::execute(void)
{
    App::DocumentObject* link = Sketch.getValue();
    if (!link)
        return new App::DocumentObjectExecReturn("No sketch linked");
    if (!link->getTypeId().isDerivedFrom(Part::Part2DObject::getClassTypeId()))
        return new App::DocumentObjectExecReturn("Linked object is not a Sketch or Part2DObject");
    TopoDS_Shape shape = static_cast<Part::Part2DObject*>(link)->Shape.getShape()._Shape;
    if (shape.IsNull())
        return new App::DocumentObjectExecReturn("Linked shape object is empty");

    // this is a workaround for an obscure OCC bug which leads to empty tessellations
    // for some faces. Making an explicit copy of the linked shape seems to fix it.
    // The error only happens when re-computing the shape.
    if (!this->Shape.getValue().IsNull()) {
        BRepBuilderAPI_Copy copy(shape);
        shape = copy.Shape();
        if (shape.IsNull())
            return new App::DocumentObjectExecReturn("Linked shape object is empty");
    }

    TopExp_Explorer ex;
    std::vector<TopoDS_Wire> wires;
    for (ex.Init(shape, TopAbs_WIRE, TopAbs_FACE); ex.More(); ex.Next()) {
        wires.push_back(TopoDS::Wire(ex.Current()));
    }
    if (/*shape.ShapeType() != TopAbs_WIRE*/wires.empty()) // there can be several wires
        return new App::DocumentObjectExecReturn("Linked shape object is not a wire");

    // get the Sketch plane
    Base::Placement SketchPos = static_cast<Part::Part2DObject*>(link)->Placement.getValue();
    Base::Rotation SketchOrientation = SketchPos.getRotation();
    Base::Vector3d SketchOrientationVector(0,0,1);
    SketchOrientation.multVec(SketchOrientationVector,SketchOrientationVector);

    // get the support of the Sketch if any
    App::DocumentObject* SupportLink = static_cast<Part::Part2DObject*>(link)->Support.getValue();
    Part::Feature *SupportObject = 0;
    if(SupportLink && SupportLink->getTypeId().isDerivedFrom(Part::Feature::getClassTypeId()))
        SupportObject = static_cast<Part::Feature*>(SupportLink);

    if (!SupportObject)
        return new App::DocumentObjectExecReturn("No support in Sketch!");

    Handle(Geom_Surface) aSurf = new Geom_Plane (gp_Pln(gp_Pnt(0,0,0),gp_Dir(0,0,1)));
    //anti-clockwise circles if too look from surface normal

    //FIXME: Need a safe method to sort wire that the outermost one comes last
    // Currently it's done with the diagonal lengths of the bounding boxes
    std::sort(wires.begin(), wires.end(), Wire_Compare());
    BRepBuilderAPI_MakeFace mkFace(wires.back());
    for (std::vector<TopoDS_Wire>::reverse_iterator it = wires.rbegin()+1; it != wires.rend(); ++it) {
        //it->Reverse(); // Shouldn't inner wires be reversed?
        mkFace.Add(*it);
    }
    TopoDS_Face aFace = mkFace.Face();
    if (aFace.IsNull())
        return new App::DocumentObjectExecReturn("Creating a face from sketch failed");

    // lengthen the vector
    SketchOrientationVector *= Length.getValue();

    // turn around for pockes
    SketchOrientationVector *= -1;

    // extrude the face to a solid
    gp_Vec vec(SketchOrientationVector.x,SketchOrientationVector.y,SketchOrientationVector.z);
    BRepPrimAPI_MakePrism PrismMaker(aFace,vec,0,1);
    if (PrismMaker.IsDone()) {
        // if the sketch has a support fuse them to get one result object (PAD!)
        if (SupportObject) {
            // Let's call algorithm computing a fuse operation:
            BRepAlgoAPI_Cut mkCut(SupportObject->Shape.getShape()._Shape, PrismMaker.Shape());
            // Let's check if the fusion has been successful
            if (!mkCut.IsDone()) 
                throw Base::Exception("cut with support failed");
            this->Shape.setValue(mkCut.Shape());
        }
        else{
            return new App::DocumentObjectExecReturn("Cannot create a tool out of Sketch!");
        }
    }
    else
        return new App::DocumentObjectExecReturn("Could not extrude the sketch!");

    return App::DocumentObject::StdReturn;
}

