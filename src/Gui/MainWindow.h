/***************************************************************************
 *   Copyright (c) 2005 Werner Mayer <werner.wm.mayer@gmx.de>              *
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


#ifndef GUI_MAINWINDOW_H
#define GUI_MAINWINDOW_H

#include <Base/Console.h>
#include <string>
#include <vector>

#ifndef __Qt4All__
# include "Qt4All.h"
#endif


namespace Gui {

class BaseView;
class CommandManager;
class Document;
class MacroManager;
class MDIView;

namespace DockWnd {
  class HelpView;
} //namespace DockWnd


/** 
 * The MainWindow class provides a main window with menu bar, toolbars, dockable windows,
 * a status bar and mainly a workspace for the MDI windows.
 * @author Werner Mayer
 */
class GuiExport MainWindow: public QMainWindow
{
    Q_OBJECT
 
public:
  /**
   * Constructs an empty main window. For default \a parent is 0, as there usually is
   * no toplevel window there.
   */
  MainWindow( QWidget * parent = 0, Qt::WFlags f = Qt::Window );
  /** Destroys the object and frees any allocated resources. */
  ~MainWindow();
  /**
   * Filters events if this object has been installed as an event filter for the watched object.
   */
  bool eventFilter ( QObject* o, QEvent* e );
  /** 
   * Adds an MDI window \a view to the main window's workspace and adds a new tab
   * to the tab bar.
   */
  void addWindow( MDIView* view );
  /**
   * Removes an MDI window from the main window's workspace and its associated tab without
   * deleting the widget. If the main windows does not have such a window nothing happens.
   */
  void removeWindow( MDIView* view );
  /**
   * Can be called after the caption of an MDIView has changed to update the tab's caption.
   */
  void tabChanged( MDIView* view );
  /**
   * Returns the first window with the caption \a caption, if there is no window with this caption 0 is returned. 
   */
  MDIView* getWindowWithCaption( const QString& ) const;
  /**
   * Returns a list of all MDI windows in the worpspace.
   */
  QList<QWidget*> windows( QWorkspace::WindowOrder order = QWorkspace::CreationOrder ) const;
  /**
   * Returns the active MDI window or 0 if there is none.
   */
  MDIView* activeWindow() const;
  /**
   * Sets the active window to \a view.
   */
  void setActiveWindow( MDIView* view );
  /**
   * Sets text to the pane in the status bar.
   */
  void setPaneText(int i, QString text);
  /**
   * MRU: Appends \a file to the list of recent files.
   */
  void appendRecentFile(const QString& filename);
  /**
   * Returns true that the context menu contains the 'Customize...' menu item.
   */
  QMenu * createPopupMenu();

  /** @name Splasher and access methods */
  //@{
  /** Gets the one and only instance. */
  static MainWindow* getInstance();
  /** Starts the splasher at startup. */
  void startSplasher(void);
  /** Stops the splasher after startup. */
  void stopSplasher(void);
  /** Shows the Tip-of-the-day dialog after startup. */
  void showTipOfTheDay(bool force=false);
  //@}

  /** @name Layout Methods 
   */
  //@{
  /// Loads the main window settings.
  void loadWindowSettings();
  /// Saves the main window settings.
  void saveWindowSettings();
  /// Loads the dock windows and toolbar settings.
  void loadLayoutSettings();
  /// Saves the dock windows and toolbar settings.
  void saveLayoutSettings();
  //@}

public Q_SLOTS:
  /**
   * Arranges all child windows in a horizontal tile pattern.
   */
  void arrangeIcons();
  /**
   * Arranges all child windows in a tile pattern.
   */
  void tile();
  /**
   * Arranges all the child windows in a cascade pattern.
   */
  void cascade();
  /**
   * Closes the child window that is currently active.
   */
  void closeActiveWindow ();
  /**
   * Closes all child windows. 
   * The windows are closed in random order. The operation stops if a window does not accept the close event.
   */
  void closeAllWindows ();
  /**
   * Activates the next window in the child window chain.
   */
  void activateNextWindow ();
  /**
   * Activates the previous window in the child window chain.
   */
  void activatePreviousWindow ();
  /**
   * Starts the what's this mode.
   */
  void whatsThis();
  /** 
   * This method gets frequently activated and test the commands if they are still active.
   */
  void updateActions();
  void switchToTopLevelMode();
  void switchToDockedMode();

protected:
  /**
   * This method checks if the main window can be closed by checking all open documents and views.
   */
  void closeEvent ( QCloseEvent * e );
  void timerEvent( QTimerEvent * e){ timeEvent();}
  void customEvent( QEvent* e );
  /**
   * Try to interpret dropped elements.
   */
  void dropEvent        ( QDropEvent        * e );
  /**
   * Checks if a mime source object can be interpreted.
   */
  void dragEnterEvent   ( QDragEnterEvent   * e );
  /**
   * This method is called from the Qt framework automatically whenever a
   * QTranslator object has been installed. This allows to translate all
   * relevant user visible text.
   */
  void changeEvent( QEvent *e );

private Q_SLOTS:
  /**
   * Activates the associated tab to this widget.
   */
  void onWindowActivated( QWidget* );
  /**
   * Fills up the menu with the current windows in the workspace.
   */
  void onWindowsMenuAboutToShow();
  /**
   * Removes the associated tab to the window when it gets destroyed from outside.
   */
  void onWindowDestroyed();
  /**
   * Activates the associated window to the tab with \a id.
   */
  void onTabSelected( int i);

Q_SIGNALS:
  void timeEvent();
  void windowStateChanged(MDIView*);

private:
  /// some kind of singleton
  static MainWindow* instance;
  struct MainWindowP* d;
};

inline GuiExport MainWindow* getMainWindow()
{
  return MainWindow::getInstance();
}

// -------------------------------------------------------------

/** The status bar observer displays the text on the status bar of the main window
 * in an appropriate color. Normal text messages are black, warnings are orange and
 * error messages are in red. Log messages are completely ignored.
 * The class is implemented to be thread-safe.
 * @see Console
 * @see ConsoleObserver
 * @author Werner Mayer
 */
class StatusBarObserver: public Base::ConsoleObserver
{
public:
  StatusBarObserver();
  virtual ~StatusBarObserver();

  /// get called when a Warning is issued
  virtual void Warning(const char *m);
  /// get called when a Message is issued
  virtual void Message(const char * m);
  /// get called when a Error is issued
  virtual void Error  (const char *m);
  /// get called when a Log Message is issued
  virtual void Log    (const char *);
  /// name of the observer
  virtual const char *Name(void){return "StatusBar";}
};

} // namespace Gui

#endif // GUI_MAINWINDOW_H
