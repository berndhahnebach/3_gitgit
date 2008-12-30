/***************************************************************************
 *   Copyright (c) J�rgen Riegel	        <FreeCAD@juergen-riegel.net    *
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

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include "SketchFlatInterface.h"
#include "SketchFlat/sketchflat.h"

using namespace Sketcher;

bool SketchFlatInterface::bAlive = false;

//**************************************************************************
// Construction/Destruction

/**
 * A constructor.
 * A more elaborate description of the constructor.
 */
SketchFlatInterface::SketchFlatInterface()
{
	// The SketchFlat solver is not build ot have 2 instances!
	assert(!bAlive);
	bAlive=true;

}

/**
 * A destructor.
 * A more elaborate description of the destructor.
 */
SketchFlatInterface::~SketchFlatInterface()
{
	bAlive=false;
}


//**************************************************************************
// separator for other implemetation aspects

/**
 */
unsigned int SketchFlatInterface::AddLine(double x, double y)
{
	hEntity he;
	he = SketchAddEntity(ENTITY_LINE_SEGMENT);
	ForcePoint(POINT_FOR_ENTITY(he, 0), x, y);
    return he;
}


//**************************************************************************



