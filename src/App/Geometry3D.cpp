/** \file Geometry3D.cpp
 *  \author $Author: wmayer $
 *  \version $Revision: 2.1 $
 *  \date    $Date: 2006/01/24 14:26:42 $
 */

/***************************************************************************
 *   (c) J�rgen Riegel (juergen.riegel@web.de) 2002                        *   
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License (LGPL)   *
 *   as published by the Free Software Foundation; either version 2 of     *
 *   the License, or (at your option) any later version.                   *
 *   for detail see the LICENCE text file.                                 *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        * 
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with FreeCAD; if not, write to the Free Software        * 
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
 *   USA                                                                   *
 *                                                                         *
 *   Juergen Riegel 2002                                                   *
 ***************************************************************************/


#include "PreCompiled.h"

#ifndef _PreComp_
#	include <assert.h>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include "Geometry3D.h"
#include "../Base/Exception.h"
#include "../Base/Console.h"

#define new DEBUG_CLIENTBLOCK

//===========================================================================
// FCGeometry3D - Warpper for the TopoDS classes
//===========================================================================

//--------------------------------------------------------------------------
// Exported functions
//--------------------------------------------------------------------------



//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------

PyTypeObject FCGeometry3D::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,						/*ob_size*/
	"FCLabel",				/*tp_name*/
	sizeof(FCGeometry3D),	/*tp_basicsize*/
	0,						/*tp_itemsize*/
	/* methods */
	PyDestructor,	  		/*tp_dealloc*/
	0,			 			/*tp_print*/
	__getattr, 				/*tp_getattr*/
	__setattr, 				/*tp_setattr*/
	0,						/*tp_compare*/
	__repr,					/*tp_repr*/
	0,						/*tp_as_number*/
	0,						/*tp_as_sequence*/
	0,						/*tp_as_mapping*/
	0,						/*tp_hash*/
	0,						/*tp_call */
};

//--------------------------------------------------------------------------
// Methods structure
//--------------------------------------------------------------------------
PyMethodDef FCGeometry3D::Methods[] = {
  {"HasChild",         (PyCFunction) sPyHasChild,         Py_NEWARGS},

  {NULL, NULL}		/* Sentinel */
};

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject FCGeometry3D::Parents[] = {&FCGeometry3D::Type, NULL};     

//--------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------
FCGeometry3D::FCGeometry3D(const TopoDS_Shape &cShape, PyTypeObject *T) 
 : PyObjectBase( T), _cTopoShape(cShape)
{
	Console().Log("Create TopoShape %p\n",this);
}

PyObject *FCGeometry3D::PyMake(PyObject *ignored, PyObject *args)	// Python wrapper
{
  return new FCGeometry3D(TopoDS_Shape());			// Make new Python-able object
}

//--------------------------------------------------------------------------
//  FCLabel destructor 
//--------------------------------------------------------------------------
FCGeometry3D::~FCGeometry3D()						// Everything handled in parent
{
	Console().Log("Destroy TopoShape %p\n",this);
} 

//--------------------------------------------------------------------------
// FCLabel Attributes
//--------------------------------------------------------------------------
PyObject *FCGeometry3D::_getattr(char *attr)				// __getattr__ function: note only need to handle new state
{ 
	try{
		// Access the number of attributes at this label
		if (streq(attr, "AttributeCount"))
		{
			return Py_BuildValue("i", 1); 
		}else
			_getattr_up(PyObjectBase); 						// send to parent
	}catch(...){
		Console().Log("Exception in FCGeometry3D::_getattr()\n");
		return 0;
	}
} 

int FCGeometry3D::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
{ 
	if (streq(attr, "Real"))						// settable new state
		; 
	else  
		return PyObjectBase::_setattr(attr, value);	// send up to parent
	return 0;
} 


//--------------------------------------------------------------------------
// Python wrappers
//--------------------------------------------------------------------------

PyObject *FCGeometry3D::PyHasChild(PyObject *args)
{ 
    if (!PyArg_ParseTuple(args, "" ))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 
	Py_Return; 
}
 




//**************************************************************************
//**************************************************************************
// Seperator for additional classes
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



