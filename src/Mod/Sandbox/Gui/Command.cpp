/***************************************************************************
 *   Copyright (c) 2009 Werner Mayer <wmayer@users.sourceforge.net>        *
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
# include <qobject.h>
#endif

#include <Base/Console.h>
#include <App/Application.h>
#include <App/Document.h>
#include <Gui/Application.h>
#include <Gui/Command.h>

#include <Mod/Sandbox/App/DocumentThread.h>


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//===========================================================================
// CmdSandboxTest THIS IS JUST A TEST COMMAND
//===========================================================================
DEF_STD_CMD(CmdSandboxTest);

CmdSandboxTest::CmdSandboxTest()
  :Command("Sandbox_Test")
{
    sAppModule    = "Sandbox";
    sGroup        = QT_TR_NOOP("Sandbox");
    sMenuText     = QT_TR_NOOP("Test thread");
    sToolTipText  = QT_TR_NOOP("Sandbox Test function");
    sWhatsThis    = QT_TR_NOOP("Sandbox Test function");
    sStatusTip    = QT_TR_NOOP("Sandbox Test function");
    sPixmap       = "Test1";
}

void CmdSandboxTest::activated(int iMsg)
{
    App::GetApplication().newDocument("Thread");
    Sandbox::DocumentThread* dt = new Sandbox::DocumentThread();
    QObject::connect(dt, SIGNAL(finished()), dt, SLOT(deleteLater()));
    dt->start();
}

void CreateSandboxCommands(void)
{
    Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();
    rcCmdMgr.addCommand(new CmdSandboxTest());
}
