/***************************************************************************
 *   Copyright (c) 2009 J�rgen Riegel <juergen.riegel@web.de>              *
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


#ifndef GUI_TASKVIEW_TASKAPPERANCE_H
#define GUI_TASKVIEW_TASKAPPERANCE_H



#ifndef __Qt4All__
# include "Qt4All.h"
#endif

#include "TaskView.h"


class Ui_TaskAppearance;

namespace App {
class Property;
}

namespace Gui {
namespace TaskView {



class TaskAppearance : public TaskBox
{
    Q_OBJECT

public:
    TaskAppearance(QWidget *parent = 0);
    ~TaskAppearance();

protected:
    void changeEvent(QEvent *e);

private:
    QWidget* widget;
    Ui_TaskAppearance* ui;
};

} //namespace TaskView
} //namespace Gui

#endif // GUI_TASKVIEW_TASKAPPERANCE_H
