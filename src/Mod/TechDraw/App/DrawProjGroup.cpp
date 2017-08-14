/***************************************************************************
 *   Copyright (c) 2013-2014 Luke Parry <l.parry@warwick.ac.uk>            *
 *   Copyright (c) 2014 Joe Dowsett <dowsettjoe[at]yahoo[dot]co[dot]uk>    *
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
#include <QRectF>
#include <cmath>
#endif

#include <App/Document.h>
#include <App/DocumentObject.h>

#include <Base/BoundBox.h>
#include <Base/Console.h>
#include <Base/Exception.h>

#include <Base/Matrix.h>

#include "Cube.h"
#include "DrawUtil.h"
#include "DrawPage.h"
#include "DrawProjGroupItem.h"
#include "DrawProjGroup.h"

#include <Mod/TechDraw/App/DrawProjGroupPy.h>  // generated from DrawProjGroupPy.xml

using namespace TechDraw;

const char* DrawProjGroup::ProjectionTypeEnums[] = {"Default",
                                                              "First Angle",
                                                              "Third Angle",
                                                              NULL};



PROPERTY_SOURCE(TechDraw::DrawProjGroup, TechDraw::DrawViewCollection)

DrawProjGroup::DrawProjGroup(void)
{
    static const char *group = "Base";
    static const char *agroup = "Distribute";


    ADD_PROPERTY_TYPE(Source    ,(0), group, App::Prop_None,"Shape to view");
    ADD_PROPERTY_TYPE(Anchor, (0), group, App::Prop_None, "The root view to align projections with");
    ProjectionType.setEnums(ProjectionTypeEnums);
    ADD_PROPERTY(ProjectionType, ((long)0));

    ADD_PROPERTY_TYPE(AutoDistribute ,(true),agroup,App::Prop_None,"Distribute Views Automatically or Manually");
    ADD_PROPERTY_TYPE(spacingX, (15), agroup, App::Prop_None, "Horizontal spacing between views");
    ADD_PROPERTY_TYPE(spacingY, (15), agroup, App::Prop_None, "Vertical spacing between views");

    ADD_PROPERTY_TYPE(CubeDirs,(Base::Vector3d()) ,agroup, App::Prop_None, "Current view directions");
    ADD_PROPERTY_TYPE(CubeRotations,(Base::Vector3d()),agroup, App::Prop_None, "Current rotations");
    m_cube = new Cube();
    m_cube->initialize();       //set default dirs & rots
}

DrawProjGroup::~DrawProjGroup()
{
    delete m_cube;
}

void DrawProjGroup::resetCube(void)
{
    m_cube->initialize();
    updateSecondaryDirs();
}

void DrawProjGroup::onChanged(const App::Property* prop)
{
    //TODO: For some reason, when the projection type is changed, the isometric views show change appropriately, but the orthographic ones don't... Or vice-versa.  WF: why would you change from 1st to 3rd in mid drawing?
    //if group hasn't been added to page yet, can't scale or distribute projItems
    TechDraw::DrawPage *page = getPage();
    if (!isRestoring() && page) {
        if (prop == &Source) {
            App::DocumentObject* sourceObj = Source.getValue();
            if (sourceObj != nullptr) {
                if (!hasAnchor()) {
                    // if we have a Source, but no Anchor, make an anchor
                    Anchor.setValue(addProjection("Front"));
                    Anchor.purgeTouched();                      //don't need to mark this
                }
            } else {
                //Source has been changed to null! Why? What to do?
            }
        } else if (prop == &AutoDistribute  &&
            AutoDistribute.getValue()) {
            distributeProjections();

        }
    }
    if (isRestoring() && (prop == &CubeDirs)) {
        m_cube->setAllDirs(CubeDirs.getValues());                    //override defaults from saved value if valid
    }
    if (isRestoring() && (prop == &CubeRotations)) {
        m_cube->setAllRots(CubeRotations.getValues());               //override defaults from saved value if valid
    }
    
    TechDraw::DrawViewCollection::onChanged(prop);
}

//! call this after changing the Cube
void DrawProjGroup::setPropsFromCube(void)
{
    CubeDirs.setValues(m_cube->getAllDirs());
    CubeRotations.setValues(m_cube->getAllRots());
}

void DrawProjGroup::setCubeFromProps(void)
{
    m_cube->setAllDirs(CubeDirs.getValues());
    m_cube->setAllRots(CubeRotations.getValues());
}
App::DocumentObjectExecReturn *DrawProjGroup::execute(void)
{
    if (!keepUpdated()) {
        return App::DocumentObject::StdReturn;
    }

    //if group hasn't been added to page yet, can't scale or distribute projItems
    TechDraw::DrawPage *page = getPage();
    if (!page) {
        return DrawViewCollection::execute();
    }

    App::DocumentObject* docObj = Source.getValue();
    if (docObj == nullptr) {
        return DrawViewCollection::execute();
    }

    docObj = Anchor.getValue();
    if (docObj == nullptr) {
        return DrawViewCollection::execute();
    }

    double newScale = getScale();
    if (ScaleType.isValue("Automatic")) {
        //Recalculate scale if Group is too big or too small!
        if (!checkFit(page)) {
            newScale = calculateAutomaticScale();
            if(std::abs(getScale() - newScale) > FLT_EPSILON) {
                Scale.setValue(newScale);
                updateChildren(newScale);
            }
         }
    } else if (ScaleType.isValue("Page")) {                 //don't really need this any more.
        newScale = page->Scale.getValue();
        if(std::abs(getScale() - newScale) > FLT_EPSILON) {
            Scale.setValue(newScale);
            updateChildren(newScale);
        }
    } else if (ScaleType.isValue("Custom")) {
        //don't have to do anything special
        updateChildren(newScale);
    }

    distributeProjections();

    if (page != nullptr) {
        page->requestPaint();
    }

    return DrawViewCollection::execute();
}

short DrawProjGroup::mustExecute() const
{
    short result = 0;
    if (!isRestoring()) {
        result = Views.isTouched() ||
                 Source.isTouched() ||
                 Scale.isTouched()  ||
                 ScaleType.isTouched() ||
                 ProjectionType.isTouched() ||
                 Anchor.isTouched();
    }
    if (result) return result;
    return TechDraw::DrawViewCollection::mustExecute();
}

Base::BoundBox3d DrawProjGroup::getBoundingBox() const
{
    Base::BoundBox3d bbox;

    std::vector<App::DocumentObject*> views = Views.getValues();
    TechDraw::DrawProjGroupItem *anchorView = dynamic_cast<TechDraw::DrawProjGroupItem *>(Anchor.getValue());
    for (std::vector<App::DocumentObject*>::const_iterator it = views.begin(); it != views.end(); ++it) {
         if ((*it)->getTypeId().isDerivedFrom(DrawViewPart::getClassTypeId())) {
            DrawViewPart *part = static_cast<DrawViewPart *>(*it);
            Base::BoundBox3d  bb = part->getBoundingBox();

            bb.ScaleX(1. / part->getScale());
            bb.ScaleY(1. / part->getScale());

            // X and Y of dependent views are relative to the anchorView
            if (part != anchorView) {
                bb.MoveX(part->X.getValue());
                bb.MoveY(part->Y.getValue());
            }

            bbox.Add(bb);
        }
    }
    // This /should/ leave the centre of the bounding box at (0,0) except when
    // we're in the process of updating the anchor view's position (eg called
    // by moveToCentre())
    if (anchorView) { //TODO: It looks like we might be getting called before an anchor view is set - weird...
        bbox.MoveX(anchorView->X.getValue());
        bbox.MoveY(anchorView->Y.getValue());
    }
    return bbox;
}

TechDraw::DrawPage * DrawProjGroup::getPage(void) const
{
    return findParentPage();
}

// Function provided by Joe Dowsett, 2014
double DrawProjGroup::calculateAutomaticScale() const
{
    TechDraw::DrawPage *page = getPage();
    if (page == NULL)
      throw Base::Exception("No page is assigned to this feature");

    DrawProjGroupItem *viewPtrs[10];

    arrangeViewPointers(viewPtrs);
    double width, height;
    minimumBbViews(viewPtrs, width, height);                               //get 1:1 bbxs
    double bbFudge = 1.2;
    width *= bbFudge;
    height *= bbFudge;

    // C++ Standard says casting bool to int gives 0 or 1
    int numVertSpaces = (viewPtrs[0] || viewPtrs[3] || viewPtrs[7]) +
                        (viewPtrs[2] || viewPtrs[5] || viewPtrs[9]) +
                        (viewPtrs[6] != NULL);
    int numHorizSpaces = (viewPtrs[0] || viewPtrs[1] || viewPtrs[2]) +
                         (viewPtrs[7] || viewPtrs[8] || viewPtrs[9]);

    double availableX = page->getPageWidth();
    double availableY = page->getPageHeight();
    double xWhite = spacingX.getValue() * (numVertSpaces + 1);
    double yWhite = spacingY.getValue() * (numHorizSpaces + 1);
    width += xWhite;
    height += yWhite;
    double scale_x = availableX / width;
    double scale_y = availableY / height;

    double scaleFudge = 0.80;
    float working_scale = scaleFudge * std::min(scale_x, scale_y);
    double result = DrawUtil::sensibleScale(working_scale);
    return result;
}

QRectF DrawProjGroup::getRect() const         //this is current rect, not potential rect
{
    DrawProjGroupItem *viewPtrs[10];
    arrangeViewPointers(viewPtrs);
    double width, height;
    minimumBbViews(viewPtrs, width, height);                           // w,h of just the views at 1:1 scale
    double xSpace = spacingX.getValue() * 3.0 * std::max(1.0,getScale());
    double ySpace = spacingY.getValue() * 2.0 * std::max(1.0,getScale());
    double rectW = getScale() * width + xSpace;                  //scale the 1:1 w,h and add whitespace
    double rectH = getScale() * height + ySpace;
    return QRectF(0,0,rectW,rectH);
}

//find area consumed by Views only in 1:1 Scale
void DrawProjGroup::minimumBbViews(DrawProjGroupItem *viewPtrs[10],
                                            double &width, double &height) const
{
    // Get bounding boxes in object scale
    Base::BoundBox3d bboxes[10];
    makeViewBbs(viewPtrs, bboxes, false);

    //TODO: note that TLF/TRF/BLF,BRF extend a bit farther than a strict row/col arrangement would suggest.
    //get widest view in each row/column
    double col0w = std::max(std::max(bboxes[0].LengthX(), bboxes[3].LengthX()), bboxes[7].LengthX()),
           col1w = std::max(std::max(bboxes[1].LengthX(), bboxes[4].LengthX()), bboxes[8].LengthX()),
           col2w = std::max(std::max(bboxes[2].LengthX(), bboxes[5].LengthX()), bboxes[9].LengthX()),
           col3w = bboxes[6].LengthX(),
           row0h = std::max(std::max(bboxes[0].LengthY(), bboxes[1].LengthY()), bboxes[2].LengthY()),
           row1h = std::max(std::max(bboxes[3].LengthY(), bboxes[4].LengthY()),
                            std::max(bboxes[5].LengthY(), bboxes[6].LengthY())),
           row2h = std::max(std::max(bboxes[7].LengthY(), bboxes[8].LengthY()), bboxes[9].LengthY());

    width = col0w + col1w + col2w + col3w;
    height = row0h + row1h + row2h;
}


void DrawProjGroup::moveToCentre(void)
{
    // Update the anchor view's X and Y to keep the bounding box centred on the origin
    Base::BoundBox3d tempbbox = getBoundingBox();
    DrawProjGroupItem *anchorView = dynamic_cast<DrawProjGroupItem *>(Anchor.getValue());
    if (anchorView) {
        anchorView->X.setValue((tempbbox.MinX + tempbbox.MaxX) / -2.0);
        anchorView->Y.setValue((tempbbox.MinY + tempbbox.MaxY) / -2.0);
    }
}

App::DocumentObject * DrawProjGroup::getProjObj(const char *viewProjType) const
{
    for( auto it : Views.getValues() ) {
        auto projPtr( dynamic_cast<DrawProjGroupItem *>(it) );
        if( projPtr &&
            strcmp(viewProjType, projPtr->Type.getValueAsString()) == 0 ) {
            return it;
        }
    }

    return 0;
}

DrawProjGroupItem* DrawProjGroup::getProjItem(const char *viewProjType) const
{
    App::DocumentObject* docObj = getProjObj(viewProjType);
    DrawProjGroupItem* result = static_cast<DrawProjGroupItem*>(docObj);
    return result;
}


bool DrawProjGroup::checkViewProjType(const char *in)
{
    if ( strcmp(in, "Front") == 0 ||
         strcmp(in, "Left") == 0 ||
         strcmp(in, "Right") == 0 ||
         strcmp(in, "Top") == 0 ||
         strcmp(in, "Bottom") == 0 ||
         strcmp(in, "Rear") == 0 ||
         strcmp(in, "FrontTopLeft") == 0 ||
         strcmp(in, "FrontTopRight") == 0 ||
         strcmp(in, "FrontBottomLeft") == 0 ||
         strcmp(in, "FrontBottomRight") == 0) {
        return true;
    }
    return false;
}

//********************************
// ProjectionItem A/D/I
//********************************
bool DrawProjGroup::hasProjection(const char *viewProjType) const
{
    for( const auto it : Views.getValues() ) {
        auto view( dynamic_cast<TechDraw::DrawProjGroupItem *>(it) );
        if( view && strcmp(viewProjType, view->Type.getValueAsString()) == 0 ) {
            return true;
        }
    }
    return false;
}

App::DocumentObject * DrawProjGroup::addProjection(const char *viewProjType)
{
    DrawProjGroupItem *view( nullptr );

    if ( checkViewProjType(viewProjType) && !hasProjection(viewProjType) ) {
        std::string FeatName = getDocument()->getUniqueObjectName("ProjItem");
        auto docObj( getDocument()->addObject( "TechDraw::DrawProjGroupItem",     //add to Document
                                               FeatName.c_str() ) );
        view = static_cast<TechDraw::DrawProjGroupItem *>( docObj );
        view->Source.setValue( Source.getValue() );
        if (ScaleType.isValue("Automatic")) {
            view->ScaleType.setValue("Custom");
        } else {
            view->ScaleType.setValue( ScaleType.getValue() );
        }
        view->Scale.setValue( getScale() );
        view->Type.setValue( viewProjType );
        view->Label.setValue( viewProjType );
        view->Source.setValue( Source.getValue() );
        view->Direction.setValue(m_cube->getViewDir(viewProjType));
        view->RotationVector.setValue(m_cube->getRotationDir(viewProjType));
        addView(view);         //from DrawViewCollection
        moveToCentre();
        if (view != getAnchor()) {                //anchor is done elsewhere
            view->recomputeFeature();
        }
        distributeProjections();
    }

    return view;
}

//NOTE: projections can be deleted without using removeProjection - ie regular DocObject deletion process.
int DrawProjGroup::removeProjection(const char *viewProjType)
{
    // TODO: shouldn't be able to delete "Front" unless deleting whole group
    if ( checkViewProjType(viewProjType) ) {
        if( !hasProjection(viewProjType) ) {
            throw Base::Exception("The projection doesn't exist in the group");
        }

        // Iterate through the child views and find the projection type
        for( auto it : Views.getValues() ) {
            auto projPtr( dynamic_cast<TechDraw::DrawProjGroupItem *>(it) );
            if( projPtr ) {
                if ( strcmp(viewProjType, projPtr->Type.getValueAsString()) == 0 ) {
                    removeView(projPtr);                                        // Remove from collection
                    getDocument()->remObject( it->getNameInDocument() );        // Remove from the document
                    moveToCentre();
                    return Views.getValues().size();
                }
            }
        }
        distributeProjections();
    }

    return -1;
}

//removes all DPGI - used when deleting DPG
int DrawProjGroup::purgeProjections()
{
    while (!Views.getValues().empty())   {
        std::vector<DocumentObject*> views = Views.getValues();
        DrawProjGroupItem* dpgi;
        DocumentObject* dObj =  views.back();
        dpgi = dynamic_cast<DrawProjGroupItem*>(dObj);
        if (dpgi) {
            std::string itemName = dpgi->Type.getValueAsString();
            removeProjection(itemName.c_str());
        }
    }
    auto page = findParentPage();
    if (page != nullptr) {
        page->requestPaint();
    }

    return Views.getValues().size();
}

void DrawProjGroup::arrangeViewPointers(DrawProjGroupItem *viewPtrs[10]) const
{
    for (int i=0; i<10; ++i) {
        viewPtrs[i] = NULL;
    }

    // Determine layout - should be either "First Angle" or "Third Angle"
    const char* projType;
    if (ProjectionType.isValue("Default")) {
        projType = findParentPage()->ProjectionType.getValueAsString();
    } else {
        projType = ProjectionType.getValueAsString();
    }

    // Iterate through views and populate viewPtrs
    if ( strcmp(projType, "Third Angle") == 0 ||
         strcmp(projType, "First Angle") == 0    ) {
        //   Third Angle:  FTL  T  FTRight          0  1  2
        //                  L   F   Right   Rear    3  4  5  6
        //                 FBL  B  FBRight          7  8  9
        //
        //   First Angle:  FBRight  B  FBL          0  1  2
        //                  Right   F   L  Rear     3  4  5  6
        //                 FTRight  T  FTL          7  8  9

        bool thirdAngle = (strcmp(projType, "Third Angle") == 0);
        for (auto it : Views.getValues()) {
            auto oView( dynamic_cast<DrawProjGroupItem *>(it) );
            if (oView) {
                const char *viewTypeCStr = oView->Type.getValueAsString();
                if (strcmp(viewTypeCStr, "Front") == 0) {
                  //viewPtrs[thirdAngle ? 4 : 4] = oView;
                    viewPtrs[4] = oView;
                } else if (strcmp(viewTypeCStr, "Left") == 0) {
                    viewPtrs[thirdAngle ? 3 : 5] = oView;
                } else if (strcmp(viewTypeCStr, "Right") == 0) {
                    viewPtrs[thirdAngle ? 5 : 3] = oView;
                } else if (strcmp(viewTypeCStr, "Top") == 0) {
                    viewPtrs[thirdAngle ? 1 : 8] = oView;
                } else if (strcmp(viewTypeCStr, "Bottom") == 0) {
                    viewPtrs[thirdAngle ? 8 : 1] = oView;
                } else if (strcmp(viewTypeCStr, "Rear") == 0) {
                    viewPtrs[6] = oView;
                } else if (strcmp(viewTypeCStr, "FrontTopLeft") == 0) {
                    viewPtrs[thirdAngle ? 0 : 9] = oView;
                } else if (strcmp(viewTypeCStr, "FrontTopRight") == 0) {
                    viewPtrs[thirdAngle ? 2 : 7] = oView;
                } else if (strcmp(viewTypeCStr, "FrontBottomLeft") == 0) {
                    viewPtrs[thirdAngle ? 7 : 2] = oView;
                } else if (strcmp(viewTypeCStr, "FrontBottomRight") == 0) {
                    viewPtrs[thirdAngle ? 9 : 0] = oView;
                } else {
                    throw Base::Exception("Unknown view type in DrawProjGroup::arrangeViewPointers()");
                }
            }
        }
    } else {
        throw Base::Exception("Unknown view type in DrawProjGroup::arrangeViewPointers()");
    }
}

void DrawProjGroup::makeViewBbs(DrawProjGroupItem *viewPtrs[10],
                                          Base::BoundBox3d bboxes[10],
                                          bool documentScale) const
{
    for (int i = 0; i < 10; ++i)
        if (viewPtrs[i]) {
            bboxes[i] = viewPtrs[i]->getBoundingBox();
            if (!documentScale) {
                double scale = 1.0 / viewPtrs[i]->getScale();    //convert bbx to 1:1 scale
                bboxes[i].ScaleX(scale);
                bboxes[i].ScaleY(scale);
                bboxes[i].ScaleZ(scale);
            }
        } else {
            // BoundBox3d defaults to length=(FLOAT_MAX + -FLOAT_MAX)
            bboxes[i].ScaleX(0);
            bboxes[i].ScaleY(0);
            bboxes[i].ScaleZ(0);
        }
}

bool DrawProjGroup::distributeProjections()
{
    //TODO: bounding boxes do not take view orientation into account
    //      ie X&Y widths might be swapped on page
    if (!AutoDistribute.getValue()) {
        return true;
    }
    DrawProjGroupItem *viewPtrs[10];
    arrangeViewPointers(viewPtrs);

    // TODO: Work on not requiring the front view...
    //WF: there is always a "Front" view. That's in the nature of the ProjGroup!
    if (!viewPtrs[4]) {
        return false;
    }

    // Calculate bounding boxes for each displayed view
    Base::BoundBox3d bboxes[10];
    makeViewBbs(viewPtrs, bboxes);

    double xSpacing = spacingX.getValue();    //in mm/scale
    double ySpacing = spacingY.getValue();    //in mm/scale

    double bigRow    = 0.0;
    double bigCol    = 0.0;
    for (auto& b: bboxes) {
        if (!b.IsValid()) {
            continue;
        }
        if (b.LengthX() > bigCol) {
            bigCol = b.LengthX();
        }
        if (b.LengthY() > bigRow) {
            bigRow = b.LengthY();
        }
    }

    //if we have iso's, make sure they fit the grid.
    if (viewPtrs[0] || viewPtrs[2]  || viewPtrs[7] ||  viewPtrs[9]) {
        bigCol = std::max(bigCol,bigRow);
        bigRow = bigCol;
    }

        //viewPtrs
        // 0  1  2
        // 3  4  5  6
        // 7  8  9

    if (viewPtrs[0] && viewPtrs[0]->allowAutoPos() &&
        bboxes[0].IsValid()) {
        viewPtrs[0]->X.setValue(-bigCol - xSpacing);
        viewPtrs[0]->Y.setValue(bigRow + ySpacing);
    }
    if (viewPtrs[1] && viewPtrs[1]->allowAutoPos() &&
        bboxes[1].IsValid()) {
        viewPtrs[1]->Y.setValue(bigRow + ySpacing);
    }
    if (viewPtrs[2] && viewPtrs[2]->allowAutoPos()
        ) {
        viewPtrs[2]->X.setValue(bigCol + xSpacing);
        viewPtrs[2]->Y.setValue(bigRow + ySpacing);
    }
    if (viewPtrs[3] && viewPtrs[3]->allowAutoPos() &&
        bboxes[3].IsValid() &&
        bboxes[4].IsValid()) {
        viewPtrs[3]->X.setValue(-bigCol - xSpacing);
    }
    if (viewPtrs[4]) {  // TODO: Move this check above, and figure out a sane bounding box based on other existing views
    }

    if (viewPtrs[5] && viewPtrs[5]->allowAutoPos() &&
        bboxes[5].IsValid() &&
        bboxes[4].IsValid()) {
        viewPtrs[5]->X.setValue(bigCol + xSpacing);
    }
    if (viewPtrs[6] && viewPtrs[6]->allowAutoPos() &&
        bboxes[6].IsValid()) {    //"Rear"
        if (viewPtrs[5] &&
            bboxes[5].IsValid()) {
            viewPtrs[6]->X.setValue(viewPtrs[5]->X.getValue() + bigCol + xSpacing);
        }else if (viewPtrs[4] &&
            bboxes[4].IsValid()) {
            viewPtrs[6]->X.setValue(bigCol + xSpacing);
        }
    }
    if (viewPtrs[7] && viewPtrs[7]->allowAutoPos() &&
        bboxes[7].IsValid()) {
        viewPtrs[7]->X.setValue(-bigCol - xSpacing);
        viewPtrs[7]->Y.setValue(-bigRow - ySpacing);
    }
    if (viewPtrs[8] && viewPtrs[8]->allowAutoPos() &&
        bboxes[8].IsValid() &&
        bboxes[4].IsValid()) {
        viewPtrs[8]->Y.setValue(-bigRow - ySpacing);
    }
    if (viewPtrs[9] && viewPtrs[9]->allowAutoPos() &&
        bboxes[9].IsValid()) {
        viewPtrs[9]->X.setValue(bigCol + xSpacing);
        viewPtrs[9]->Y.setValue(-bigRow - ySpacing);
    }
    return true;
}

//! tell children DPGIs that parent DPG has changed ?Scale?
void DrawProjGroup::updateChildren(double scale)
{
    for( const auto it : Views.getValues() ) {
        auto view( dynamic_cast<DrawProjGroupItem *>(it) );
        if( view ) {
            if(std::abs(view->getScale() - scale) > FLT_EPSILON) {
                view->Scale.setValue(scale);
                view->recomputeFeature();
            }
        }
    }
}

//!check if ProjectionGroup fits on Page
bool DrawProjGroup::checkFit(TechDraw::DrawPage* p) const
{
    bool result = true;

    QRectF viewBox = getRect();
    double fudge = 1.1;
    double maxWidth = viewBox.width() * fudge;
    double maxHeight = viewBox.height() * fudge;

    if ( (maxWidth > p->getPageWidth()) ||
         (maxHeight > p->getPageHeight()) ) {
        result = false;
    }

    if (ScaleType.isValue("Automatic")) {                        //expand if too small
        double magnifyLimit = 0.60;
        if ( (maxWidth < p->getPageWidth() * magnifyLimit) &&
             (maxHeight < p->getPageHeight() * magnifyLimit) ) {
            result = false;
        }
    }
    return result;
}


App::Enumeration DrawProjGroup::usedProjectionType(void)
{
    //TODO: Would've been nice to have an Enumeration(const PropertyEnumeration &) constructor
    App::Enumeration ret(ProjectionTypeEnums, ProjectionType.getValueAsString());
    if (ret.isValue("Default")) {
        TechDraw::DrawPage * page = getPage();
        if ( page != NULL ) {
            ret.setValue(page->ProjectionType.getValueAsString());
        }
    }
    return ret;
}

bool DrawProjGroup::hasAnchor(void)
{
    bool result = false;
    App::DocumentObject* docObj = Anchor.getValue();
    if (docObj != nullptr) {
        result = true;
    }
    return result;
}

TechDraw::DrawProjGroupItem* DrawProjGroup::getAnchor(void)
{
    DrawProjGroupItem* result = nullptr;
    App::DocumentObject* docObj = Anchor.getValue();
    if (docObj != nullptr) {
        result = static_cast<DrawProjGroupItem*>(docObj);
    }
    return result;
}


void DrawProjGroup::setAnchorDirection(const Base::Vector3d dir)
{
    App::DocumentObject* docObj = Anchor.getValue();
    DrawProjGroupItem* item = static_cast<DrawProjGroupItem*>(docObj);
    item->Direction.setValue(dir);
}

Base::Vector3d DrawProjGroup::getAnchorDirection(void)
{
    Base::Vector3d result;
    App::DocumentObject* docObj = Anchor.getValue();
    if (docObj != nullptr) {
        DrawProjGroupItem* item = static_cast<DrawProjGroupItem*>(docObj);
        result = item->Direction.getValue();
    } else {
        Base::Console().Log("ERROR - DPG::getAnchorDir - no Anchor!!\n");
    }
    return result;
}


//*************************************
//* view direction manipulation routines
//*************************************

void DrawProjGroup::updateSecondaryDirs()
{
    for (auto& docObj: Views.getValues()) {
        Base::Vector3d newDir;
        Base::Vector3d newAxis;
        std::string pic;
        DrawProjGroupItem* v = static_cast<DrawProjGroupItem*>(docObj);
        ProjItemType t = static_cast<ProjItemType>(v->Type.getValue());
        switch (t) {
            case Front : {
                newDir = m_cube->getFront();
                newAxis = m_cube->getFrontRot();
                break;
            }
            case Rear : {
                newDir = m_cube->getRear();
                newAxis = m_cube->getRearRot();
                break;
            }
            case Left : {
                newDir = m_cube->getLeft();
                newAxis = m_cube->getLeftRot();
                break;
            }
            case Right : {
                newDir = m_cube->getRight();
                newAxis = m_cube->getRightRot();
                break;
            }
            case Top : {
                newDir = m_cube->getTop();
                newAxis = m_cube->getTopRot();
                break;
            }
            case Bottom : {
                newDir = m_cube->getBottom();
                newAxis = m_cube->getBottomRot();
                break;
            }
            case FrontTopLeft : {
                newDir = m_cube->getFTL();
                newAxis = m_cube->getFTLRot();
                break;
            }
            case FrontTopRight : {
                newDir = m_cube->getFTR();
                newAxis = m_cube->getFTRRot();
                break;
            }
            case FrontBottomLeft : {
                newDir = m_cube->getFBL();
                newAxis = m_cube->getFBLRot();
                break;
            }
            case FrontBottomRight : {
                newDir = m_cube->getFBR();
                newAxis = m_cube->getFBRRot();
                break;
            }
            default: {
                //TARFU invalid secondary type
                Base::Console().Message("ERROR - DPG::updateSecondaryDirs - invalid projection type\n");
                newDir = v->Direction.getValue();
                newAxis = v->RotationVector.getValue();
            }
        }
        v->Direction.setValue(newDir);
        v->RotationVector.setValue(newAxis);
    }
    setPropsFromCube();

    auto page = findParentPage();
    if (page != nullptr) {
        page->requestPaint();
    }
}


void DrawProjGroup::rotateRight()
{
//Front -> Right -> Rear -> Left -> Front
    m_cube->rotateRight(); 
    updateSecondaryDirs();
}

void DrawProjGroup::rotateLeft()
{
//Front -> Left -> Rear -> Right -> Front
    m_cube->rotateLeft(); 
    updateSecondaryDirs();
}

void DrawProjGroup::rotateUp()
{
//Front -> Top -> Rear -> Bottom -> Front
    m_cube->rotateUp(); 
    updateSecondaryDirs();
}
    
void DrawProjGroup::rotateDown()
{
//Front -> Bottom -> Rear -> Top -> Front
    m_cube->rotateDown(); 
    updateSecondaryDirs();
}

void DrawProjGroup::spinCW()
{
//Top -> Right -> Bottom -> Left -> Top
    m_cube->spinCW(); 
    updateSecondaryDirs();
}
    
void DrawProjGroup::spinCCW()
{
//Top -> Left -> Bottom -> Right -> Top
    m_cube->spinCCW(); 
    updateSecondaryDirs();
}


//dumps the current iso DPGI's 
void DrawProjGroup::dumpISO(char * title)
{
    Base::Console().Message("DPG ISO: %s\n", title); 
    for (auto& docObj: Views.getValues()) {
        Base::Vector3d dir;
        Base::Vector3d axis;
        DrawProjGroupItem* v = static_cast<DrawProjGroupItem*>(docObj);
        std::string t = v->Type.getValueAsString();
        dir = v->Direction.getValue();
        axis = v->RotationVector.getValue();

        Base::Console().Message("%s:  %s/%s\n", 
                                t.c_str(),DrawUtil::formatVector(dir).c_str(),DrawUtil::formatVector(axis).c_str());
    }
}

PyObject *DrawProjGroup::getPyObject(void)
{
    if (PythonObject.is(Py::_None())) {
        // ref counter is set to 1
        PythonObject = Py::Object(new DrawProjGroupPy(this),true);
    }
    return Py::new_reference_to(PythonObject);
}
