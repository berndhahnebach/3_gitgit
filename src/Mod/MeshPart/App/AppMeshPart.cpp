/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   J�rgen Riegel 2007                                                    *
 *                                                                         *
 ***************************************************************************/

#include "PreCompiled.h"
#ifndef _PreComp_
# include <Python.h>
#endif

#include <Base/Console.h>
#include <Base/Interpreter.h>
 

extern struct PyMethodDef MeshPart_methods[];

PyDoc_STRVAR(module_MeshPart_doc,
"This module is the MeshPart module.");


/* Python entry */
extern "C" {
void AppMeshPartExport initMeshPart()
{
    // load dependend module
    try {
        Base::Interpreter().loadModule("Part");
        //Base::Interpreter().loadModule("Mesh");
    }
    catch(const Base::Exception& e) {
        PyErr_SetString(PyExc_ImportError, e.what());
        return;
    }
    Py_InitModule3("MeshPart", MeshPart_methods, module_MeshPart_doc);   /* mod name, table ptr */
    Base::Console().Log("Loading MeshPart module... done\n");


    // NOTE: To finish the initialization of our own type objects we must
    // call PyType_Ready, otherwise we run into a segmentation fault, later on.
    // This function is responsible for adding inherited slots from a type's base class.
 
    //MeshPart::FeatureViewPart        ::init();
}

} // extern "C"
