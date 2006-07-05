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


#include "PreCompiled.h"

#ifndef _PreComp_
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include <Base/Parameter.h>

#include "ViewProviderBox.h"


//#include "Tree.h"



using namespace PartGui;


//**************************************************************************
// Construction/Destruction

PROPERTY_SOURCE(PartGui::ViewProviderBox, PartGui::ViewProviderPart)

       
ViewProviderBox::ViewProviderBox()
{
  sPixmap = "PartFeatureImport";
}

ViewProviderBox::~ViewProviderBox()
{

}



// **********************************************************************************

std::list<std::string> ViewProviderBox::getModes(void) const
{
  // get the modes of the father
  list<string> StrList;

  // add your own modes
  StrList.push_back("Normal");
  StrList.push_back("Flat");
  StrList.push_back("Wireframe");
  StrList.push_back("Points");

  return StrList;
}


void ViewProviderBox::setEdit(void)
{

}

void ViewProviderBox::unsetEdit(void)
{

}

