/***************************************************************************
 *   Copyright (c) 2004 Jürgen Riegel <juergen.riegel@web.de>              *
 *   Copyright (c) 2012 Luke Parry <l.parry@warwick.ac.uk>                 *
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


#ifndef DRAWINGGUI_VIEWPROVIDERCLIP_H
#define DRAWINGGUI_VIEWPROVIDERCLIP_H

#include <Gui/ViewProviderDocumentObjectGroup.h>
#include "ViewProviderDrawingView.h"

namespace TechDraw{
    class DrawViewClip;
}

namespace TechDrawGui {

class TechDrawGuiExport ViewProviderViewClip : public ViewProviderDrawingView
{
    PROPERTY_HEADER(TechDrawGui::ViewProviderViewClip);

public:
    /// constructor
    ViewProviderViewClip();
    /// destructor
    virtual ~ViewProviderViewClip();


    virtual void attach(App::DocumentObject *);
    virtual void setDisplayMode(const char* ModeName);
    virtual bool useNewSelectionModel(void) const {return false;}
    /// returns a list of all possible modes
    virtual std::vector<std::string> getDisplayModes(void) const;
    virtual void updateData(const App::Property*);
    TechDraw::DrawViewClip* getObject() const;

    /// Hide the object in the view
    virtual void hide(void);
    /// Show the object in the view
    virtual void show(void);
    virtual bool isShow(void) const;
    std::vector<App::DocumentObject*> claimChildren(void) const;

    /** @name Restoring view provider from document load */
    //@{
    virtual void startRestoring();
    virtual void finishRestoring();
    //@}
};
} // namespace TechDrawGui


#endif // DRAWINGGUI_VIEWPROVIDERCLIP_H
