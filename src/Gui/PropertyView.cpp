/***************************************************************************
 *   Copyright (c) 2002 J�rgen Riegel <juergen.riegel@web.de>              *
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
# include <qcursor.h>
# include <qdatetime.h>
# include <qlayout.h>
# include <qtabwidget.h>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......

#include <App/PropertyStandard.h>
#include <App/PropertyGeo.h>
#include <App/PropertyLinks.h>
#include <App/PropertyContainer.h>
#include <App/Feature.h>
#include <App/Document.h>

#include "PropertyView.h"
#include "Application.h"
#include "Document.h"
#include "BitmapFactory.h"
#include "ViewProvider.h"

#include "propertyeditor/propertyeditor.h"
#include "propertyeditor/propertyeditorlist.h"
#include "propertyeditor/propertyeditorfile.h"
#include "propertyeditor/propertyeditorfont.h"
#include "propertyeditor/propertyeditorinput.h"
#include "propertyeditor/propertyeditordate.h"


using namespace std;
using namespace Gui;
using namespace Gui::DockWnd;
using namespace Gui::PropertyEditor;


QPixmap* PropertyView::pcLabelOpen=0;
QPixmap* PropertyView::pcLabelClosed=0;
QPixmap* PropertyView::pcAttribute=0;

/* TRANSLATOR Gui::DockWnd::PropertyView */

PropertyView::PropertyView(Gui::Document* pcDocument,QWidget *parent,const char *name)
  :DockView(pcDocument,parent,name)
{
  setCaption( tr( "Property View" ) );

  QGridLayout* pLayout = new QGridLayout( this ); 
  pLayout->setSpacing( 0 );
  pLayout->setMargin ( 0 );

  QTabWidget* pTabs = new QTabWidget (this);
  pTabs->setTabPosition(QTabWidget::Bottom);
  pTabs->setTabShape(QTabWidget::Triangular);
  pLayout->addWidget( pTabs, 0, 0 );

  _pPropEditor = new EditableListView( pTabs );
  EditableItem::parentView = _pPropEditor;
  pTabs->insertTab(_pPropEditor, "Properties");
  _pPropEditor->addColumn("Name");
  _pPropEditor->addColumn( "Value" );

  // retrieve the Pixmaps
  pcLabelOpen   = new QPixmap(Gui::BitmapFactory().pixmap("RawTree_LabelOpen"));
  pcLabelClosed = new QPixmap(Gui::BitmapFactory().pixmap("RawTree_LabelClosed"));
  pcAttribute   = new QPixmap(Gui::BitmapFactory().pixmap("RawTree_Attr"));

  //_pcListView->setSize(200,400);
  resize( 200, 400 );

  onUpdate();

  Gui::Selection().Attach(this);

}

PropertyView::~PropertyView()
{
  Gui::Selection().Detach(this);
}


void PropertyView::OnChange(Gui::SelectionSingleton::SubjectType &rCaller,Gui::SelectionSingleton::MessageType Reason)
{
  //FIXME: We get notified for each object that has changed its selection status. It is sufficient to be called only once.
  //

  if(Reason.Type == SelectionChanges::ClearSelection)
  {
    tearDown();
  }else if(Reason.Type == SelectionChanges::AddSelection || Reason.Type == SelectionChanges::RmvSelection){
#if 0
    // We must not listen neither to the given rCaller nor to the Reason object, because there can be selected items from
    // documents other as the currently active one.
    vector<SelectionSingleton::SelObj> list = Gui::Selection().getCompleteSelection();
    if(list.size() == 1){
      if(list.begin()->pFeat)
        buildUp(list.begin()->pFeat);
      else if(list.begin()->pDoc)
        buildUp(reinterpret_cast<App::PropertyContainer*>(list.begin()->pDoc));
    }else
      tearDown();
#else
    // We must not listen neither to the given rCaller nor to the Reason object only, because there can be selected items from
    // documents other than the currently active one.

    // group the properties by <name,id>
    std::map<std::pair<std::string, int>, std::vector<App::Property*> > propMap;
    vector<SelectionSingleton::SelObj> array = Gui::Selection().getCompleteSelection();
    for ( vector<SelectionSingleton::SelObj>::const_iterator it = array.begin(); it != array.end(); ++it )
    {
      std::map<std::string,App::Property*> Map;
      if ( (*it).pObject ) {
        (*it).pObject->getPropertyMap(Map);
        // get also the properites of the associated view provider
        Gui::Document* doc = Gui::Application::Instance->getDocument(it->pDoc);
        ViewProvider* vp = doc->getViewProvider((*it).pObject);
        vp->getPropertyMap(Map);
      }
      else if ( (*it).pDoc ) {
        (*it).pDoc->getPropertyMap(Map);
      }

      // store the properties with <name,id> as key in a map
      for( std::map<std::string,App::Property*>::iterator pt = Map.begin(); pt != Map.end(); ++pt )
      {
        std::pair<std::string, int> nameType = std::make_pair<std::string, int>( pt->first, pt->second->getTypeId().getKey());
        propMap[nameType].push_back(pt->second);
      }
    }

    tearDown();

    std::map<std::pair<std::string, int>, std::vector<App::Property*> >::const_iterator jt;
    for ( jt = propMap.begin(); jt != propMap.end(); ++jt )
    {
      // the property must be part of each selected object, i.e. the number of selected objects is equal 
      // to the number of properties with same name and id
      if ( jt->second.size() == array.size() )
      {
        App::Property* prop = (jt->second)[0];
        QString editor = prop->getEditorName();
        if ( !editor.isEmpty() )
        {
          EditableItem* item = (EditableItem*) Base::Type::createInstanceByName( prop->getEditorName(),true);
          if ( item )
          {
            item->setText(0, QString(prop->getName()));
            item->setProperty( jt->second );
          }
        }
      }
    }
    
#endif
  }
}

