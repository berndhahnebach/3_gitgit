/***************************************************************************
 *   Copyright (c) 2004 J�rgen Riegel <juergen.riegel@web.de>              *
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


#ifndef __GUIDOCUMENT_h__
#define __GUIDOCUMENT_h__

#ifndef __Qt4All__
# include "Qt4All.h"
#endif

#ifndef __Qt3All__
# include "Qt3All.h"
#endif

#include "View.h"

#include <list>
#include <map>
#include <string>

#include <Base/PyExport.h>
#include <Base/Persistance.h>
#include <App/Document.h>
//Added by qt3to4:
//#include <QtGui/QCloseEvent>

#ifdef _MSC_VER
# pragma warning( disable : 4251 )
# pragma warning( disable : 4503 )
# pragma warning( disable : 4786 )  // specifier longer then 255 chars
# pragma warning( disable : 4290 )  // not implemented throw specification
# pragma warning( disable : 4275 )  
#endif

class SoSelection;
class SoPath;

namespace Base
{
  class Matrix4D;
}

namespace App
{
  class AbstractFeature;
}

namespace Gui {

class ViewProvider;
class Application;
class DocumentPy;
class DocItem;

class GuiExport DocChanges
{
public:  
  enum {
    New,
    Update,
    Delete
  } Why;

  std::set<ViewProvider*> ViewProviders;
};

/** The Gui Document
 *  This is the document on GUI level. Its main responsibility is 
 *  keeping track off open windows for a document, handling the OCC
 *  interactiv context and warning on unsaved closes.
 *  All handled views on the document must inherit from MDIView
 *  @see FCDocument 
 *  @see MDIView
 *  @author J�rgen Riegel
 */
class GuiExport Document : /*public QObject, */public App::Document::ObserverType, public Base::Subject<const DocChanges&>, public Base::Persistance
{
public:
  Document(App::Document* pcDocument, Application * app, const char * name=0);
  ~Document();


 	/** @name I/O of the document */
	//@{
  /// Save the document
  bool save(void);
  /// Save the document under a new file name
  bool saveAs(void);
  /// This method is used to save properties or very small amounts of data to an XML document.
  virtual void Save (Base::Writer &writer) const;
  /// This method is used to restore properties from an XML document.
  virtual void Restore(Base::XMLReader &reader);
  /// This method is used to save large amounts of data to a binary file.
  virtual void SaveDocFile (Base::Writer &writer) const;
  /// This method is used to restore large amounts of data from a binary file.
  virtual void RestoreDocFile(Base::Reader &reader);
  //@}

  /// Observer message from the App doc
  virtual void OnChange(App::Document::SubjectType &rCaller,App::Document::MessageType Reason);
  void setModified(bool);
  bool isModified() const { return _isModified; }

  /// Geter for the Application
  Application*    getAppWnd(){return _pcAppWnd;}

  /// Gerer for the App Document 
  App::Document*  getDocument(void) const {return _pcDocument;}


  /** @name methods for activation handling */
  //@{
  /// Geter for the Active View
  Gui::MDIView* getActiveView(void) const;
  //@}



  /** @name methods for View handling */
  //@{
  /// Creat a new View
  void createView(const char* sType); 
  /** send Messages to the active view 
    * send a specific massage to the active view and is able to recive a
    * return massage
    */
  /// send Messages to all views
  bool sendMsgToViews(const char* pMsg);
  /// Attach a view (get called by the MDIView constructor)
  void attachView(Gui::BaseView* pcView, bool bPassiv=false);
  /// Detach a view (get called by the MDIView destructor)
  void detachView(Gui::BaseView* pcView, bool bPassiv=false);
  /// call update on attached all views
  void onUpdate(void);
  /// call rename to all attached views
  void onRename(void);
  /// close all attached views in order to close the gui document
  void closeAllViews(void);
  /// returns a list of all attached MDI views
  std::list<MDIView*> getMDIViews() const;
  //@}


  /** @name View provider handling  */
  //@{
  /// Get the view provider for that Feature
  ViewProvider * getViewProvider(App::DocumentObject *) const;
  /// set an anotation view provider
  void setAnotationViewProvider(const char* name, ViewProvider *pcProvider);
  /// get an anotation view provider
  ViewProvider * getAnotationViewProvider(const char* name) const;
  /// remove an anotation view provider
  void rmvAnotationViewProvider(const char* name);
  /// test if the feature is in show
  bool isShow(const char* name);
  /// put the feature in show
  void setShow(const char* name);
  /// set the feature in Noshow
  void setHide(const char* name);
  /// set the feature transformation (only viewing)
  void setPos(const char* name, const Base::Matrix4D& rclMtrx);
  /// updates the view property of all view provider
  void update(void);
  ViewProvider *getViewProviderByName(const char* name) const;
  //@}



  /** @name methods for the UNDO REDO handling */
  //@{
  /// Open a new Undo transaction on the document
  void openCommand(const char* sName=0);
  /// Commit the Undo transaction on the document
  void commitCommand(void);
  /// Abort the Undo transaction on the document
  void abortCommand(void);
  /// Get an Undo string vector with the Undo names
  std::vector<std::string> getUndoVector(void) const;
  /// Get an Redo string vector with the Redo names
  std::vector<std::string> getRedoVector(void) const;
  /// Will UNDO  one or more steps
  void undo(int iSteps);
  /// Will REDO  one or more steps
  void redo(int iSteps) ;
  //@}

  /// handels the application close event
  void closeEvent ( QCloseEvent * e );
  ///
  void canClose(QCloseEvent * e );
  /// 
  bool isLastView(void);

  virtual PyObject *getPyObject(void);

protected:
  	// pointer to the python class
  Gui::DocumentPy *_pcDocPy;

private:
  static int _iDocCount;
  int        _iWinCount;
  int        _iDocId;
  bool _isClosing;
  bool _isModified;
  Application*    _pcAppWnd;
  /// List of all registered views
  std::list<Gui::BaseView*> _LpcViews;
  /// List of all registered views
  std::list<Gui::BaseView*> _LpcPassivViews;
  /// root of the document in the tree
  DocItem *pcTreeItem;

  // the doc/Document
  App::Document*  _pcDocument;
 
  std::map<App::DocumentObject*,ViewProvider*> _ViewProviderMap;
  std::map<std::string,ViewProvider*> _ViewProviderMapAnotation;

  /** @name attributes for the UNDO REDO facility
   */
  //@{
  /// undo names list
  std::list<std::string> listUndoNames;
  /// redo names list
  std::list<std::string> listRedoNames;
  //@}
/*
  // selection stuff
  SoSelection *pcSelection;
  static void sFinishSelectionCallback(void *,SoSelection *);
  virtual void finishSelectionCallback(SoSelection *);
  static void sMadeSelection(void *,SoPath *);
  virtual void madeSelection(SoPath *);
  static void sUnmadeSelection(void *,SoPath *);
  virtual void unmadeSelection(SoPath *);
*/
};

} // namespace Gui


#endif // __GUIDOCUMENT_h__

