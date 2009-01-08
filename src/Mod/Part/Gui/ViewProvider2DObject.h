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


#ifndef __ViewProvider2DObject_H__
#define __ViewProvider2DObject_H__

#include "ViewProvider.h"


class TopoDS_Shape;
class TopoDS_Face;
class SoSeparator;
class SbVec3f;
class SoTransform;

namespace Gui {
  class View3DInventorViewer;
  class SoFCSelection;
}

namespace PartGui {


	class AppPartGuiExport ViewProvider2DObject: public PartGui::ViewProviderPart
{
  PROPERTY_HEADER(PartGui::ViewProvider2DObject);

public:
  /// constructor
  ViewProvider2DObject();
  /// destructor
  virtual ~ViewProvider2DObject();

  /// Property to switch the grid on and off
  App::PropertyBool ShowGrid;
  App::PropertyDistance GridSize;

  virtual void attach(App::DocumentObject *);
  virtual void updateData(const App::Property*);


  virtual void setEdit(void);
  virtual void unsetEdit(void);

  /// creats the grid
  SoSeparator* createGrid(float size=0.0, int density=0); 


protected:
  /// get called by the container whenever a property has been changed
  virtual void onChanged(const App::Property* prop);

  SoSeparator  *GridRoot;
};

} // namespace PartGui


#endif // __ViewProvider2DObject_H__