void PropertyView::buildUp(App::PropertyContainer *cont)
{

  tearDown();
  
  std::map<std::string,App::Property*> Map;
  cont->getPropertyMap(Map);

  std::map<std::string,App::Property*>::iterator it;
  for(it = Map.begin(); it != Map.end(); ++it)
  {
    EditableItem* item = (EditableItem*) Base::Type::createInstanceByName( it->second->getEditorName(),true);
    if ( item )
    {
      item->setText(0, QString(it->first.c_str()));
//      item->setProperty( it->second );
    }
  }
}


void PropertyView::tearDown(void)
{

  _pPropEditor->stopEdit();

  _pPropEditor->clear();
}


void PropertyView::onUpdate(void)
{

/*
  PropertyBuffer* buf = new PropertyBuffer(_pPropEditor, "Test");

  // append sample properties
  buf->add(new Property("Boolean", QVariant(true,1)));
  buf->add(new Property("Integer", 9));
  buf->add(new Property("String", QString("Hallo")));
  buf->add(new Property("Double", 3.14));
  buf->add(new Property("Font", QFont()));
  buf->add(new Property("Pixmap", QPixmap()));
  buf->add(new Property("Color", Qt::blue));
  buf->add(new Property("Time", QTime()));
  buf->add(new Property("Cursor", QCursor()));
  buf->add(new Property("CString", QCString("Hallo")));
  buf->add(new Property("Date time", QDateTime()));
  buf->add(new Property("Date", QDate()));
  QStringList l; l << "Hallo" << "Hallo2";
  buf->add(new Property("List", "Test", l, l, "Descr."));
  _pPropEditor->setBuffer(buf);
  if ( _pPropEditor->childCount() > 0)
    return;
  QStringList lst; lst << QString("This") << QString("is") << QString("my") << QString("first") << QString("list") << QString("first");
  new ListEditorItem(_pPropEditor, QString("List"), lst);
  new DateEditorItem(_pPropEditor, QString("Date"), QDate::currentDate ());
  new DateTimeEditorItem(_pPropEditor, QString("Datetime"), QDateTime::currentDateTime ());
  new FileEditorItem(_pPropEditor, QString("File name"), QCString());
  new CursorEditorItem(_pPropEditor, QString("Cursor"), QCursor(Qt::SplitVCursor));
  new TimeEditorItem(_pPropEditor, QString("Time"), QTime::currentTime ());
  new ColorEditorItem(_pPropEditor, QString("Color"), Qt::red);
  new ChildrenEditorItem(_pPropEditor, QString("Children"), 1);
  new PixmapEditorItem( _pPropEditor, QString("Pixmap"), QPixmap() );
  new FontEditorItem( _pPropEditor, QString("Font"), QFont() );
  new FloatEditorItem( _pPropEditor, QString("Float"), 0.85f );
  new TextEditorItem( _pPropEditor, QString("Text"), QString("Example text") );
  new IntEditorItem( _pPropEditor, QString("Integer"), 2 );
  new BoolEditorItem( _pPropEditor, QString("Boolean"), QVariant(true, 0) );
*/
//  Base::Console().Log("Property Updated\n");
}

void PropertyView::onNewDocument(Gui::Document* pcOldDocument,Gui::Document* pcNewDocument)
{
}

bool PropertyView::onMsg(const char* pMsg)
{
  // no messages yet
  return false;
}

//#include "moc_PropertyView.cpp"
