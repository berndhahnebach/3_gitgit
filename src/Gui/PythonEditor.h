/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <werner.wm.mayer@gmx.de>              *
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


#ifndef GUI_PYTHONEDITOR_H
#define GUI_PYTHONEDITOR_H

#ifndef __Qt4All__
# include "Qt4All.h"
#endif

#ifndef __Qt3All__
# include "Qt3All.h"
#endif

#include "MDIView.h"
#include "Window.h"
#include "TextEdit.h"

namespace Gui {

class PythonSyntaxHighlighter;
class PythonSyntaxHighlighterP;
class SyntaxLatex;

/**
 * Python text editor with syntax highlighting..
 * \author Werner Mayer
 */
class GuiExport PythonEditor : public TextEdit, public WindowParameter
{
  Q_OBJECT

public:
  PythonEditor(QWidget *parent = 0);
  ~PythonEditor();

  void OnChange( Base::Subject<const char*> &rCaller,const char* rcReason );

public Q_SLOTS:
  /** Inserts a '#' at the beginning of each selected line or the current line if 
   * nothing is selected
   */
  void onComment();
  /**
   * Removes the leading '#' from each selected line or the current line if
   * nothing is selected. In case a line hasn't a leading '#' then
   * this line is skipped.
   */
  void onUncomment();

protected:
  void keyPressEvent ( QKeyEvent * e );
  /** Pops up the context menu with some extensions */
  void contextMenuEvent ( QContextMenuEvent* e );

private:
  PythonSyntaxHighlighter* pythonSyntax;
  struct PythonEditorP* d;
};

/**
 * Syntax highlighter for Python.
 * \author Werner Mayer
 */
class GuiExport PythonSyntaxHighlighter : public QSyntaxHighlighter
{
public:
  PythonSyntaxHighlighter(QTextEdit* );
  virtual ~PythonSyntaxHighlighter();

  void highlightBlock ( const QString & text );
  int maximumUserState() const;
  
  void setColor( const QString& type, const QColor& col );
  QColor color( const QString& type );

protected:
  virtual void colorChanged( const QString& type, const QColor& col );

private:
  PythonSyntaxHighlighterP* d;
};

class LineMarker: public QWidget
{
    Q_OBJECT

public:
  LineMarker(TextEdit* textEdit, QWidget* parent, const char* name );
  virtual ~LineMarker();

public Q_SLOTS:
  void onRepaint() { repaint( FALSE ); }
  void setFont( QFont f );

protected:
  void resizeEvent( QResizeEvent* );
  void paintEvent ( QPaintEvent * );

private:
  TextEdit* _textEdit;
  QPixmap _buffer;
  QFont _font;
};

/**
 * A special view class which sends the messages from the application to
 * the Python editor and embeds the editor in a window.
 * \author Werner Mayer
 */
class GuiExport PythonEditView : public MDIView, public WindowParameter
{
  Q_OBJECT

public:
  PythonEditView(const QString& file, QWidget* parent);
  ~PythonEditView();

  void OnChange(Base::Subject<const char*> &rCaller,const char* rcReason);
  QTextEdit* editor() const { return _textEdit; }

  const char *getName(void) const {return "PythonEditView";}
  void onUpdate(void){};

  bool onMsg(const char* pMsg,const char** ppReturn);
  bool onHasMsg(const char* pMsg) const;

  bool canClose(void);
  void print();
  void openFile (const QString& fileName);

  /** @name Standard actions of the editor */
  //@{
  bool save   (void);
  bool saveAs (void);
  void cut    (void);
  void copy   (void);
  void paste  (void);
  void undo   (void);
  void redo   (void);
  void run    (void);
  //@}

  bool isSavedOnce();

  QStringList undoActions() const;
  QStringList redoActions() const;

private Q_SLOTS:
  void checkTimestamp();
  void onModified(bool);

private:
  void saveFile();

private:
  LineMarker* _lineMarker;
  QTextEdit* _textEdit;
  QString _fileName;
  QTimer*  _pcActivityTimer;
  uint _timeStamp;
};

} // namespace Gui

#endif // GUI_PYTHONEDITOR_H
