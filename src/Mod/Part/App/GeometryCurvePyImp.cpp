/***************************************************************************
 *   Copyright (c) 2009 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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
# include <BRepBuilderAPI_MakeEdge.hxx>
# include <gp_Dir.hxx>
# include <gp_Vec.hxx>
# include <Geom_Geometry.hxx>
# include <Geom_Curve.hxx>
# include <GeomLProp_CLProps.hxx>
# include <Precision.hxx>
# include <Standard_Failure.hxx>
#endif

#include <Base/GeometryPyCXX.h>
#include <Base/VectorPy.h>

#include "Geometry.h"
#include "GeometryCurvePy.h"
#include "GeometryCurvePy.cpp"

#include "TopoShape.h"
#include "TopoShapePy.h"
#include "TopoShapeEdgePy.h"

using namespace Part;

// returns a string which represents the object e.g. when printed in python
std::string GeometryCurvePy::representation(void) const
{
    return "<Curve object>";
}

PyObject *GeometryCurvePy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // never create such objects with the constructor
    PyErr_SetString(PyExc_RuntimeError,
        "You cannot create an instance of the abstract class 'GeometryCurve'.");
    return 0;
}

// constructor method
int GeometryCurvePy::PyInit(PyObject* /*args*/, PyObject* /*kwd*/)
{
    return 0;
}

PyObject* GeometryCurvePy::toShape(PyObject *args)
{
    Handle_Geom_Geometry g = getGeometryPtr()->handle();
    Handle_Geom_Curve c = Handle_Geom_Curve::DownCast(g);
    try {
        if (!c.IsNull()) {
            double u,v;
            u=c->FirstParameter();
            v=c->LastParameter();
            if (!PyArg_ParseTuple(args, "|dd", &u,&v))
                return 0;
            BRepBuilderAPI_MakeEdge mkBuilder(c, u, v);
            TopoDS_Shape sh = mkBuilder.Shape();
            return new TopoShapeEdgePy(new TopoShape(sh));
        }
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }

    PyErr_SetString(PyExc_Exception, "Geometry is not a curve");
    return 0;
}

PyObject* GeometryCurvePy::value(PyObject *args)
{
    Handle_Geom_Geometry g = getGeometryPtr()->handle();
    Handle_Geom_Curve c = Handle_Geom_Curve::DownCast(g);
    try {
        if (!c.IsNull()) {
            double u;
            if (!PyArg_ParseTuple(args, "d", &u))
                return 0;
            gp_Pnt p = c->Value(u);
            return new Base::VectorPy(Base::Vector3d(p.X(),p.Y(),p.Z()));
        }
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }

    PyErr_SetString(PyExc_Exception, "Geometry is not a curve");
    return 0;
}

PyObject* GeometryCurvePy::tangent(PyObject *args)
{
    Handle_Geom_Geometry g = getGeometryPtr()->handle();
    Handle_Geom_Curve c = Handle_Geom_Curve::DownCast(g);
    try {
        if (!c.IsNull()) {
            double u;
            if (!PyArg_ParseTuple(args, "d", &u))
                return 0;
            gp_Dir dir;
            Py::Tuple tuple(1);
            GeomLProp_CLProps prop(c,u,1,Precision::Confusion());
            if (prop.IsTangentDefined()) {
                prop.Tangent(dir);
                tuple.setItem(0, Py::Vector(Base::Vector3d(dir.X(),dir.Y(),dir.Z())));
            }

            return Py::new_reference_to(tuple);
        }
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }

    PyErr_SetString(PyExc_Exception, "Geometry is not a curve");
    return 0;
}

PyObject *GeometryCurvePy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int GeometryCurvePy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}
