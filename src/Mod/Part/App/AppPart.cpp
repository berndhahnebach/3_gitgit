/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   J�rgen Riegel 2002                                                    *
 *                                                                         *
 ***************************************************************************/

#include "PreCompiled.h"
#ifndef _PreComp_
# include <Python.h>
#endif

#include <Base/Console.h>

#include "TopoShape.h"
#include "FeaturePartBox.h"
#include "FeaturePartBoolean.h"
#include "FeaturePartCommon.h"
#include "FeaturePartCut.h"
#include "FeaturePartFuse.h"
#include "FeaturePartSection.h"
#include "FeaturePartImportStep.h"
#include "FeaturePartImportIges.h"
#include "FeaturePartImportBrep.h"
#include "FeaturePartCurveNet.h"
#include "FeaturePartLine.h"
#include "FeaturePartLineSet.h"
#include "FeaturePartCircle.h"
#include "FeaturePartPolygon.h"
#include "PrimitiveFeature.h"
#include "PropertyLine.h"
#include "PropertyCircle.h"
#include "TopologyPy.h"
#include "LinePy.h"
#include "CirclePy.h"

extern struct PyMethodDef Part_methods[];

PyDoc_STRVAR(module_part_doc,
"This is a module working with shapes.");

extern "C" {
void AppPartExport initPart()
{
    PyObject* partModule = Py_InitModule3("Part", Part_methods, module_part_doc);   /* mod name, table ptr */
    Base::Console().Log("Loading Part module... done\n");

    // NOTE: To finish the initialization of our own type objects we must
    // call PyType_Ready, otherwise we run into a segmentation fault, later on.
    // This function is responsible for adding inherited slots from a type's base class.
    if(PyType_Ready(&Part::TopoShapePyOld::Type) < 0) return;
    union PyType_Object pyPartType = {&Part::TopoShapePyOld::Type};
    PyModule_AddObject(partModule, "shape", pyPartType.o);
    // Append line() method
    if(PyType_Ready(&Part::LinePy::Type) < 0) return;
    union PyType_Object pyLineType = {&Part::LinePy::Type};
    PyModule_AddObject(partModule, "line", pyLineType.o);
    // Append circle() method
    if(PyType_Ready(&Part::CirclePy::Type) < 0) return;
    union PyType_Object pyCircType = {&Part::CirclePy::Type};
    PyModule_AddObject(partModule, "circle", pyCircType.o);

    Part::TopoShape           ::init();
    Part::PropertyPartShape   ::init();
    Part::PropertyLine        ::init();
    Part::PropertyLineSet     ::init();
    Part::PropertyCircle      ::init();

    Part::Feature             ::init();
    Part::Box                 ::init();
    Part::Boolean             ::init();
    Part::Common              ::init();
    Part::Cut                 ::init();
    Part::Fuse                ::init();
    Part::Section             ::init();
    Part::ImportStep          ::init();
    Part::ImportIges          ::init();
    Part::ImportBrep          ::init();
    Part::CurveNet            ::init();
    Part::Line                ::init();
    Part::LineSet             ::init();
    Part::Polygon             ::init();
    Part::Circle              ::init();
    Part::Plane               ::init();
    Part::Sphere              ::init();
    Part::Cylinder            ::init();
    Part::Cone                ::init();
    Part::Torus               ::init();
}

} // extern "C"
