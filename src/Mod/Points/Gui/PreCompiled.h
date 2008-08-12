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


#ifndef __PRECOMPILED_GUI__
#define __PRECOMPILED_GUI__

#include <FCConfig.h>

// Make sure to explicitly use the correct conversion
#define QT_NO_CAST_FROM_ASCII

// Importing of App classes
#ifdef FC_OS_WIN32
# define PointsAppExport __declspec(dllimport)
# define PointsGuiExport __declspec(dllexport)
#else // for Linux
# define PointsAppExport
# define PointsGuiExport
#endif

#ifdef _PreComp_

// standard
#include <stdio.h>
#include <assert.h>

// STL
#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <vector>
#include <bitset>

// Xerces
#include <xercesc/util/XercesDefs.hpp>

#include <Python.h>
#ifdef FC_OS_WIN32
# include <windows.h>
#endif


// Qt Toolkit
#ifndef __Qt4All__
# include <Gui/Qt4All.h>
#endif

// Inventor
#ifndef __InventorAll__
# include <Gui/InventorAll.h>
#endif


#endif  //_PreComp_

#endif // __PRECOMPILED_GUI__ 
