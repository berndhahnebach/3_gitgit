/**************************************************************************\
 *
 *  This file is part of the Coin GUI toolkit libraries.
 *  Copyright (C) 1998-2002 by Systems in Motion.  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License
 *  version 2.1 as published by the Free Software Foundation.  See the
 *  file LICENSE.LGPL at the root directory of this source distribution
 *  for more details.
 *
 *  If you want to use this library with software that is incompatible
 *  licensewise with the LGPL, and / or you would like to take
 *  advantage of the additional benefits with regard to our support
 *  services, please contact Systems in Motion about acquiring a Coin
 *  Professional Edition License.  See <URL:http://www.coin3d.org> for
 *  more information.
 *
 *  Systems in Motion, Prof Brochs gate 6, 7030 Trondheim, NORWAY
 *  <URL:http://www.sim.no>, <mailto:support@sim.no>
 *
\**************************************************************************/

#ifndef SOGUIFULLVIEWERP_H
#define SOGUIFULLVIEWERP_H

#include <Inventor/SbString.h>
#include <Inventor/Qt/viewers/SoQtViewer.h>

class SoQtFullViewer;

// ************************************************************************

// This class contains private data and methods used within the
// SoGuiFullViewer class.

class SoGuiFullViewerP
{
public:
  ~SoGuiFullViewerP();

  enum SoQtFullViewerPopupMenuItem {
    // FIXME: there's code in (some of) the popupmenu widgets which
    // assumes the root id is equal to 0.  Yup, lousy design. :-/
    // Anyway, don't change it until those cases are fixed.
    // 20010810 mortene.
    ROOT_MENU = 0,
      ROOT_MENU_TITLE,

      FUNCTIONS_MENU,
      HOME_ITEM,
      SET_HOME_ITEM,
      VIEW_ALL_ITEM,
      SEEK_ITEM,

      DRAWSTYLES_MENU,
      DRAWSTYLES_STILL_MENU,
      AS_IS_ITEM,
      HIDDEN_LINE_ITEM,
      WIREFRAME_OVERLAY_ITEM,
      NO_TEXTURE_ITEM,
      LOW_RESOLUTION_ITEM,
      WIREFRAME_ITEM,
      POINTS_ITEM,
      BOUNDING_BOX_ITEM,

      DRAWSTYLES_ANIMATING_MENU,
      MOVE_SAME_AS_STILL_ITEM,
      MOVE_NO_TEXTURE_ITEM,
      MOVE_LOW_RES_ITEM,
      MOVE_WIREFRAME_ITEM,
      MOVE_LOW_RES_WIREFRAME_ITEM,
      MOVE_POINTS_ITEM,
      MOVE_LOW_RES_POINTS_ITEM,
      MOVE_BOUNDING_BOX_ITEM,

      TRANSPARENCY_TYPE_MENU,
      SCREEN_DOOR_TRANSPARENCY_ITEM,
      ADD_TRANSPARENCY_ITEM,
      DELAYED_ADD_TRANSPARENCY_ITEM,
      SORTED_OBJECT_ADD_TRANSPARENCY_ITEM,
      BLEND_TRANSPARENCY_ITEM,
      DELAYED_BLEND_TRANSPARENCY_ITEM,
      SORTED_OBJECT_BLEND_TRANSPARENCY_ITEM,
      // These two are Coin extensions.
      SORTED_OBJECT_SORTED_TRIANGLE_ADD_TRANSPARENCY_ITEM,
      SORTED_OBJECT_SORTED_TRIANGLE_BLEND_TRANSPARENCY_ITEM,

      RENDER_BUFFER_TYPE_MENU,
      SINGLE_BUFFER_ITEM,
      DOUBLE_BUFFER_ITEM,
      INTERACTIVE_BUFFER_ITEM,

      EXAMINING_ITEM,
      DECORATION_ITEM,
      HEADLIGHT_ITEM,
      FULLSCREEN_ITEM,

      STEREO_MENU,
      STEREO_OFF_ITEM,
      STEREO_ANAGLYPH_ITEM,
      STEREO_QUADBUFFER_ITEM
      };

  enum SoQtViewerPopupMenuRadioGroup {
    STILL_GROUP = 1,
      MOVE_GROUP,
      TRANSPARENCY_GROUP,
      BUFFER_GROUP,
      STEREO_GROUP
      };

  class SoQtPopupMenu * setupStandardPopupMenu(void);
  void prepareMenu(class SoQtPopupMenu * menu);
  void menuSelection(int menuitemid);
  static void menuSelectionCallback(int menuitemid, void * userdata);
  SbString popupmenutitle;

  void setDrawStyleMenuActivation(SoQtViewer::DrawType type,
                                  SoQtViewer::DrawStyle value);
  void drawstyleActivated(int menuitemid);

  static void pan(SoCamera * cam,
                  float aspectratio, const SbPlane & panplane,
                  const SbVec2f & currpos, const SbVec2f & prevpos);

protected:
  SoGuiFullViewerP(SoQtFullViewer * publ);
  SoQtFullViewer * pub;
};

// ************************************************************************

#endif // ! SOGUIFULLVIEWERP_H
