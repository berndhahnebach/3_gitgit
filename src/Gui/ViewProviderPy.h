/***************************************************************************
 *   Copyright (c) 2006 Werner Mayer <werner.wm.mayer@gmx.de>              *
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


#ifndef GUI_VIEWPROVIDER_PY_H
#define GUI_VIEWPROVIDER_PY_H

#include <Base/PyObjectBase.h>

namespace Gui {
class ViewProvider;

class GuiExport ViewProviderPy : public Base::PyObjectBase
{
	/// always start with Py_Header
	Py_Header;

public:
  /// constructor.
  ViewProviderPy(ViewProvider *pcViewProvider, PyTypeObject *T = &Type);
	static PyObject *PyMake(PyObject *, PyObject *);
  /// destructor.
  virtual ~ViewProviderPy();
  void setInvalid();

	//---------------------------------------------------------------------
	// python exports goes here +++++++++++++++++++++++++++++++++++++++++++
	//---------------------------------------------------------------------

	virtual PyObject *_repr(void);  				// the representation
	PyObject *_getattr(char *attr);					// __getattr__ function
	int _setattr(char *attr, PyObject *value);		// __setattr__ function
	PYFUNCDEF_D(ViewProviderPy,hide)
	PYFUNCDEF_D(ViewProviderPy,show)
	PYFUNCDEF_D(ViewProviderPy,isVisible)
	PYFUNCDEF_D(ViewProviderPy,update)
	PYFUNCDEF_D(ViewProviderPy,listDisplayModes)

  //---------------------------------------------------------------------
	// helpers for python exports goes here +++++++++++++++++++++++++++++++
	//---------------------------------------------------------------------
  ViewProvider *getViewProvider(void) const {return _pcViewProvider;}

private:
  ViewProvider *_pcViewProvider;
};

} // namespace Gui

#endif // GUI_VIEWPROVIDER_PY_H

