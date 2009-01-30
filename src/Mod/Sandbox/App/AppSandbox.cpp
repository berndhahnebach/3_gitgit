/***************************************************************************
 *   Copyright (c) YEAR YOUR NAME         <Your e-mail address>            *
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
# include <Python.h>
#endif

#include <Base/Console.h>
#include <CXX/Extensions.hxx>
#include <CXX/Objects.hxx>


/* module functions */

class SandboxModule : public Py::ExtensionModule<SandboxModule>
{

public:
    SandboxModule() : Py::ExtensionModule<SandboxModule>("SandboxModule")
    {
        initialize("This module is the Sandbox module"); // register with Python
    }
    
    virtual ~SandboxModule() {}

private:
};


/* Python entry */
extern "C" {
void SandboxAppExport initSandbox() {

    // the following constructor call registers our extension module
    // with the Python runtime system
    static SandboxModule* module = new SandboxModule;
    Base::Console().Log("Loading Sandbox module... done\n");
}

} // extern "C"
