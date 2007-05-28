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

#include <Base/PyObjectBase.h>
#include <Base/Console.h>
#include <Base/Exception.h>
using Base::Console;

#include "Material.h"
#include "MaterialPy.h"
#define new DEBUG_CLIENTBLOCK
using namespace App;




//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------

PyTypeObject App::MaterialPy::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,						/*ob_size*/
	"Material",				/*tp_name*/
	sizeof(MaterialPy),			/*tp_basicsize*/
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
  0,                                                /*tp_str  */
  0,                                                /*tp_getattro*/
  0,                                                /*tp_setattro*/
  /* --- Functions to access object as input/output buffer ---------*/
  0,                                                /* tp_as_buffer */
  /* --- Flags to define presence of optional/expanded features */
  Py_TPFLAGS_HAVE_CLASS,                            /*tp_flags */
  "About Material",                                 /*tp_doc */
  0,                                                /*tp_traverse */
  0,                                                /*tp_clear */
  0,                                                /*tp_richcompare */
  0,                                                /*tp_weaklistoffset */
  0,                                                /*tp_iter */
  0,                                                /*tp_iternext */
  MaterialPy::Methods,                              /*tp_methods */
  0,                                                /*tp_members */
  0,                                                /*tp_getset */
  &Base::PyObjectBase::Type,                        /*tp_base */
  0,                                                /*tp_dict */
  0,                                                /*tp_descr_get */
  0,                                                /*tp_descr_set */
  0,                                                /*tp_dictoffset */
  0,                                                /*tp_init */
  0,                                                /*tp_alloc */
  0,                                                /*tp_new */
  0,                                                /*tp_free   Low-level free-memory routine */
  0,                                                /*tp_is_gc  For PyObject_IS_GC */
  0,                                                /*tp_bases */
  0,                                                /*tp_mro    method resolution order */
  0,                                                /*tp_cache */
  0,                                                /*tp_subclasses */
  0                                                 /*tp_weaklist */
};

PyDoc_STRVAR(Material_set_doc,
"Set(string) -- Set the material.\n"
"\n"
"The material must be one of the following values:\n"
"Brass, Bronze, Copper, Gold, Pewter, Plaster, Plastic, Silver, Steel, Stone, Shiny plastic,\n"
"Satin, Metalized, Neon GNC, Chrome, Aluminium, Obsidian, Neon PHC, Jade, Ruby or Emerald.\n");

//--------------------------------------------------------------------------
// Methods structure
//--------------------------------------------------------------------------
PyMethodDef App::MaterialPy::Methods[] = {
  {"set", (PyCFunction) sset, 1, Material_set_doc},
	{NULL, NULL}		/* Sentinel */
};

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject App::MaterialPy::Parents[] = {&PyObjectBase::Type, NULL};

//--------------------------------------------------------------------------
//t constructor
//--------------------------------------------------------------------------
App::MaterialPy::MaterialPy(Material *pcMaterial, PyTypeObject *T)
: PyObjectBase(0, T), _pcMaterial(pcMaterial)
{
	Base::Console().Log("Create MaterialPy: %p \n",this);
}

PyObject *MaterialPy::PyMake(PyObject *ignored, PyObject *args)	// Python wrapper
{
  //return new MaterialPy(name, n, tau, gamma);			// Make new Python-able object
	return 0;
}

//--------------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------------
MaterialPy::~MaterialPy()						// Everything handled in parent
{
	Base::Console().Log("Destroy MaterialPy: %p \n",this);
}


