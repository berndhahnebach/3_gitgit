/***************************************************************************
 *   Copyright (c) J�rgen Riegel          <juergen.riegel@web.de>          *
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

#ifndef __MEMDEBUG_H__
#define __MEMDEBUG_H__

// Std. configurations
#if defined(_MSC_VER)

namespace Base
{

class BaseExport MemCheck
{
public:
    MemCheck();
    ~MemCheck();

    void setNextCheckpoint();
    static bool checkMemory();
    static bool dumpLeaks();
    static bool isValidHeapPointer(const void*);

private:
    _CrtMemState s1, s2, s3;
};

} //namespace Base

#endif

#endif // __FILETEMPLATE_H__

