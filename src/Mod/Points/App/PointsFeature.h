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



#ifndef Points_FEATURE_H
#define Points_FEATURE_H

#include <App/GeoFeature.h>
#include <App/PropertyLinks.h>
#include <App/PropertyGeo.h>
#include "Points.h"
#include "PropertyPointKernel.h"


namespace Base{
class PyObjectBase;
class Writer;
}

namespace App{
class Color;
}

namespace Points
{
class Property;
class PointsFeaturePy;

/** Base class of all Points feature classes in FreeCAD.
 * This class holds an PointsKernel object.
 */
class PointsAppExport Feature : public App::GeoFeature
{
    PROPERTY_HEADER(Points::Feature);

public:
    /// Constructor
    Feature(void);
    virtual ~Feature(void);

    App::PropertyPlacement Placement;

    /** @name methods overide Feature */
    //@{
    void Restore(Base::XMLReader &reader);
    void RestoreDocFile(Base::Reader &reader);
    /// recalculate the Feature
    virtual App::DocumentObjectExecReturn *execute(void);
    /// returns the type name of the ViewProvider
    virtual const char* getViewProviderName(void) const {
        return "PointsGui::ViewProviderPoints";
    }
    // handling the PointsPy object
    virtual PyObject* getPyObject(void);
    //@}

public:
    PropertyPointKernel Points; /**< The point kernel property. */
};

/**
 * The Export class writes a point cloud to a file.
 * @author Werner Mayer
 */
class Export : public Feature
{
    PROPERTY_HEADER(Points::Export);

public:
    Export();

    App::PropertyLinkList   Sources;
    App::PropertyString FileName;
    App::PropertyString Format;

    /** @name methods override Feature */
    //@{
    /// recalculate the Feature
    virtual App::DocumentObjectExecReturn *execute(void);
    //@}
};

class PointsAppExport Transform : public Feature
{
    PROPERTY_HEADER(Points::Transform);

public:
    /// Constructor
    Transform(void);
    virtual ~Transform(void);

    App::PropertyLink Source;
    App::PropertyMatrix Trnsfrm;

    /** @name methods overide Feature */
    //@{
    /// recalculate the Feature
    virtual App::DocumentObjectExecReturn *execute(void);
    //@}
};

} //namespace Points


#endif 
