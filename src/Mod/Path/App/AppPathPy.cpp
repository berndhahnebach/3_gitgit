/***************************************************************************
 *   Copyright (c) Yorik van Havre (yorik@uncreated.net) 2014              *
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
# include <Python.h>
#endif

#include <CXX/Extensions.hxx>
#include <CXX/Objects.hxx>

#include <Base/Console.h>
#include <Base/VectorPy.h>
#include <Base/FileInfo.h>
#include <Base/Interpreter.h>
#include <App/Document.h>
#include <App/DocumentObjectPy.h>
#include <App/Application.h>

#include <Mod/Part/App/OCCError.h>
#include <Mod/Part/App/TopoShape.h>
#include <Mod/Part/App/TopoShapePy.h>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopExp_Explorer.hxx>
#include <gp_Lin.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_CompCurve.hxx>
#include <BRepAdaptor_HCompCurve.hxx>
#include <Approx_Curve3d.hxx>
#include <BRepAdaptor_HCurve.hxx>

#include "CommandPy.h"
#include "PathPy.h"
#include "Path.h"
#include "FeaturePath.h"
#include "FeaturePathCompound.h"
#include "Area.h"

#define PATH_CATCH catch (Standard_Failure &e)                      \
    {                                                               \
        std::string str;                                            \
        Standard_CString msg = e.GetMessageString();                \
        str += typeid(e).name();                                    \
        str += " ";                                                 \
        if (msg) {str += msg;}                                      \
        else     {str += "No OCCT Exception Message";}              \
        Base::Console().Error(str.c_str());                         \
        PyErr_SetString(Part::PartExceptionOCCError,str.c_str());   \
    }                                                               \
    catch(Base::Exception &e)                                       \
    {                                                               \
        std::string str;                                            \
        str += "FreeCAD exception thrown (";                        \
        str += e.what();                                            \
        str += ")";                                                 \
        e.ReportException();                                        \
        PyErr_SetString(Base::BaseExceptionFreeCADError,str.c_str());\
    }                                                               \
    catch(std::exception &e)                                        \
    {                                                               \
        std::string str;                                            \
        str += "STL exception thrown (";                            \
        str += e.what();                                            \
        str += ")";                                                 \
        Base::Console().Error(str.c_str());                         \
        PyErr_SetString(Base::BaseExceptionFreeCADError,str.c_str());\
    }                                                               \
    catch(const char *e)                                            \
    {                                                               \
        PyErr_SetString(Base::BaseExceptionFreeCADError,e);         \
    } throw Py::Exception();                                                               

namespace Part {
extern PartExport Py::Object shape2pyshape(const TopoDS_Shape &shape);
}

namespace Path {
class Module : public Py::ExtensionModule<Module>
{
    
public:

    Module() : Py::ExtensionModule<Module>("Path")
    {
        add_varargs_method("write",&Module::write,
            "write(object,filename): Exports a given path object to a GCode file"
        );
        add_varargs_method("read",&Module::read,
            "read(filename,[document]): Imports a GCode file into the given document"
        );
        add_varargs_method("show",&Module::show,
            "show(path): Add the path to the active document or create one if no document exists"
        );
        add_varargs_method("fromShape",&Module::fromShape,
            "fromShape(Shape): Returns a Path object from a Part Shape"
        );
        add_keyword_method("fromShapes",&Module::fromShapes,
            "fromShapes(shapes, start=Vector(), " PARAM_PY_ARGS_DOC(ARG,AREA_PARAMS_PATH) ", key=value...)\n"
            "\nReturns a Path object from a list of shapes\n"
            "\n* shapes: input list of shapes.\n"
            "\n* start (Vector()): optional start position.\n"
            PARAM_PY_DOC(ARG, AREA_PARAMS_PATH)
            "\n* <key>: any key supported by Path.Area, see Path.Area.getParamDesc() for description"
        );
        add_keyword_method("sortWires",&Module::sortWires,
            "sortWires(shapes, start=Vector(), "  PARAM_PY_ARGS_DOC(ARG,AREA_PARAMS_SORT) ", key=value...)\n"
            "\nReturns (wires,end), where 'wires' is sorted accross Z value and with optimized travel distance,\n"
            "and 'end' is the ending position of the whole wires\n"
            "\n* shapes: input shape list\n"
            "\n* start (Vector()): optional start position.\n"
            PARAM_PY_DOC(ARG, AREA_PARAMS_SORT)
            "\n* <key>: any key supported by Path.Area, see Path.Area.getParamDesc() for description"
        );
        initialize("This module is the Path module."); // register with Python
    }

    virtual ~Module() {}

private:

    Py::Object write(const Py::Tuple& args)
    {
        char* Name;
        PyObject* pObj;
        if (!PyArg_ParseTuple(args.ptr(), "Oet",&pObj,"utf-8",&Name))
            throw Py::Exception();
        std::string EncodedName = std::string(Name);
        PyMem_Free(Name);
        Base::FileInfo file(EncodedName.c_str());
        
        if (PyObject_TypeCheck(pObj, &(App::DocumentObjectPy::Type))) {
            App::DocumentObject* obj = static_cast<App::DocumentObjectPy*>(pObj)->getDocumentObjectPtr();
            if (obj->getTypeId().isDerivedFrom(Base::Type::fromName("Path::Feature"))) {
                const Toolpath& path = static_cast<Path::Feature*>(obj)->Path.getValue();
                std::string gcode = path.toGCode();    
                std::ofstream ofile(EncodedName.c_str());
                ofile << gcode;
                ofile.close();
            }
            else {
                throw Py::RuntimeError("The given file is not a path");
            }
        }

        return Py::None();
    }


    Py::Object read(const Py::Tuple& args)
    {
        char* Name;
        const char* DocName=0;
        if (!PyArg_ParseTuple(args.ptr(), "et|s","utf-8",&Name,&DocName))
            throw Py::Exception();
        std::string EncodedName = std::string(Name);
        PyMem_Free(Name);

        Base::FileInfo file(EncodedName.c_str());
        if (!file.exists())
            throw Py::RuntimeError("File doesn't exist");

        App::Document *pcDoc;
        if (DocName)
            pcDoc = App::GetApplication().getDocument(DocName);
        else
            pcDoc = App::GetApplication().getActiveDocument();
        if (!pcDoc) 
            pcDoc = App::GetApplication().newDocument(DocName);

        try {
            // read the gcode file
            std::ifstream filestr(file.filePath().c_str());
            std::stringstream buffer;
            buffer << filestr.rdbuf();
            std::string gcode = buffer.str();
            Toolpath path;
            path.setFromGCode(gcode);
            Path::Feature *object = static_cast<Path::Feature *>(pcDoc->addObject("Path::Feature",file.fileNamePure().c_str()));
            object->Path.setValue(path);
            pcDoc->recompute();
        }
        catch (const Base::Exception& e) {
            throw Py::RuntimeError(e.what());
        }

        return Py::None();
    }


    Py::Object show(const Py::Tuple& args)
    {
        PyObject *pcObj;
        if (!PyArg_ParseTuple(args.ptr(), "O!", &(PathPy::Type), &pcObj))
            throw Py::Exception();

        try {
            App::Document *pcDoc = App::GetApplication().getActiveDocument(); 	 
            if (!pcDoc)
                pcDoc = App::GetApplication().newDocument();
            PathPy* pPath = static_cast<PathPy*>(pcObj);
            Path::Feature *pcFeature = (Path::Feature *)pcDoc->addObject("Path::Feature", "Path");
            Path::Toolpath* pa = pPath->getToolpathPtr();
            if (!pa) {
                throw Py::Exception(PyExc_ReferenceError, "object doesn't reference a valid path");
            }

            // copy the data
            pcFeature->Path.setValue(*pa);
        }
        catch (const Base::Exception& e) {
            throw Py::RuntimeError(e.what());
        }

        return Py::None();
    }


    Py::Object fromShape(const Py::Tuple& args)
    {
        PyObject *pcObj;
        if (!PyArg_ParseTuple(args.ptr(), "O", &pcObj))
            throw Py::Exception();
        TopoDS_Shape shape;
        try {
            if (PyObject_TypeCheck(pcObj, &(Part::TopoShapePy::Type))) {
                shape = static_cast<Part::TopoShapePy*>(pcObj)->getTopoShapePtr()->getShape();
            } else {
                throw Py::TypeError("the given object is not a shape");
            }
            if (!shape.IsNull()) {
                if (shape.ShapeType() == TopAbs_WIRE) {
                    Path::Toolpath result;
                    bool first = true;
                    Base::Placement last;
                    
                    TopExp_Explorer ExpEdges (shape,TopAbs_EDGE);
                    while (ExpEdges.More()) {
                        const TopoDS_Edge& edge = TopoDS::Edge(ExpEdges.Current());
                        TopExp_Explorer ExpVerts(edge,TopAbs_VERTEX);
                        bool vfirst = true;
                        while (ExpVerts.More()) {
                            const TopoDS_Vertex& vert = TopoDS::Vertex(ExpVerts.Current());
                            gp_Pnt pnt = BRep_Tool::Pnt(vert);
                            Base::Placement tpl;
                            tpl.setPosition(Base::Vector3d(pnt.X(),pnt.Y(),pnt.Z()));
                            if (first) {
                                // add first point as a G0 move
                                Path::Command cmd;
                                std::ostringstream ctxt;
                                ctxt << "G0 X" << tpl.getPosition().x << " Y" << tpl.getPosition().y << " Z" << tpl.getPosition().z;
                                cmd.setFromGCode(ctxt.str());
                                result.addCommand(cmd);
                                first = false;
                                vfirst = false;
                            } else {
                                if (vfirst)
                                    vfirst = false;
                                else {
                                    Path::Command cmd;
                                    cmd.setFromPlacement(tpl);
                        
                                    // write arc data if needed
                                    BRepAdaptor_Curve adapt(edge);
                                    if (adapt.GetType() == GeomAbs_Circle) {
                                        gp_Circ circ = adapt.Circle();
                                        gp_Pnt c = circ.Location();
                                        bool clockwise = false;
                                        gp_Dir n = circ.Axis().Direction();
                                        if (n.Z() < 0)
                                            clockwise = true;
                                        Base::Vector3d center = Base::Vector3d(c.X(),c.Y(),c.Z());
                                        // center coords must be relative to last point
                                        center -= last.getPosition();
                                        cmd.setCenter(center,clockwise);
                                    }
                                    result.addCommand(cmd);
                                }
                            }
                            ExpVerts.Next();
                            last = tpl;
                        }
                        ExpEdges.Next();
                    }
                    return Py::asObject(new PathPy(new Path::Toolpath(result)));
                } else {
                    throw Py::TypeError("the given shape must be a wire");
                }
            } else {
                throw Py::TypeError("the given shape is empty");
            }
        }
        catch (const Base::Exception& e) {
            throw Py::RuntimeError(e.what());
        }
    }

    Py::Object fromShapes(const Py::Tuple& args, const Py::Dict &kwds)
    {
        PARAM_PY_DECLARE_INIT(PARAM_FARG,AREA_PARAMS_PATH)
        PARAM_PY_DECLARE_INIT(PARAM_FNAME,AREA_PARAMS_CONF)
        PyObject *pShapes=NULL;
        PyObject *start=NULL;
        static char* kwd_list[] = {"shapes", "start", 
                PARAM_FIELD_STRINGS(ARG,AREA_PARAMS_PATH), 
                PARAM_FIELD_STRINGS(NAME,AREA_PARAMS_CONF), NULL};
        if (!PyArg_ParseTupleAndKeywords(args.ptr(), kwds.ptr(), 
                "O|O!" PARAM_PY_KWDS(AREA_PARAMS_PATH) PARAM_PY_KWDS(AREA_PARAMS_CONF), 
                kwd_list, &pShapes, &(Base::VectorPy::Type), &start, 
                PARAM_REF(PARAM_FARG,AREA_PARAMS_PATH),
                PARAM_REF(PARAM_FNAME,AREA_PARAMS_CONF)))
            throw Py::Exception();

        std::list<TopoDS_Shape> shapes;
        if (PyObject_TypeCheck(pShapes, &(Part::TopoShapePy::Type)))
            shapes.push_back(static_cast<Part::TopoShapePy*>(pShapes)->getTopoShapePtr()->getShape());
        else if (PyObject_TypeCheck(pShapes, &(PyList_Type)) ||
                PyObject_TypeCheck(pShapes, &(PyTuple_Type))) 
        {
            Py::Sequence shapeSeq(pShapes);
            for (Py::Sequence::iterator it = shapeSeq.begin(); it != shapeSeq.end(); ++it) {
                PyObject* item = (*it).ptr();
                if(!PyObject_TypeCheck(item, &(Part::TopoShapePy::Type))) {
                    PyErr_SetString(PyExc_TypeError, "non-shape object in sequence");
                    throw Py::Exception();
                }
                shapes.push_back(static_cast<Part::TopoShapePy*>(item)->getTopoShapePtr()->getShape());
            }
        }

#define AREA_GET(_param) \
        params.PARAM_FNAME(_param) = \
            PARAM_TYPED(PARAM_CAST_PY_,_param)(PARAM_FNAME(_param));
        AreaParams params;
        PARAM_FOREACH(AREA_GET,AREA_PARAMS_CONF)

        gp_Pnt pstart;
        if(start) {
            Base::Vector3d vec = static_cast<Base::VectorPy*>(start)->value();
            pstart.SetCoord(vec.x, vec.y, vec.z);
        }

        try {
            std::unique_ptr<Toolpath> path(new Toolpath);
            Area::toPath(*path,shapes,&params, &pstart, NULL,
                    PARAM_PY_FIELDS(PARAM_FARG,AREA_PARAMS_PATH));
            return Py::asObject(new PathPy(path.release()));
        } PATH_CATCH
    }

    Py::Object sortWires(const Py::Tuple& args, const Py::Dict &kwds)
    {
        PARAM_PY_DECLARE_INIT(PARAM_FARG,AREA_PARAMS_SORT)
        PARAM_PY_DECLARE_INIT(PARAM_FNAME,AREA_PARAMS_CONF)
        PyObject *pShapes=NULL;
        PyObject *start=NULL;
        static char* kwd_list[] = {"shapes", "start", 
                PARAM_FIELD_STRINGS(ARG,AREA_PARAMS_SORT), 
                PARAM_FIELD_STRINGS(NAME,AREA_PARAMS_CONF), NULL};
        if (!PyArg_ParseTupleAndKeywords(args.ptr(), kwds.ptr(), 
                "O|O!" PARAM_PY_KWDS(AREA_PARAMS_SORT) PARAM_PY_KWDS(AREA_PARAMS_CONF), 
                kwd_list, &pShapes, &(Base::VectorPy::Type), &start, 
                PARAM_REF(PARAM_FARG,AREA_PARAMS_SORT),
                PARAM_REF(PARAM_FNAME,AREA_PARAMS_CONF)))
            throw Py::Exception();

        std::list<TopoDS_Shape> shapes;
        if (PyObject_TypeCheck(pShapes, &(Part::TopoShapePy::Type)))
            shapes.push_back(static_cast<Part::TopoShapePy*>(pShapes)->getTopoShapePtr()->getShape());
        else if (PyObject_TypeCheck(pShapes, &(PyList_Type)) ||
                PyObject_TypeCheck(pShapes, &(PyTuple_Type))) {
            Py::Sequence shapeSeq(pShapes);
            for (Py::Sequence::iterator it = shapeSeq.begin(); it != shapeSeq.end(); ++it) {
                PyObject* item = (*it).ptr();
                if(!PyObject_TypeCheck(item, &(Part::TopoShapePy::Type))) {
                    PyErr_SetString(PyExc_TypeError, "non-shape object in sequence");
                    throw Py::Exception();
                }
                shapes.push_back(static_cast<Part::TopoShapePy*>(item)->getTopoShapePtr()->getShape());
            }
        }

        AreaParams params;
        PARAM_FOREACH(AREA_GET,AREA_PARAMS_CONF)

        gp_Pnt pstart,pend;
        if(start) {
            Base::Vector3d vec = static_cast<Base::VectorPy*>(start)->value();
            pstart.SetCoord(vec.x, vec.y, vec.z);
        }
        
        try {
            std::list<TopoDS_Shape> wires = Area::sortWires(shapes,&params,&pstart,
                    &pend, PARAM_PY_FIELDS(PARAM_FARG,AREA_PARAMS_SORT));
            PyObject *list = PyList_New(0);
            for(auto &wire : wires)
                PyList_Append(list,Py::new_reference_to(
                            Part::shape2pyshape(TopoDS::Wire(wire))));
            PyObject *ret = PyTuple_New(2);
            PyTuple_SetItem(ret,0,list);
            PyTuple_SetItem(ret,1,new Base::VectorPy(
                        Base::Vector3d(pend.X(),pend.Y(),pend.Z())));
            return Py::asObject(ret);
        } PATH_CATCH
    }
};

PyObject* initModule()
{
    return (new Module)->module().ptr();
}

} // namespace Path
