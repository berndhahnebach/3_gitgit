/* This file is part of the KDE project
   Copyright (C) 2003 Cedric Pasteur <cedric.pasteur@free.fr>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

/* Modifications for FreeCAD from 06-13-2004
    + use FreeCAD's export macro GuiExport instead of KEXICORE_EXPORT
    + use QUrl instead of KURL
    + use QPushButton instead of KPushButton
    + use QLineEdit instead of KLineEdit

   Modifications for FreeCAD from 10-19-2004
    + use namespace Gui::Kexi instead of prefix
*/

#ifndef PROPERTYEDITORFILE_H
#define PROPERTYEDITORFILE_H

#ifndef _PreComp_
# include <qurl.h>
#endif

#include "kexipropertysubeditor.h"

class QLineEdit;
class QPushButton;
class QResizeEvent;
class QLabel;

namespace Gui {
namespace Kexi {

class KexiProperty;

class GuiExport PropertyEditorFile : public PropertySubEditor
{
  Q_OBJECT

public:
  PropertyEditorFile(QWidget *parent, Property *property, const char *name=0);
  ~PropertyEditorFile() {;}

  virtual QVariant  value();
  virtual void  setValue(const QVariant &value);

  void setFilter(QString filter, bool add);
  void resizeEvent(QResizeEvent *ev);
  bool eventFilter(QObject* watched, QEvent* e);

protected slots:
  void selectFile();

protected:
  QLineEdit *m_lineedit;
  QPushButton *m_button;
  QUrl    m_url;
  QString   m_filter;
};


class GuiExport PropertyEditorPixmap : public PropertySubEditor
{
  Q_OBJECT

public:
  PropertyEditorPixmap(QWidget *parent, Property *property, const char *name=0);
  ~PropertyEditorPixmap();

  virtual QVariant  value();
  virtual void  setValue(const QVariant &value);
  virtual bool  eventFilter(QObject *o, QEvent *e);
  void  resizeEvent(QResizeEvent *ev);

protected slots:
  void selectFile();

private:
  QPushButton *m_button;
  QUrl    m_url;
  QLabel  *m_label;
  QLabel  *m_popup;
};

} // namespace Kexi
} // namespace Gui


#endif

