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
#endif



#include "SketchObject.h"
#include "SketchFlatInterface.h"


using namespace Sketcher;


PROPERTY_SOURCE(Sketcher::SketchObject, Part::Part2DObject)


SketchObject::SketchObject()
{
    ADD_PROPERTY_TYPE(SketchFlatFile,(0),"",(App::PropertyType)(App::Prop_None),"SketchFlat file (*.skf) which defines this sketch");
}

short SketchObject::mustExecute() const
{
    if ( SketchFlatFile.isTouched() )
        return 1;
    return 0;
}


App::DocumentObjectExecReturn *SketchObject::execute(void)
{
    SketchFlatInterface temp;
    temp.load(SketchFlatFile.getValue());

	this->Shape.setValue(temp.getGeoAsShape());

    return App::DocumentObject::StdReturn;
}
