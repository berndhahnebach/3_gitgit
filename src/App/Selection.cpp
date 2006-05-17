/***************************************************************************
 *   Copyright (c) Juergen Riegel         <juergen.riegel@web.de>          *
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


/*  Precompiled header stuff
 *  on some compilers the precompiled header option gain significant compile 
 *  time! So every external header (libs and system) should included in 
 *  Precompiled.h. For systems without precompilation the header needed are
 *  included in the else fork.
 */
#include "PreCompiled.h"

#ifndef _PreComp_
# include <assert.h>
# include <string>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include "Selection.h"
#include <Base/Exception.h>


using namespace App;


//**************************************************************************
// Construction/Destruction

/**
 * A constructor.
 * A more elaborate description of the constructor.
 */
SelectionSingelton::SelectionSingelton()
{
}

/**
 * A destructor.
 * A more elaborate description of the destructor.
 */
SelectionSingelton::~SelectionSingelton()
{
}


SelectionSingelton* SelectionSingelton::_pcSingleton = NULL;

SelectionSingelton& SelectionSingelton::instance(void)
{
  if (_pcSingleton == NULL)
  {
    _pcSingleton = new SelectionSingelton;
  }

  return *_pcSingleton;
}

void SelectionSingelton::destruct (void)
{
  if (_pcSingleton != NULL)
    delete _pcSingleton;
}

void SelectionSingelton::addFeature(Feature *f)
{
  _FeatureSet.insert(f);

}

void SelectionSingelton::removeFeature(Feature *f)
{
  _FeatureSet.erase(f);


}



//**************************************************************************
// separator for other implemetation aspects



