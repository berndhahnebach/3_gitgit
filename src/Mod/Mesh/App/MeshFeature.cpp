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

#ifndef _PreComp_
#endif

#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/Reader.h>
#include <Base/Writer.h>

#include "Core/MeshIO.h"

#include "MeshFeature.h"
#include "MeshFeaturePy.h"
#include <App/Feature.h>

using namespace Mesh;


//===========================================================================
// Feature
//===========================================================================

PROPERTY_SOURCE(Mesh::Feature, App::GeoFeature)

Feature::Feature()
{
    ADD_PROPERTY_TYPE(Mesh,(MeshObject()),0,App::Prop_Output,"The mesh kernel");
}

Feature::~Feature()
{
}

App::DocumentObjectExecReturn *Feature::execute(void)
{
    return App::DocumentObject::StdReturn;
}

PyObject *Feature::getPyObject(void)
{
    if(PythonObject.is(Py::_None())){
        // ref counter is set to 1
        PythonObject = Py::Object(new MeshFeaturePy(this),true);
    }
    return Py::new_reference_to(PythonObject); 
}

void Feature::onChanged(const App::Property* prop)
{
    // Note: If the Mesh property has changed the property and this object are marked as 'touched'
    // but no recomputation of this objects needs to be done because the Mesh property is regarded
    // as output of a previous recomputation The mustExecute() method returns 0 in that case. 
    // However, objects that reference this object the Mesh property can be an input parameter.
    // As this object and the property are touched such objects can check this and return a value 1 
    // (or -1) in their mustExecute() to be recomputed the next time the document recomputes itself.
    DocumentObject::onChanged(prop);
}
