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

 


#ifndef _FeaturePy_h_
#define _FeaturePy_h_

#ifndef _PreComp_
# include <TDF_Label.hxx>
#endif

#include <Base/PyExportImp.h>

#include "MaterialPy.h"
/*
class TFunction_Logbook;
class PyObjectBase;
class FeaturePy;
class TopoDS_Shape;
*/
namespace App
{

class Feature;

//===========================================================================
// FeaturePy - Python wrapper
//===========================================================================

/** The DocTypeStd python class
 */
class AppExport FeaturePy :public Base::PyObjectBase
{
	/// always start with Py_Header
	Py_Header;

protected:
	~FeaturePy();

public:
	FeaturePy(Feature *pcFeature, PyTypeObject *T = &Type);
	static PyObject *PyMake(PyObject *, PyObject *);

	//---------------------------------------------------------------------
	// python exports goes here +++++++++++++++++++++++++++++++++++++++++++
	//---------------------------------------------------------------------

	virtual PyObject *_repr(void);  				// the representation
	PyObject *_getattr(char *attr);					// __getattr__ function
	int _setattr(char *attr, PyObject *value);		// __setattr__ function
	PYFUNCDEF_D(FeaturePy,setModified)
	PYFUNCDEF_D(FeaturePy,setModifiedView)
	PYFUNCDEF_D(FeaturePy,isValid)


	//---------------------------------------------------------------------
	// helpers for python exports goes here +++++++++++++++++++++++++++++++
	//---------------------------------------------------------------------
  int setProperty(const char *attr, PyObject *value);

private:
  Feature *_pcFeature;

  MaterialPy* solidMaterialPy;
  MaterialPy* lineMaterialPy;
  MaterialPy* pointMaterialPy;

};



} //namespace App



#endif
