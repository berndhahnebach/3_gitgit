/***************************************************************************
 *   Copyright (c) 2011 J�rgen Riegel <juergen.riegel@web.de>              *
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
# include <QDialog>
#endif

#include <Gui/MainWindow.h>
#include <Base/Tools.h>

#include "ui_SketchOrientationDialog.h"
#include "SketchOrientationDialog.h"

using namespace SketcherGui;

SketchOrientationDialog::SketchOrientationDialog(void) 
{
    

}

SketchOrientationDialog::~SketchOrientationDialog()
{

}



int SketchOrientationDialog::exec()
{
    //Gui::MDIView *mdi = Gui::Application::Instance->activeDocument()->getActiveView();
    //Gui::View3DInventorViewer *viewer = static_cast<Gui::View3DInventor *>(mdi)->getViewer();

    QDialog dlg(Gui::getMainWindow());

    Ui::SketchOrientationDialog ui_SketchOrientationDialog;
    ui_SketchOrientationDialog.setupUi(&dlg);


    int res;
    if (res=dlg.exec()) {
        if(ui_SketchOrientationDialog.XY_radioButton->isChecked() ){
            Pos = Base::Placement(Base::Vector3d(0,0,ui_SketchOrientationDialog.Offset_doubleSpinBox->value()),Base::Rotation());
            DirType = 0;
        }else
        if(ui_SketchOrientationDialog.XZ_radioButton->isChecked() ){
            Pos = Base::Placement(Base::Vector3d(0,ui_SketchOrientationDialog.Offset_doubleSpinBox->value(),0),Base::Rotation(Base::Vector3d(1,0,0),-M_PI/2));
            DirType = 1;
        }else
        if(ui_SketchOrientationDialog.YZ_radioButton->isChecked() ){
            Pos = Base::Placement(Base::Vector3d(ui_SketchOrientationDialog.Offset_doubleSpinBox->value(),0,0),Base::Rotation(Base::Vector3d(0,1,0),M_PI/2));
            DirType = 2;
        }
    }

    return res;

}
