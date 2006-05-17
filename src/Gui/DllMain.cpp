/***************************************************************************
 *   Copyright (c) 2004 J�rgen Riegel <juergen.riegel@web.de>              *
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


// === Incuding of libs: ============================================================================
#include "../FCConfig.h"
#ifndef FC_OS_WIN32
# error "Dont compile that file on UNIX!"
#endif

#include <iostream>
#include <windows.h>
#include <direct.h>
#include <stdio.h>
#include <string>

#ifdef FC_OS_WIN32
#	include <direct.h>
# include <windows.h>
#endif
#include "../Base/EnvMacros.h"

/** DllMain
 *  is called when DLL is loaded and set some variables different from OCC
 *  especially to use the one plugin files
 */

BOOL APIENTRY DllMain(HANDLE hModule, 
                      DWORD  ul_reason_for_call, 
                      LPVOID lpReserved)
{
  switch( ul_reason_for_call ) {
    case DLL_PROCESS_ATTACH:
    //case DLL_THREAD_ATTACH:
    // set the resource env variables

    EnvPrint("Gui ==============================================");

    std::string cHomePath = FindHomePathWin32(hModule);


    // try to figure out if using FreeCADLib
    std::string Temp = GetFreeCADLib(cHomePath.c_str());

    // sets all needed varables if a FreeCAD LibPack is found
    if(Temp != "")
    {
      // sets the python environment variables if the FREECADLIB variable is defined
      SetPythonToFreeCADLib(Temp.c_str());

      // sets the OpenCasCade environment variables if the FREECADLIB variable is defined
      SetCasCadeToFreeCADLib(Temp.c_str());
    }

    SetPluginDefaults(cHomePath.c_str());

    break;

    //case DLL_THREAD_ATTACH:
    //case DLL_THREAD_DETACH:
    //case DLL_PROCESS_DETACH:
    }
    return TRUE;
}

