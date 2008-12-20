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
# include <QPixmap>
# include <Inventor/nodes/SoSeparator.h>
# include <Inventor/nodes/SoSwitch.h>
# include <Inventor/nodes/SoTransform.h>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include <Base/Console.h>
#include <Base/Matrix.h>
#include <App/PropertyGeo.h>

#include "ViewProvider.h"
#include "ViewProviderPy.h"
#include "BitmapFactory.h"
#include "View3DInventor.h"
#include "SoFCDB.h"

using namespace std;
using namespace Gui;


//**************************************************************************
//**************************************************************************
// ViewProvider
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

PROPERTY_SOURCE_ABSTRACT(Gui::ViewProvider, App::PropertyContainer)

ViewProvider::ViewProvider() 
  : pcAnnotation(0), pyViewObject(0), _iActualMode(-1)
{
    pcRoot = new SoSeparator();
    pcRoot->ref();
    pcModeSwitch = new SoSwitch();
    pcModeSwitch->ref();
    pcTransform  = new SoTransform();
    pcTransform->ref();
    pcRoot->addChild(pcTransform);
    pcRoot->addChild(pcModeSwitch);
    sPixmap = "px";
    pcModeSwitch->whichChild = _iActualMode;
}

ViewProvider::~ViewProvider()
{
    if (pyViewObject) {
        pyViewObject->setInvalid();
        pyViewObject->DecRef();
    }

    pcRoot->unref();
    pcTransform->unref();
    pcModeSwitch->unref();
    if(pcAnnotation)
        pcAnnotation->unref();
}

bool ViewProvider::setEdit(int ModNum)
{
    //Gui::View3DInventorViewer* viewer = ((Gui::View3DInventor*)view)->getViewer();
    //viewer->addEventCallback(SoEvent::getClassTypeId(), Gui::ViewProvider::EventCallback, this);
	return true;
};
void ViewProvider::unsetEdit(void)
{
    //viewer->removeEventCallback(SoEvent::getClassTypeId(), Gui::ViewProvider::EventCallback, this);

};

void ViewProvider::EventCallback(void * ud, SoEventCallback * n)
{

}

SoSeparator* ViewProvider::getAnnotation(void)
{
    if (!pcAnnotation) {
        pcAnnotation = new SoSeparator();
        pcAnnotation->ref();
        pcRoot->addChild(pcAnnotation);
    }
    return pcAnnotation;
}

void ViewProvider::update(const App::Property* prop)
{
    // Hide the object temporarily to speed up the update
    bool vis = this->isShow();
    if (vis) hide();
    updateData(prop);
    if (vis) show();
}

QIcon ViewProvider::getIcon(void) const
{
    return Gui::BitmapFactory().pixmap(sPixmap);
}

void ViewProvider::setTransformation(const Base::Matrix4D &rcMatrix)
{
    double dMtrx[16];
    rcMatrix.getGLMatrix(dMtrx);

    pcTransform->setMatrix(SbMatrix(dMtrx[0], dMtrx[1], dMtrx[2],  dMtrx[3],
                                    dMtrx[4], dMtrx[5], dMtrx[6],  dMtrx[7],
                                    dMtrx[8], dMtrx[9], dMtrx[10], dMtrx[11],
                                    dMtrx[12],dMtrx[13],dMtrx[14], dMtrx[15]));
}

void ViewProvider::setTransformation(const SbMatrix &rcMatrix)
{
    pcTransform->setMatrix(rcMatrix);
}

SbMatrix ViewProvider::convert(const Base::Matrix4D &rcMatrix) const
{
    double dMtrx[16];
    rcMatrix.getGLMatrix(dMtrx);
    return SbMatrix(dMtrx[0], dMtrx[1], dMtrx[2],  dMtrx[3],
                    dMtrx[4], dMtrx[5], dMtrx[6],  dMtrx[7],
                    dMtrx[8], dMtrx[9], dMtrx[10], dMtrx[11],
                    dMtrx[12],dMtrx[13],dMtrx[14], dMtrx[15]);
}

void ViewProvider::addDisplayMaskMode( SoNode *node, const char* type )
{
    _sDisplayMaskModes[ type ] = pcModeSwitch->getNumChildren();
    pcModeSwitch->addChild( node );
}

void ViewProvider::setDisplayMaskMode( const char* type )
{
    std::map<std::string, int>::const_iterator it = _sDisplayMaskModes.find( type );
    if ( it != _sDisplayMaskModes.end() )
        pcModeSwitch->whichChild = it->second;
    else
        pcModeSwitch->whichChild = -1;
    _iActualMode = pcModeSwitch->whichChild.getValue();
}

std::vector<std::string> ViewProvider::getDisplayMaskModes() const
{
    std::vector<std::string> types;
    for (std::map<std::string, int>::const_iterator it = _sDisplayMaskModes.begin();
         it != _sDisplayMaskModes.end(); ++it)
        types.push_back( it->first );
    return types;
}

/**
 * If you add new viewing modes in @ref getModes() then you need to reimplement
 * also setMode() to handle these new modes by setting the appropriate display
 * mode.
 */
void ViewProvider::setDisplayMode(const char* ModeName)
{
    _sCurrentMode = ModeName;
}

std::string ViewProvider::getActiveDisplayMode(void) const
{
    return _sCurrentMode;
}

void ViewProvider::hide(void)
{
    pcModeSwitch->whichChild = -1;
}

void ViewProvider::show(void)
{
    pcModeSwitch->whichChild = _iActualMode;
}

bool ViewProvider::isShow(void) const
{
    return pcModeSwitch->whichChild.getValue() != -1;
}

void ViewProvider::setDefaultMode(int val)
{
    _iActualMode = val;
}

std::string ViewProvider::toString() const
{
    return SoFCDB::writeNodesToString(pcRoot);
}

PyObject* ViewProvider::getPyObject()
{
    if (!pyViewObject)
        pyViewObject = new ViewProviderPy(this);
    pyViewObject->IncRef();
    return pyViewObject;
}