//--------------------------------------------------------------------------
// MaterialPy representation
//--------------------------------------------------------------------------
PyObject *MaterialPy::_repr(void)
{
  std::stringstream a;
  a << "Material: [ ";
/*  for(std::map<std::string,int>::const_iterator It = _pcMaterial->_PropertiesMap.begin();It!=_pcMaterial->_PropertiesMap.end();It++)
  {
    a << It->first << "=" << _pcMaterial->GetProperty(It->first.c_str()).GetAsString() << "; ";
  }*/
  a << "]" << std::endl;
	return Py_BuildValue("s", a.str().c_str());
}
//--------------------------------------------------------------------------
// MaterialPy Attributes
//--------------------------------------------------------------------------
PyObject *MaterialPy::_getattr(char *attr)				// __getattr__ function: note only need to handle new state
{
    if (Base::streq(attr, "__dict__")) {
      PyObject *dict = PyDict_New();
      if (dict) {
        PyDict_SetItem(dict, PyString_FromString("ambientColor"),  PyString_FromString(""));
        PyDict_SetItem(dict, PyString_FromString("diffuseColor"),  PyString_FromString(""));
        PyDict_SetItem(dict, PyString_FromString("specularColor"), PyString_FromString(""));
        PyDict_SetItem(dict, PyString_FromString("shininess"),     PyString_FromString(""));
        PyDict_SetItem(dict, PyString_FromString("transparency"),  PyString_FromString(""));
        if (PyErr_Occurred()) { Py_DECREF(dict);dict = NULL;}
      }
      return dict;
    }else if (Base::streq(attr, "ambientColor"))
    {
      if(_pcMaterial->ambientColor.a == 0.0)
			  return Py_BuildValue("(fff)",_pcMaterial->ambientColor.r,_pcMaterial->ambientColor.g,_pcMaterial->ambientColor.b);
      else
			  return Py_BuildValue("(ffff)",_pcMaterial->ambientColor.r,_pcMaterial->ambientColor.g,_pcMaterial->ambientColor.b,_pcMaterial->ambientColor.a);
    }else	if (Base::streq(attr, "diffuseColor"))
    {
      if(_pcMaterial->diffuseColor.a == 0.0)
			  return Py_BuildValue("(fff)",_pcMaterial->diffuseColor.r,_pcMaterial->diffuseColor.g,_pcMaterial->diffuseColor.b); 
      else
			  return Py_BuildValue("(ffff)",_pcMaterial->diffuseColor.r,_pcMaterial->diffuseColor.g,_pcMaterial->diffuseColor.b,_pcMaterial->diffuseColor.a); 
    }else	if (Base::streq(attr, "specularColor"))
    {
      if(_pcMaterial->specularColor.a == 0.0)			
			  return Py_BuildValue("(fff)",_pcMaterial->specularColor.r,_pcMaterial->specularColor.g,_pcMaterial->specularColor.b); 
      else
			  return Py_BuildValue("(ffff)",_pcMaterial->specularColor.r,_pcMaterial->specularColor.g,_pcMaterial->specularColor.b,_pcMaterial->specularColor.a); 
    }else	if (Base::streq(attr, "shininess"))
    {
			return Py_BuildValue("f",_pcMaterial->shininess); 
    }else	if (Base::streq(attr, "transparency"))
    {
			return Py_BuildValue("f",_pcMaterial->transparency); 
    }else

	    _getattr_up(PyObjectBase); 						
} 


Color MaterialPy::getColorFromPy(PyObject *value)
{
  Color c;

  if(PyTuple_Check(value))
  {
    int size = PyTuple_Size( value);

    if( size != 4 && size != 3)
      throw Base::Exception("wrong Tuple size. Only Tuple with 3 (r,g,b) or 4 (r,g,b,a) floats are allowed...");

    c.r = getFloatFromPy(PyTuple_GetItem( value, 0));
    c.g = getFloatFromPy(PyTuple_GetItem( value, 1));
    c.b = getFloatFromPy(PyTuple_GetItem( value, 2));
    if(size > 3)
      c.a = getFloatFromPy(PyTuple_GetItem( value, 3));

  }else
    throw Base::Exception("wrong type for color. Only Tuple with 3 (r,g,b) or 4 (r,g,b,a) floats are allowed...");


  return c;

}

int MaterialPy::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
{ 
	if (Base::streq(attr, "ambientColor")){						// settable new state
		_pcMaterial->ambientColor = getColorFromPy(value); 
		return 0;
  }else if (Base::streq(attr, "diffuseColor")){						// settable new state
		_pcMaterial->diffuseColor = getColorFromPy(value); 
		return 0;
  }else if (Base::streq(attr, "specularColor")){						// settable new state
		_pcMaterial->specularColor = getColorFromPy(value); 
		return 0;
  }else if (Base::streq(attr, "shininess")){						// settable new state
		_pcMaterial->shininess = getFloatFromPy(value); 
		return 0;
  }else if (Base::streq(attr, "transparency")){						// settable new state
		_pcMaterial->transparency = getFloatFromPy(value); 
		return 0;
  }else

	  return PyObjectBase::_setattr(attr, value); 						
} 


//--------------------------------------------------------------------------
// Python wrappers
//--------------------------------------------------------------------------

PYFUNCIMP_D(MaterialPy,set)
{
	char *pstr;
  if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
    return NULL;                             // NULL triggers exception 

  PY_TRY{
    _pcMaterial->set(pstr);
	}PY_CATCH;

	Py_Return;
}


