/***************************************************************************
 *   Copyright (c) J�rgen Riegel          (juergen.riegel@web.de) 2008     *
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
# include <TopoDS_Shape.hxx>
# include <TopoDS_Face.hxx>
# include <TopoDS.hxx>
# include <BRepAdaptor_Surface.hxx>
#endif

#include <Base/Writer.h>
#include <Base/Reader.h>

#include <Mod/Part/App/Geometry.h>

#include <vector>

#include "SketchObject.h"
#include "SketchObjectPy.h"
#include "Sketch.h"

using namespace Sketcher;
using namespace Base;


PROPERTY_SOURCE(Sketcher::SketchObject, Part::Part2DObject)


SketchObject::SketchObject()
{
    ADD_PROPERTY_TYPE(Geometry,           (0)  ,"Sketch",(App::PropertyType)(App::Prop_None),"Sketch geometry");
    ADD_PROPERTY_TYPE(Constraints,        (0)  ,"Sketch",(App::PropertyType)(App::Prop_None),"Sketch constraints");
    ADD_PROPERTY_TYPE(ExternalConstraints,(0,0),"Sketch",(App::PropertyType)(App::Prop_None),"Sketch external constraints");
}

App::DocumentObjectExecReturn *SketchObject::execute(void)
{
    // recalculate support:
    Part::Feature *part = static_cast<Part::Feature*>(Support.getValue());
    if (part && part->getTypeId().isDerivedFrom(Part::Feature::getClassTypeId()))
    {
        Base::Placement ObjectPos = part->Placement.getValue();
        const std::vector<std::string> &sub = Support.getSubValues();
        assert(sub.size()==1);
        // get the selected sub shape (a Face)
        const Part::TopoShape &shape = part->Shape.getShape();
        if (shape._Shape.IsNull())
            return new App::DocumentObjectExecReturn("Support shape is empty!");
        TopoDS_Shape sh = shape.getSubShape(sub[0].c_str());
        const TopoDS_Face &face = TopoDS::Face(sh);
        assert(!face.IsNull());

        BRepAdaptor_Surface adapt(face);
        if (adapt.GetType() != GeomAbs_Plane)
            return new App::DocumentObjectExecReturn("Sketch has no planar support!");

        // set sketch position
        Base::Placement placement = Part2DObject::positionBySupport(face,ObjectPos);
        Placement.setValue(placement);
    }

    // and now solve the sketch
    Sketch sketch;

    sketch.setUpSketch(Geometry.getValues(), Constraints.getValues());

    // solve the sketch with no fixed points
    if (sketch.solve() != 0)
        return new App::DocumentObjectExecReturn("Solving the sketch failed!",this);

    std::vector<Part::Geometry *> geomlist = sketch.getGeometry();
    Geometry.setValues(geomlist);
    for (std::vector<Part::Geometry *>::iterator it = geomlist.begin(); it != geomlist.end(); ++it)
        if (*it) delete *it;

    Shape.setValue(sketch.toShape());

    return App::DocumentObject::StdReturn;
}

int SketchObject::setDatum(double Datum, int ConstrNbr)
{
    // set the changed value for the constraint
    const std::vector<Constraint *> &vals = this->Constraints.getValues();
    if (ConstrNbr < 0 || ConstrNbr >= (int)vals.size())
        return -1;
    ConstraintType type = vals[ConstrNbr]->Type;
    if (type != Distance &&
        type != DistanceX &&
        type != DistanceY &&
        type != Radius &&
        type != Angle)
        return -1;

    // copy the list
    std::vector<Constraint *> newVals(vals);
    // clone the changed Constraint
    Constraint *constNew = vals[ConstrNbr]->clone();
    constNew->Value = Datum;
    newVals[ConstrNbr] = constNew;
    this->Constraints.setValues(newVals);
    delete constNew;

    // set up an extra sketch
    Sketch sketch;
    sketch.setUpSketch(Geometry.getValues(), Constraints.getValues());

    int ret = sketch.solve();
    if (ret)
        return ret;

    // set the newly solved geometry
    std::vector<Part::Geometry *> geomlist = sketch.getGeometry();
    Geometry.setValues(geomlist);
    for (std::vector<Part::Geometry *>::iterator it = geomlist.begin(); it != geomlist.end(); ++it)
        if (*it) delete *it;

    return 0;
}

int SketchObject::movePoint(int geoIndex, PointPos PosId, const Base::Vector3d& toPoint)
{
    // set up an extra sketch
    Sketch sketch;
    sketch.setUpSketch(Geometry.getValues(), Constraints.getValues());

    int ret = sketch.movePoint(geoIndex, PosId, toPoint);

    if (ret == 0) {
        std::vector<Part::Geometry *> geomlist = sketch.getGeometry();
        Geometry.setValues(geomlist);
        for (std::vector<Part::Geometry *>::iterator it = geomlist.begin(); it != geomlist.end(); ++it) {
            if (*it) delete *it;
        }
    }

    return ret;
}

Base::Vector3d SketchObject::getPoint(int geoIndex, PointPos PosId)
{
    const std::vector< Part::Geometry * > &geomlist = this->Geometry.getValues();
    assert(geoIndex < (int)geomlist.size());
    Part::Geometry *geo = geomlist[geoIndex];
    if (geo->getTypeId() == Part::GeomLineSegment::getClassTypeId()) {
        const Part::GeomLineSegment *lineSeg = dynamic_cast<const Part::GeomLineSegment*>(geo);
        if (PosId == start)
            return lineSeg->getStartPoint();
        else if (PosId == end)
            return lineSeg->getEndPoint();
    } else if (geo->getTypeId() == Part::GeomCircle::getClassTypeId()) {
        const Part::GeomCircle *circle = dynamic_cast<const Part::GeomCircle*>(geo);
        if (PosId == mid)
            return circle->getCenter();
    } else if (geo->getTypeId() == Part::GeomArcOfCircle::getClassTypeId()) {
        const Part::GeomArcOfCircle *aoc = dynamic_cast<const Part::GeomArcOfCircle*>(geo);
        if (PosId == start)
            return aoc->getStartPoint();
        else if (PosId == end)
            return aoc->getEndPoint();
        else if (PosId == mid)
            return aoc->getCenter();
    }

    return Base::Vector3d();
}

int SketchObject::addGeometry(const std::vector<Part::Geometry *> &geoList)
{
    return -1;
}

int SketchObject::addGeometry(const Part::Geometry *geo)
{
    const std::vector< Part::Geometry * > &vals = Geometry.getValues();

    std::vector< Part::Geometry * > newVals(vals);
    Part::Geometry *geoNew = geo->clone();
    newVals.push_back(geoNew);
    Geometry.setValues(newVals);
    delete geoNew;
    rebuildVertexIndex();
    return Geometry.getSize()-1;
}

int SketchObject::delGeometry(int GeoNbr)
{
    const std::vector< Part::Geometry * > &vals = this->Geometry.getValues();
    if (GeoNbr < 0 || GeoNbr >= (int)vals.size())
        return -1;

    std::vector< Part::Geometry * > newVals(vals);
    newVals.erase(newVals.begin()+GeoNbr);

    const std::vector< Constraint * > &constraints = this->Constraints.getValues();
    std::vector< Constraint * > newConstraints(0);
    for (std::vector<Constraint *>::const_iterator it = constraints.begin();
         it != constraints.end(); ++it) {
        if ((*it)->First != GeoNbr && (*it)->Second != GeoNbr) {
            if ((*it)->First > GeoNbr)
                (*it)->First -= 1;
            if ((*it)->Second > GeoNbr)
                (*it)->Second -= 1;
            newConstraints.push_back(*it);
        }
    }

    this->Constraints.setValues(newConstraints);
    this->Geometry.setValues(newVals);
    rebuildVertexIndex();
    return 0;
}

int SketchObject::toggleConstruction(int GeoNbr)
{
    const std::vector< Part::Geometry * > &vals = this->Geometry.getValues();
    if (GeoNbr < 0 || GeoNbr >= (int)vals.size())
        return -1;

    std::vector< Part::Geometry * > newVals(vals);

    Part::Geometry *geoNew = newVals[GeoNbr]->clone();
    geoNew->Construction = !geoNew->Construction;
    newVals[GeoNbr]=geoNew;

    this->Geometry.setValues(newVals);
    return 0;
}

int SketchObject::addConstraints(const std::vector<Constraint *> &ConstraintList)
{
    return -1;
}

int SketchObject::addConstraint(const Constraint *constraint)
{
    const std::vector< Constraint * > &vals = this->Constraints.getValues();

    std::vector< Constraint * > newVals(vals);
    Constraint *constNew = constraint->clone();
    newVals.push_back(constNew);
    this->Constraints.setValues(newVals);
    delete constNew;
    return this->Constraints.getSize()-1;
}

int SketchObject::delConstraint(int ConstrNbr)
{
    const std::vector< Constraint * > &vals = this->Constraints.getValues();
    if (ConstrNbr < 0 || ConstrNbr >= (int)vals.size())
        return -1;

    std::vector< Constraint * > newVals(vals);
    newVals.erase(newVals.begin()+ConstrNbr);
    this->Constraints.setValues(newVals);
    return 0;
}

int SketchObject::delConstraintOnPoint(int VertexId)
{
    int GeoId;
    PointPos PosId;
    getGeoVertexIndex(VertexId, GeoId, PosId);
    return delConstraintOnPoint(GeoId, PosId);
}

int SketchObject::delConstraintOnPoint(int GeoId, PointPos PosId)
{
    const std::vector< Constraint * > &vals = this->Constraints.getValues();

    std::vector< Constraint * > newVals(0);
    for (std::vector<Constraint *>::const_iterator it = vals.begin(); it != vals.end(); ++it) {
        if ((*it)->Type == Sketcher::Coincident) {
            if (((*it)->First == GeoId && (*it)->FirstPos == PosId) ||
                ((*it)->Second == GeoId && (*it)->SecondPos == PosId))
               continue;
        }
        newVals.push_back(*it);
    }
    if (newVals.size() < vals.size()) {
        this->Constraints.setValues(newVals);
        return 0;
    }

    return -1; // no such constraint
}

int SketchObject::fillet(int GeoId, PointPos PosId, double radius, bool trim)
{
    const std::vector<Part::Geometry *> &geomlist = this->Geometry.getValues();
    assert(GeoId < int(geomlist.size()));
    // Find the other geometry Id associated with the coincident point
    std::vector<int> GeoIdList;
    std::vector<PointPos> PosIdList;
    getCoincidentPoints(GeoId, PosId, GeoIdList, PosIdList);

    // only coincident points between two edges can be filleted
    if (GeoIdList.size() == 2) {
        Part::Geometry *geo1 = geomlist[GeoIdList[0]];
        Part::Geometry *geo2 = geomlist[GeoIdList[1]];
        if (geo1->getTypeId() == Part::GeomLineSegment::getClassTypeId() &&
            geo2->getTypeId() == Part::GeomLineSegment::getClassTypeId() ) {
            const Part::GeomLineSegment *lineSeg1 = dynamic_cast<const Part::GeomLineSegment*>(geo1);
            const Part::GeomLineSegment *lineSeg2 = dynamic_cast<const Part::GeomLineSegment*>(geo2);

            Base::Vector3d midPnt1 = (lineSeg1->getStartPoint() + lineSeg1->getEndPoint()) / 2 ;
            Base::Vector3d midPnt2 = (lineSeg2->getStartPoint() + lineSeg2->getEndPoint()) / 2 ;
            return fillet(GeoIdList[0], GeoIdList[1], midPnt1, midPnt2, radius, trim);
        }
    }

    return -1;
}

int SketchObject::fillet(int GeoId1, int GeoId2,
                         const Base::Vector3d& refPnt1, const Base::Vector3d& refPnt2,
                         double radius, bool trim)
{
    const std::vector<Part::Geometry *> &geomlist = this->Geometry.getValues();
    assert(GeoId1 < int(geomlist.size()));
    assert(GeoId2 < int(geomlist.size()));
    Part::Geometry *geo1 = geomlist[GeoId1];
    Part::Geometry *geo2 = geomlist[GeoId2];
    if (geo1->getTypeId() == Part::GeomLineSegment::getClassTypeId() &&
        geo2->getTypeId() == Part::GeomLineSegment::getClassTypeId() ) {
        const Part::GeomLineSegment *lineSeg1 = dynamic_cast<const Part::GeomLineSegment*>(geo1);
        const Part::GeomLineSegment *lineSeg2 = dynamic_cast<const Part::GeomLineSegment*>(geo2);

        Base::Vector3d filletCenter;
        if (!Part::findFilletCenter(lineSeg1, lineSeg2, radius, refPnt1, refPnt2, filletCenter))
            return -1;
        Base::Vector3d dir1 = lineSeg1->getEndPoint() - lineSeg1->getStartPoint();
        Base::Vector3d dir2 = lineSeg2->getEndPoint() - lineSeg2->getStartPoint();

        // the intersection point will and two distances will be necessary later for trimming the lines
        Base::Vector3d intersection, dist1, dist2;

        // create arc from known parameters and lines
        int filletId;
        Part::GeomArcOfCircle *arc = Part::createFilletGeometry(lineSeg1, lineSeg2, filletCenter, radius);
        if (arc) {
            // calculate intersection and distances before we invalidate lineSeg1 and lineSeg2
            if (!find2DLinesIntersection(lineSeg1, lineSeg2, intersection)) {
                delete arc;
                return -1;
            }
            dist1.ProjToLine(arc->getStartPoint()-intersection, dir1);
            dist2.ProjToLine(arc->getStartPoint()-intersection, dir2);
            Part::Geometry *newgeo = dynamic_cast<Part::Geometry* >(arc);
            filletId = addGeometry(newgeo);
            if (filletId < 0) {
                delete arc;
                return -1;
            }
        }
        else
            return -1;

        if (trim) {
            PointPos PosId1 = (filletCenter-intersection)*dir1 > 0 ? start : end;
            PointPos PosId2 = (filletCenter-intersection)*dir2 > 0 ? start : end;

            delConstraintOnPoint(GeoId1, PosId1);
            delConstraintOnPoint(GeoId2, PosId2);
            Sketcher::Constraint *tangent1 = new Sketcher::Constraint();
            Sketcher::Constraint *tangent2 = new Sketcher::Constraint();

            tangent1->Type = Sketcher::Tangent;
            tangent1->First = GeoId1;
            tangent1->FirstPos = PosId1;
            tangent1->Second = filletId;

            tangent2->Type = Sketcher::Tangent;
            tangent2->First = GeoId2;
            tangent2->FirstPos = PosId2;
            tangent2->Second = filletId;

            if (dist1.Length() < dist2.Length()) {
                tangent1->SecondPos = start;
                tangent2->SecondPos = end;
                movePoint(GeoId1, PosId1, arc->getStartPoint());
                movePoint(GeoId2, PosId2, arc->getEndPoint());
            }
            else {
                tangent1->SecondPos = end;
                tangent2->SecondPos = start;
                movePoint(GeoId1, PosId1, arc->getEndPoint());
                movePoint(GeoId2, PosId2, arc->getStartPoint());
            }

            addConstraint(tangent1);
            addConstraint(tangent2);
            delete tangent1;
            delete tangent2;
        }
        delete arc;
        return 0;
    }
    return -1;
}

int SketchObject::addExternal(App::DocumentObject *Obj, const char* SubName)
{
    // so far only externals to the support of the sketch
    assert(Support.getValue() == Obj);

    // get the actual lists of the externals
    std::vector<DocumentObject*> Objects     = ExternalConstraints.getValues();
    std::vector<std::string>     SubElements = ExternalConstraints.getSubValues();

    // add the new ones
    Objects.push_back(Obj);
    SubElements.push_back(std::string(SubName));

    // set the Link list.
    ExternalConstraints.setValues(Objects,SubElements);

    return ExternalConstraints.getValues().size()-1;
}

int SketchObject::delExternal(int ConstrNbr)
{
    // FIXME: still to implement
    return 0;

}

std::vector<Part::Geometry *> getExternalGeometry(void)
{
    std::vector<Part::Geometry *> ExtGeos;

    // add the root point (0,0) the the external geos(-1)
    ExtGeos.push_back(new Part::GeomPoint(Base::Vector3d(0,0,0)));

    // add the X,Y (V,H) axis (-2,-3)
    ExtGeos.push_back(new Part::GeomLine(Base::Vector3d(0,0,0),Base::Vector3d(1,0,0)));
    ExtGeos.push_back(new Part::GeomLine(Base::Vector3d(0,0,0),Base::Vector3d(0,1,0)));

    // return the result set
    return ExtGeos;
}


void SketchObject::rebuildVertexIndex(void)
{
    VertexId2GeoId.resize(0);
    VertexId2PosId.resize(0);
    int i=0;
    const std::vector< Part::Geometry * > &geometry = this->Geometry.getValues();
    for (std::vector< Part::Geometry * >::const_iterator it = geometry.begin();
         it != geometry.end(); ++it,i++) {
        if ((*it)->getTypeId() == Part::GeomLineSegment::getClassTypeId()) {
            VertexId2GeoId.push_back(i);
            VertexId2PosId.push_back(start);
            VertexId2GeoId.push_back(i);
            VertexId2PosId.push_back(end);
        } else if ((*it)->getTypeId() == Part::GeomCircle::getClassTypeId()) {
            VertexId2GeoId.push_back(i);
            VertexId2PosId.push_back(mid);
        } else if ((*it)->getTypeId() == Part::GeomArcOfCircle::getClassTypeId()) {
            VertexId2GeoId.push_back(i);
            VertexId2PosId.push_back(mid);
            VertexId2GeoId.push_back(i);
            VertexId2PosId.push_back(start);
            VertexId2GeoId.push_back(i);
            VertexId2PosId.push_back(end);
        }
    }
}

void SketchObject::getCoincidentPoints(int GeoId, PointPos PosId, std::vector<int> &GeoIdList,
                                       std::vector<PointPos> &PosIdList)
{
    const std::vector<Constraint *> &constraints = this->Constraints.getValues();

    GeoIdList.clear();
    PosIdList.clear();
    GeoIdList.push_back(GeoId);
    PosIdList.push_back(PosId);
    for (std::vector<Constraint *>::const_iterator it=constraints.begin();
         it != constraints.end(); ++it) {
        if ((*it)->Type == Sketcher::Coincident) {
            if ((*it)->First == GeoId && (*it)->FirstPos == PosId) {
                GeoIdList.push_back((*it)->Second);
                PosIdList.push_back((*it)->SecondPos);
            }
            else if ((*it)->Second == GeoId && (*it)->SecondPos == PosId) {
                GeoIdList.push_back((*it)->First);
                PosIdList.push_back((*it)->FirstPos);
            }
        }
    }
    if (GeoIdList.size() == 1) {
        GeoIdList.clear();
        PosIdList.clear();
    }
}

void SketchObject::getCoincidentPoints(int VertexId, std::vector<int> &GeoIdList,
                                       std::vector<PointPos> &PosIdList)
{
    int GeoId;
    PointPos PosId;
    getGeoVertexIndex(VertexId, GeoId, PosId);
    getCoincidentPoints(GeoId, PosId, GeoIdList, PosIdList);
}

PyObject *SketchObject::getPyObject(void)
{
    if (PythonObject.is(Py::_None())) {
        // ref counter is set to 1
        PythonObject = Py::Object(new SketchObjectPy(this),true);
    }
    return Py::new_reference_to(PythonObject);
}

unsigned int SketchObject::getMemSize(void) const
{
    return 0;
}

void SketchObject::Save(Writer &writer) const
{
    // save the father classes
    Part::Part2DObject::Save(writer);
}

void SketchObject::Restore(XMLReader &reader)
{
    // read the father classes
    Part::Part2DObject::Restore(reader);
    rebuildVertexIndex();
}

void SketchObject::getGeoVertexIndex(int VertexId, int &GeoId, PointPos &PosId)
{
    if (VertexId < 0 || VertexId >= (int)VertexId2GeoId.size()) {
        GeoId = -1;
        PosId = none;
        return;
    }
    GeoId = VertexId2GeoId[VertexId];
    PosId = VertexId2PosId[VertexId];
}

// Python Sketcher feature ---------------------------------------------------------

namespace App {
/// @cond DOXERR
PROPERTY_SOURCE_TEMPLATE(Sketcher::SketchObjectPython, Sketcher::SketchObject)
template<> const char* Sketcher::SketchObjectPython::getViewProviderName(void) const {
    return "SketcherGui::ViewProviderPython";
}
/// @endcond

// explicit template instantiation
template class SketcherExport FeaturePythonT<Sketcher::SketchObject>;
}
