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

// includes the FreeCAD configuration
#include "../../../Config.h"


#include "6DOFEvents.h"

SoQt6dofDevicePressureEvent::SoQt6dofDevicePressureEvent(void)
  : inherited(QEvent::User, NULL)
{
  this->t = (QEvent::Type) soqt6dofDevicePressureEvent;
  this->trans_x = 0.0f;
  this->trans_y = 0.0f;
  this->trans_z = 0.0f;
  this->rot_x = 0.0f;
  this->rot_y = 0.0f;
  this->rot_z = 0.0f;
}

void
SoQt6dofDevicePressureEvent::setTranslation(float x, float y, float z)
{
  this->trans_x = x;
  this->trans_y = y;
  this->trans_z = z;
}

void
SoQt6dofDevicePressureEvent::getTranslation(float & x,
                                            float & y,
                                            float & z) const
{
  x = this->trans_x;
  y = this->trans_y;
  z = this->trans_z;
}

void
SoQt6dofDevicePressureEvent::setRotation(float x, float y, float z)
{
  this->rot_x = x;
  this->rot_y = y;
  this->rot_z = z;
}

void
SoQt6dofDevicePressureEvent::getRotation(float & x, float & y, float & z) const
{
  x = this->rot_x;
  y = this->rot_y;
  z = this->rot_z;
}

// *************************************************************************

SoQt6dofDeviceButtonEvent::SoQt6dofDeviceButtonEvent(void)
  : inherited(QEvent::User, NULL)
{
  this->t = (QEvent::Type) soqt6dofDeviceButtonReleasedEvent;
  this->state = 0;
  this->button = 0;
  this->buttons = 0;
}

void
SoQt6dofDeviceButtonEvent::setButton(unsigned int button)
{
  this->button = button;
  if ((1 << this->button) & this->state)
    this->t = (QEvent::Type) soqt6dofDeviceButtonPressedEvent;
  else
    this->t = (QEvent::Type) soqt6dofDeviceButtonReleasedEvent;
}

unsigned int
SoQt6dofDeviceButtonEvent::getButton(void) const
{
  return this->button;
}

void
SoQt6dofDeviceButtonEvent::setState(unsigned int state)
{
  this->state = state;
  if ((1 << this->button) & this->state)
    this->t = (QEvent::Type) soqt6dofDeviceButtonPressedEvent;
  else
    this->t = (QEvent::Type) soqt6dofDeviceButtonReleasedEvent;
}

unsigned int
SoQt6dofDeviceButtonEvent::getState(void) const
{
  return this->state;
}

void
SoQt6dofDeviceButtonEvent::setNumButtons(unsigned int buttons)
{
  this->buttons = buttons;
}

unsigned int
SoQt6dofDeviceButtonEvent::getNumButtons(void) const
{
  return this->buttons;
}

int
SoQt6dofDeviceButtonEvent::isButtonPress(void) const
{
  if (this->t == (QEvent::Type) soqt6dofDeviceButtonPressedEvent)
    return TRUE;
  return FALSE;
}

// *************************************************************************
