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

// @configure_input@

#ifndef SOQT_H
#define SOQT_H

#include <Inventor/Qt/SoQtBasic.h>

// FIXME: use configure defines for the header files.
// 20020613 mortene.

#ifdef __COIN_SOQT__
#include <qobject.h>
#endif // __COIN_SOQT__
#ifdef __COIN_SOXT__
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#endif // __COIN_SOXT__
#ifdef __COIN_SOGTK__
// Fetch stdlib.h, so NULL is defined before glib.h is (indirectly)
// included. Otherwise we get a compile error with KCC on some
// systems.
#include <stdlib.h>
#include <gtk/gtk.h>
class SoGtkComponent;
class SbPList;
#endif // __COIN_SOGTK__
#ifdef __COIN_SOWIN__
#include <windows.h>
#endif // __COIN_SOWIN__

#include <Inventor/SbBasic.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SbString.h>
#include <Inventor/SoDB.h>
#include <Inventor/errors/SoDebugError.h>

// *************************************************************************

class SOQT_DLL_API SoQt
{

public:
  static QWidget* init(const char * appname, const char * classname = "SoQt");
  static QWidget* init(int & argc, char ** argv,
                       const char * appname, const char * classname = "SoQt");
  static void init(QWidget* toplevelwidget);

  static void mainLoop(void);
  static void exitMainLoop(void);
  static void done(void);

  static QWidget* getTopLevelWidget(void);
  static QWidget* getShellWidget(const QWidget* w);

  static void show(QWidget* const widget);
  static void hide(QWidget* const widget);

  static void setWidgetSize(QWidget* const widget, const SbVec2s size);
  static SbVec2s getWidgetSize(const QWidget* widget);

  static void createSimpleErrorDialog(QWidget* widget,
                                      const char * title,
                                      const char * string1,
                                      const char * string2 = NULL);

  static void getVersionInfo(int * major = NULL,
                             int * minor = NULL,
                             int * micro = NULL);
  static const char * getVersionString(void);

  enum FatalErrors {
    UNSPECIFIED_ERROR = 0,
    NO_OPENGL_CANVAS,
    INTERNAL_ASSERT
  };
  typedef void FatalErrorCB(const SbString errmsg, SoQt::FatalErrors errcode,
                            void * userdata);
  static FatalErrorCB * setFatalErrorHandler(SoQt::FatalErrorCB * cb,
                                             void * userdata);

  static SbBool isDebugLibrary(void);
  static SbBool isCompatible(unsigned int major, unsigned int minor);

  enum ABIType { DLL, LIB, UNKNOWN };
  static ABIType getABIType(void);

private:
  // Since the class consists solely of static functions, hide the
  // default constructor and the destructor so nobody can instantiate
  // it.
  SoQt(void);
  virtual ~SoQt();

#ifndef DOXYGEN_SKIP_THIS // To avoid having it show up in Doxygen doc.
  friend class SoGuiP;
  friend class SoQtP;
#endif // DOXYGEN_SKIP_THIS


/// FIXME!: merge the remaining toolkit specific parts properly. 20020117 mortene. ////

#ifdef __COIN_SOXT__
public:
  static void nextEvent(XtAppContext, XEvent *);
  static Boolean dispatchEvent(XEvent * event);
  static XtAppContext getAppContext(void);
  static Display * getDisplay(void);
  static XmString encodeString(const char * const str);
  static char * decodeString(XmString xstring);
  static void getPopupArgs(Display * display, int screen,
                           ArgList args, int * n);

  static void registerColormapLoad(Widget widget, Widget shell);
  static void addColormapToShell(Widget widget, Widget shell);
  static void removeColormapFromShell(Widget widget, Widget shell);

  static void addExtensionEventHandler(Widget widget,
                                       int eventType, XtEventHandler proc,
                                       XtPointer clientData);
  static void removeExtensionEventHandler(Widget widget,
                                          int eventType, XtEventHandler proc,
                                          XtPointer clientData);

  static void selectBestVisual(Display * dpy, Visual * & visual,
                               Colormap & cmap, int & depth);

  static const char * getAppName(void);
  static const char * getAppClass(void);

protected:
  static void getExtensionEventHandler(XEvent * event, Widget & widget,
                                       XtEventHandler & proc,
                                       XtPointer & clientData);
#endif // __COIN_SOXT__

#ifdef __COIN_SOWIN__
public:
  static BOOL dispatchEvent(MSG * msg);
  static SbBool nextEvent(int appContext, MSG * msg);
  static void setInstance(HINSTANCE instance);
  static HINSTANCE getInstance(void);
  static void errorHandlerCB(const class SoError * error, void * data);
  static void doIdleTasks(void);

protected:
  friend class SoWinP;

  static void registerWindowClass(const char * const className);
  static void unRegisterWindowClass(const char * const className);

  static HWND createWindow(char * title, char * className,
                           SIZE size, HWND parent = NULL, HMENU menu = NULL);

  static LRESULT CALLBACK eventHandler(HWND window, UINT message,
                                       WPARAM wparam, LPARAM lparam);

#endif // __COIN_SOWIN__

#ifdef __COIN_SOGTK__
public:
  friend class SoGtkComponent;
  enum SoGtkComponentAction { CREATION, DESTRUCTION, CHANGE };
  typedef void SoGtkComponentActionCallback(SoGtkComponent *, SoGtk::SoGtkComponentAction, void *);

  static void addComponentActionCallback(SoGtkComponentActionCallback *, void *);
  static void removeComponentActionCallback(SoGtkComponentActionCallback *, void *);

  static int getComponents(SbPList & components);

protected:
  static void invokeComponentActionCallbacks(SoGtkComponent * component,
                                             SoGtkComponentAction action);

  static gint componentCreation(SoGtkComponent * component);
  static gint componentDestruction(SoGtkComponent * component);
  static gint componentChange(SoGtkComponent * component);

private:
  static gint timerSensorCB(gpointer data);
  static gint idleSensorCB(gpointer data);
  static gint delaySensorCB(gpointer data);

  static GtkWidget * mainWidget;
  static SbPList * components;
  static SbPList * component_callbacks;
#endif // __COIN_SOGTK__
};

// *************************************************************************

#endif // ! SOQT_H
