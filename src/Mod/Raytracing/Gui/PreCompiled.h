#ifndef __PRECOMPILED_GUI__
#define __PRECOMPILED_GUI__

#include "../../../FCConfig.h"

// Importing of App classes
#ifdef FC_OS_WIN32
# define AppPartExport          __declspec(dllimport)
# define AppRaytracingExport    __declspec(dllimport)
# define AppRaytracingGuiExport __declspec(dllexport)
#else // for Linux
# define AppPartExport
# define AppRaytracingExport
# define AppRaytracingGuiExport
#endif

#ifdef _PreComp_

/// here get the warnings of to long specifieres disabled (needed for VC6)
#ifdef _MSC_VER
# pragma warning( disable : 4251 )
# pragma warning( disable : 4503 )
# pragma warning( disable : 4786 )  // specifier longer then 255 chars
#endif

// standard
#include <stdio.h>
#include <assert.h>

// STL
#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

// Xerces
#include <xercesc/util/XercesDefs.hpp>

#include <Python.h>
#ifdef FC_OS_WIN32
# include <windows.h>
#endif


// OpenCasCade Base
#include <Standard_Failure.hxx>

// OpenCascade View
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <Bnd_Box.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_NurbsConvert.hxx>
#include <BRepMesh.hxx>
#include <BRepMesh_Discret.hxx>
#include <BRepMesh_Edge.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepMesh_Triangle.hxx>
#include <BRepTools.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <Geom_Axis2Placement.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <gp_Trsf.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <Poly_Connect.hxx>
#include <Poly_Triangulation.hxx>
#include <Standard.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_ListIteratorOfListOfReal.hxx>
#include <TColStd_ListOfReal.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRep_Tool.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <GeomLProp_SLProps.hxx>
#include <TopoDS_Face.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Edge.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <GCPnts_UniformDeflection.hxx>

// Q Toolkit
#include <qaccel.h>
#include <qaction.h>
#include <qapplication.h>
#include <qbuttongroup.h>
#include <qcdestyle.h>
#include <qclipboard.h>
#include <qcolordialog.h>
#include <qcombobox.h>
#include <qdragobject.h>
#include <qdrawutil.h>
#include <qevent.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qframe.h>
#include <qhbox.h>
#include <qheader.h>
#include <qiconview.h> 
#include <qimage.h>
#include <qinputdialog.h>
#include <qkeycode.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qlistview.h>
#include <qmainwindow.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qmotifplusstyle.h>
#include <qmotifstyle.h>
#include <qmovie.h>
#include <qmultilinedit.h>
#include <qnamespace.h>
#include <qobjectlist.h>
#include <qpaintdevicemetrics.h>
#include <qpainter.h>
#include <qpalette.h> 
#include <qpixmap.h>
#include <qplatinumstyle.h>
#include <qpopupmenu.h>
#include <qprinter.h>
#include <qprogressbar.h>
#include <qpushbutton.h>
#include <qrangecontrol.h>
#include <qrect.h>
#include <qscrollbar.h>
#include <qspinbox.h>
#include <qsplitter.h> 
#include <qsgistyle.h>
#include <qstatusbar.h>
#include <qtabbar.h>
#include <qtextbrowser.h> 
#include <qtextstream.h>
#include <qthread.h>
#include <qtimer.h>
#include <qtoolbar.h> 
#include <qtoolbutton.h>
#include <qtooltip.h>
#include <qurl.h>
#include <qvalidator.h>
#include <qvariant.h>
#include <qvbox.h>
#include <qwhatsthis.h>
#include <qwidget.h>
#include <qwidgetstack.h>
#include <qwindowsstyle.h>
#include <qworkspace.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qslider.h>
#include <qwidgetfactory.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qradiobutton.h>
#include <qslider.h>



#endif  //_PreComp_

#endif // __PRECOMPILED_GUI__ 
