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


#ifndef GUI_DIALOG_DLGTIPOFTHEDAY_IMP_H
#define GUI_DIALOG_DLGTIPOFTHEDAY_IMP_H

#include "ui_DlgTipOfTheDay.h"
#include "Window.h"

#ifndef __Qt4All__
# include "Qt4All.h"
#endif

#ifndef __Qt3All__
# include "Qt3All.h"
#endif

namespace Gui {
namespace Dialog {

/** Implementation of the well-known Tip-of-the-day dialog.
 *
 * All the tips are read from the Tip-of-the-day site located in the documentation
 * directory of this installation.
 *
 * To download the latest online documentation you just have to press 
 * "Download online help" in the help menu. If you cannot compile these sources it is
 * also possible to use the program \a wget specifying the options -r, -k, -E and the
 * URL of the homepage of FreeCAD.
 *
 * For more details have a look at the FreeCAD homepage (http://freecad.juergen-riegel.net/Docu/).
 *
 * You can simply append a new tip by clicking the "EditText" hyperlink
 * and writing a text, e.g: "** this is a new tip"
 *
 * \author Werner Mayer
 */
class DlgTipOfTheDayImp : public QDialog, public Ui_DlgTipOfTheDay, public WindowParameter
{
  Q_OBJECT

public:
  DlgTipOfTheDayImp( QWidget* parent = 0, Qt::WFlags fl = 0 );
  ~DlgTipOfTheDayImp();

  void reload();

public Q_SLOTS:
  void on_buttonNextTip_clicked();

private:
  QStringList _lTips;
  int _iCurrentTip;
};

} // namespace Dialog
} // namespace Gui

#endif // GUI_DIALOG_DLGTIPOFTHEDAY_IMP_H
