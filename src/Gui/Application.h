/** \file Application.h
 *  \brief  
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 *   
 */

/***************************************************************************
 *   (c) J�rgen Riegel (juergen.riegel@web.de) 2002                        *   
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License (LGPL)   *
 *   as published by the Free Software Foundation; either version 2 of     *
 *   the License, or (at your option) any later version.                   *
 *   for detail see the LICENCE text file.                                 *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        * 
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with FreeCAD; if not, write to the Free Software        * 
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
 *   USA                                                                   *
 *                                                                         *
 *   Juergen Riegel 2002                                                   *
 ***************************************************************************/
#ifndef APPLICATION_H
#define APPLICATION_H


#include <qmainwindow.h>
#include <qaction.h>
#include <qworkspace.h>
#include <qthread.h>

#include <qextmdimainfrm.h>


#define  putpix()

#include "../Base/Console.h"
#include "../App/Application.h"
#include "Command.h"
#include "Widgets.h"
#include "PrefWidgets.h"
#include <list>
#include <vector>
#include <map>

class FCGuiDocument;
class QComboBox;
class FCWindow;
class QToolBar;
class FCView;
class FCToolboxBar;
class FCAutoWaitCursor;
class FCCmdBar;
class FCHtmlView;
class FCUndoRedoDlg;
class FCMacroManager;
class QPopupMenu;
class QToolBar;


/** The Bitmap Factory
  * the main purpos is to collect all build in Bitmaps and 
  * hold all paths for the extern bitmaps (files) to serve
  * as a single point of axcesing bitmaps in FreeCAD
  */
class FCBmpFactory
{
public:
	FCBmpFactory(void);
	~FCBmpFactory();
	/// Adds a path were pixmaps can be found
	void AddPath(const char* sPath);
	/// Remove a path from the list of pixmap paths
	void RemovePath(const char* sPath);

	/// Add a build in XPM pixmap under a given name
	void AddXPM(const char* sName, const char* pXPM);
	/// Remove a build in pixmap by a given name
	void RemoveXPM(const char* sName);

	/// retrive a pixmap by name
	QPixmap GetPixmap(const char* sName);

protected:
	std::map<std::string,const char*> _mpXPM;
	std::vector<std::string>          _vsPaths;
};


/** The Applcation main class
 *  This is the central class of the GUI 
 */
class GuiExport ApplicationWindow: public QextMdiMainFrm, public FCApplicationObserver
{
    Q_OBJECT
 
	
public:
	/// construction
    ApplicationWindow();
	/// destruction
    ~ApplicationWindow();

	void CreateStandardOperations();

	// Observer
	virtual void OnDocNew(FCDocument* pcDoc);
	virtual void OnDocDelete(FCDocument* pcDoc);

	/// message when a GuiDocument is about to vanish
	void OnLastWindowClosed(FCGuiDocument* pcDoc);

	/// some kind of singelton
	static ApplicationWindow* Instance;

	/// send Messages to the active view
	bool SendMsgToActiveView(const char* pMsg);
	/// returns the active view or NULL
	FCView* GetActiveView(void);
	/// Geter for the Active View
	FCGuiDocument* GetActiveDocument(void);


	/// Reference to the command manager
	FCCommandManager &GetCommandManager(void){return _cCommandManager;}
	
	/// Returns the widget manager
	FCCustomWidgetManager* GetCustomWidgetManager(void) { return _pcWidgetMgr; }

	/// Referenc to the bitmap factory
	FCBmpFactory     &GetBmpFactory(void){return _cBmpFactory;}

	/** @name dock window handling */
	//@{	
	/// Add a new named Dock Window
	void          AddDockWindow(const char* name,FCWindow *pcDocWindow, const char* sCompanion = NULL,KDockWidget::DockPosition pos = KDockWidget::DockRight);
	/// Gets you a registered Dock Window back
	FCWindow *    GetDockWindow(const char* name);
	/// Delete (or only remove) a named Dock Window
	void          DelDockWindow(const char* name, bool bOnlyRemove = false);
	//@}

	/** @name status bar handling */
	//@{	
	/// set text to the pane
	void SetPaneText(int i, QString text);
	/// gets a pointer to the Progress bar
	FCProgressBar* GetProgressBar();
	//@}

	/** @name workbench handling */
	//@{	
	/// Activate a named workbench
	void ActivateWorkbench(const char* name);
	/// update the combo box when there are changes in the workbenches
	void UpdateWorkbenchEntrys(void);
	/// returns the name of the active workbench
	QString GetActiveWorkbench(void){return _cActiveWorkbenchName;}
	//@}

	/// Get macro manager
	FCMacroManager *GetMacroMngr(void){return _pcMacroMngr;}




	//---------------------------------------------------------------------
	// python exports goes here +++++++++++++++++++++++++++++++++++++++++++	
	//---------------------------------------------------------------------

