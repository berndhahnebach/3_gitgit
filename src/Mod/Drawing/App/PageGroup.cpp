/***************************************************************************
 *   Copyright (c) J�rgen Riegel          (juergen.riegel@web.de) 2002     *
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
# include <sstream>

#endif



#include "PageGroup.h"

using namespace Drawing;


//===========================================================================
// FeatureView
//===========================================================================

PROPERTY_SOURCE(Drawing::PageGroup, App::DocumentObjectGroup)



PageGroup::PageGroup(void) 
{
  static const char *group = "Drawings";
  ADD_PROPERTY_TYPE(Pages ,(0),group,App::Prop_None,"List of pages");
}

PageGroup::~PageGroup()
{
}


//
//PyObject *PageGroup::getPyObject(void)
//{
// if(PythonObject.is(Py::_None())){
//    // ref counter is set to 1
//    PythonObject.set(new PageGroupPy(this),true);
//  }
//  return Py::new_reference_to(PythonObject); 
//}


