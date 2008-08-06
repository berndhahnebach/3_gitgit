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


#ifndef __PRECOMPILED__
#define __PRECOMPILED__

#include "../FCConfig.h"


/// here get the warnings of to long specifieres disabled (needed for VC6)
#ifdef _MSC_VER
#pragma warning( disable : 4251 )
#pragma warning( disable : 4273 )
#pragma warning( disable : 4275 )
#pragma warning( disable : 4503 )
#pragma warning( disable : 4786 )  // specifier longer then 255 chars
#endif


// standard
#include <stdio.h>
#include <assert.h>
#include <time.h>

#ifdef FC_OS_WIN32
#include <direct.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <crtdbg.h>
#endif


// Streams
#include <iostream>
#include <sstream>

// STL 
#include <string>
#include <list>
#include <map>
#include <vector>
#include <set>
#include <stack>
#include <sstream>
#include <queue>
#include <bitset>

// Boost
#include <boost/signals.hpp>
#include <boost/bind.hpp>

#include <boost/tuple/tuple.hpp>
#include <boost/utility.hpp>
#include <boost/graph/adjacency_list.hpp>

#include <boost/program_options.hpp>
//namespace po = boost::program_options;

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>

// xerces
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/PlatformUtils.hpp>


#endif
