/** \file $RCSfile$
 *  \brief The Gui Document
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 */


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   J�rgen Riegel 2002                                                    *
 *                                                                         *
 ***************************************************************************/
 

#include "../Config.h"
#ifdef _PreComp_
#	include "PreCompiled.h"
#else
#	include <qstatusbar.h>
#	ifndef WNT
#		include <Graphic3d_GraphicDevice.hxx>
#	else
#		include <Graphic3d_WNTGraphicDevice.hxx>
#	endif
#endif


#include "../App/Document.h"
#include "Application.h"
#include "Document.h"
#include "View3D.h"



FCGuiDocument::FCGuiDocument(FCDocument* pcDocument,ApplicationWindow * app, const char * name)
	:_pcAppWnd(app),
	 _pcDocument(pcDocument),
	 _iWinCount(0)
{
	// keeping an Instance of this document as long as at least one window lives
	_pcDocument->_INCREF();

	TCollection_ExtendedString a3DName("Visu3D");
	_hViewer = Viewer(getenv("DISPLAY"),a3DName.ToExtString(),"",1000.0,V3d_XposYnegZpos,Standard_True,Standard_True);

    _hViewer->Init();
	_hViewer->SetDefaultLights();
	_hViewer->SetLightOn();

	_hContext =new AIS_InteractiveContext(_hViewer);

	// World coordinate system
	Handle(AIS_Trihedron) hTrihedron;
	hTrihedron = new AIS_Trihedron(new Geom_Axis2Placement(gp::XOY()));
	_hContext->Display(hTrihedron);
	_hContext->Deactivate(hTrihedron);

	// alwayes create at least one view
	CreateView("View3D");

}

FCGuiDocument::~FCGuiDocument()
{
	for(FClist<FCView*>::iterator It = _LpcViews.begin();It != _LpcViews.end() ;It++) 
		delete *It;

	// remove the reverence from the object
	_pcDocument->_DECREF();
}

#include "Icons/FCIcon.xpm"

void FCGuiDocument::CreateView(const char* sType) 
{

    FCView* w = new FCView3D(this,0L,"View");
	
	// add to the view list of document
	_LpcViews.push_back(w);
	// add to the view list of Application window

	//connect( w, SIGNAL( message(const QString&, int) ), _pcAppWnd->statusBar(), SLOT( message(const QString&, int )) );
	//connect( w, SIGNAL(sendCloseView(FCView*)),this,SLOT(onCloseView(FCView*)));
	connect( w, SIGNAL(sendCloseView(FCView*)),this,SLOT(slotCloseView(FCView*)));

	QString aName;
//	aName.sprintf("%s:%d",_pcDocument->GetName(),_iWinCount++);
	aName.sprintf("%s:%d","Document",_iWinCount++);
    w->setCaption(aName);
	w->setTabCaption(aName);
    w->setIcon( FCIcon );

	w->resize( 400, 300 );
    if ( _LpcViews.size() == 1 )
		_pcAppWnd->addWindow(w,QextMdi::StandardAdd);
    else
		_pcAppWnd->addWindow(w);

}

void FCGuiDocument::slotCloseView(FCView* theView)
{

	_LpcViews.remove(theView);
	// last view?
	if(_LpcViews.size() == 0)
	{
		OnLastViewClosed();
	}
}

void FCGuiDocument::OnLastViewClosed(void)
{

	_pcAppWnd->OnLastWindowClosed(this);
}

/// send Messages to the active view
bool FCGuiDocument::SendMsgToViews(const char* pMsg)
{

	bool bResult = false;

	for(FClist<FCView*>::iterator It = _LpcViews.begin();It != _LpcViews.end();It++)
	{
		if( (*It)->OnMsg(pMsg))
		{
			bResult = true;
			break;
		}
	}

	return bResult;
}

/// send Messages to all views
bool FCGuiDocument::SendMsgToActiveView(const char* pMsg)
{

	if(_pcActiveView)
		return _pcActiveView->OnMsg(pMsg);
	else
		return false;
}


/// set the parameter to the active view or reset in case of 0
void FCGuiDocument::SetActive(FCView* pcView)
{
	_pcActiveView = pcView;

	// trigger the application about Activity change
	if(_pcActiveView)
		_pcAppWnd->SetActive(this);
	else
		_pcAppWnd->SetActive(0l);

}

/// Geter for the Active View
FCView* FCGuiDocument::GetActiveView(void)
{
	return _pcActiveView;
}






Handle(V3d_Viewer) FCGuiDocument::Viewer(const Standard_CString aDisplay,
										 const Standard_ExtString aName,
										 const Standard_CString aDomain,
										 const Standard_Real ViewSize,
										 const V3d_TypeOfOrientation ViewProj,
										 const Standard_Boolean ComputedMode,
										 const Standard_Boolean aDefaultComputedMode )
{
#	ifndef WNT
		static Handle(Graphic3d_GraphicDevice) defaultdevice;

		if(defaultdevice.IsNull()) defaultdevice = new Graphic3d_GraphicDevice(aDisplay);
		return new V3d_Viewer(defaultdevice,aName,aDomain,ViewSize,ViewProj,
								Quantity_NOC_GRAY30,V3d_ZBUFFER,V3d_GOURAUD,V3d_WAIT,
								ComputedMode,aDefaultComputedMode,V3d_TEX_NONE);
#	else
		static Handle(Graphic3d_WNTGraphicDevice) defaultdevice;

		if(defaultdevice.IsNull()) defaultdevice = new Graphic3d_WNTGraphicDevice();
		return new V3d_Viewer(defaultdevice,aName,aDomain,ViewSize,ViewProj,
								Quantity_NOC_GRAY30,V3d_ZBUFFER,V3d_GOURAUD,V3d_WAIT,
								ComputedMode,aDefaultComputedMode,V3d_TEX_NONE);
#	endif  // WNT
}



#include "Document_moc.cpp"