	// static python wrapper of the exported functions
	PYFUNCDEF_S(sToolbarAddTo);
	PYFUNCDEF_S(sToolbarDelete);
	PYFUNCDEF_S(sToolbarAddSeperator);
	PYFUNCDEF_S(sToolbarLoadSettings);

	PYFUNCDEF_S(sCommandbarAddTo);
	PYFUNCDEF_S(sCommandbarDelete);
	PYFUNCDEF_S(sCommandbarLoadSettings);
	PYFUNCDEF_S(sCommandbarAddSeperator);

	PYFUNCDEF_S(sWorkbenchAdd);
	PYFUNCDEF_S(sWorkbenchDelete);
	PYFUNCDEF_S(sWorkbenchActivate);
	PYFUNCDEF_S(sWorkbenchGet);

	PYFUNCDEF_S(sUpdateGui);

	PYFUNCDEF_S(sCommandAdd);

	static PyMethodDef    Methods[]; 
 

signals:
	void sendQuit();
    void timeEvent();


protected: // Protected methods
	/** just fits the system menu button position to the menu position */
	virtual void resizeEvent ( QResizeEvent * );
	virtual void closeEvent ( QCloseEvent * e );
	virtual bool eventFilter( QObject* o, QEvent *e );
	/// Handels all commands 
	FCCommandManager _cCommandManager;
	FCBmpFactory     _cBmpFactory;
	/// waiting cursor stuff 
	void timerEvent( QTimerEvent * e){emit timeEvent();}

public slots:
	/// this slot get frequently activatet and test the commands if they are still active
	void UpdateCmdActivity();
	/** @name methodes for the UNDO REDO handling 
	 *  this methodes are usaly used by the GUI document! Its not intended
	 *  to use them directly. If the GUI is not up, there is usaly no UNDO / REDO 
	 *  nececary.
	 */
	//@{
	void slotUndo();
	void slotRedo();
	//@}
	void OnWorkbenchChange( const QString & string);

private:
#	pragma warning( disable : 4251 )
	/// list of all handled documents
 	std::list<FCGuiDocument*>         lpcDocuments;
	/// list of windows
	std::map <std::string,FCWindow*> mpcDocWindows;
#	pragma warning( default : 4251 )
	/// Active document
	FCGuiDocument*   _pcActiveDocument;
	FCCustomWidgetManager*		 _pcWidgetMgr;
	FCMacroManager*  _pcMacroMngr;
	QComboBox *		 _pcWorkbenchCombo;
	QLabel *         _pclSizeLabel, *_pclActionLabel;
	FCProgressBar *  _pclProgress;
	FCCmdBar*        _pcCmdBar;
	FCHtmlView*		 _pcHtmlView;
	/// workbench python dictionary
	PyObject*		 _pcWorkbenchDictionary;
	QString			 _cActiveWorkbenchName;
	QTimer *		 _pcActivityTimer; 

};



/** The console observer for the Application window
 *  This class distribute the Messages issued on the console
 *  to the status bar. 
 *  @see FCConsole
 *  @see FCConsoleObserver
 */
class FCGuiConsoleObserver: public FCConsoleObserver
{
public:
	FCGuiConsoleObserver(ApplicationWindow *pcAppWnd);
		
	/// get calles when a Warning is issued
	virtual void Warning(const char *m);
	/// get calles when a Message is issued
	virtual void Message(const char * m);
	/// get calles when a Error is issued
	virtual void Error  (const char *m);
	/// get calles when a Log Message is issued
	virtual void Log    (const char *);

	/// Mutes the Observer, no Dialog Box will appear
	static bool bMute;
protected:
	ApplicationWindow* _pcAppWnd;
};


/** The FCAutoWaitCursor sets automatically the 
 *  waiting cursor if the application is busy
 *  @author Werner Mayer
 */
class GuiExport FCAutoWaitCursor : public QObject, public QThread
{

	Q_OBJECT

public:
	static void Destruct(void);
	static FCAutoWaitCursor &Instance(void);
	// getter/setter
	int GetInterval();
	void SetInterval(int i);
  /// sets the wait cursor calling API methods of the OS
	void SetWaitCursor();

  // Singleton
private:
#ifdef FC_OS_WIN32 // windows os
    FCAutoWaitCursor(DWORD id, int i);
#else
    FCAutoWaitCursor(int i);
#endif

    static FCAutoWaitCursor* _pclSingleton;

protected:
    /// run the thread, decrement an internal counter
    void run();

    QMutex awcMutex;
    int iAutoWaitCursorCounter;
    int iAutoWaitCursorMaxCount;
    int iInterval;
    bool bOverride;
#ifdef FC_OS_WIN32
  	DWORD main_threadid;
#endif

public slots:
    /// increment the internal counter
    void timeEvent();
};







#endif
