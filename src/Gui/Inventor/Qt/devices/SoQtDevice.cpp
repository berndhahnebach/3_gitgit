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

// Most of the class documentation is found within the file
// common/devices/SoGuiDevice.cpp.in.

// *************************************************************************


// includes the FreeCAD configuration
#include "../../../../Config.h"

#include <Inventor/Qt/devices/SoQtDevice.h>

#include <soqtdefs.h>
#include <Inventor/Qt/devices/SoQtDeviceP.h>

// *************************************************************************

/*!
  \typedef void SoQtEventHandler(QWidget *, void *, QEvent *, bool *)
  FIXME: write doc
*/

// *************************************************************************

SoQtDevice::SoQtDevice(void)
{
  PRIVATE(this) = new SoQtDeviceP(this);
}

SoQtDevice::~SoQtDevice()
{
  delete PRIVATE(this);
}

// *************************************************************************

void
SoQtDevice::invokeHandlers(QEvent * event)
{
  PRIVATE(this)->invokeHandlers(SoQtDeviceP::invokeHandlerCB, event);
}

// *************************************************************************

#ifndef DOXYGEN_SKIP_THIS

// Code and data for the internal, private hidden implementation
// class.

SbBool
SoQtDeviceP::invokeHandlerCB(SoQtEventHandler * handler,
                             QWidget * widget,
                             QEvent * event,
                             void * handlerclosure)
{
  bool dispatch = false;
  handler(widget, handlerclosure, event, &dispatch);
  return TRUE;
}

#endif // DOXYGEN_SKIP_THIS

// *************************************************************************
