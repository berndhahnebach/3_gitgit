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


#ifndef SKETCHER_SKETCHFLATINTERFACE_H
#define SKETCHER_SKETCHFLATINTERFACE_H

// Std. configurations


#include <string>

namespace Sketcher
{


/** Interface to the sketch flat constraint solver.
 */
class AppSketcherExport SketchFlatInterface
{
public:
    /// Construction
    SketchFlatInterface();
    /// Destruction
    virtual ~SketchFlatInterface();

    int testMe(int a,const char *s);

 };

} //namespace Base

#endif // SKETCHER_SKETCHFLATINTERFACE_H

