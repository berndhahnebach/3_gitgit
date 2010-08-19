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
# include <QEventLoop>
# include <QMutex>
# include <QThread>
# include <QTimer>
# include <QMdiArea>
# include <QMdiSubWindow>
# include <QWaitCondition>
#endif

#include <Base/Console.h>
#include "Application.h"
#include "MainWindow.h"
#include "MDIView.h"
#include "Command.h"

#include "ProgressBar.h"


using namespace Gui;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//===========================================================================
// Std_Test1
//===========================================================================
DEF_STD_CMD_A(FCCmdTest1);

FCCmdTest1::FCCmdTest1()
  : Command("Std_Test1")
{
    sGroup        = "Standard-Test";
    sMenuText     = "Test1";
    sToolTipText  = "Test function 1";
    sWhatsThis    = sToolTipText;
    sStatusTip    = sToolTipText;
    sPixmap       = "Std_Tool1";
    iAccel        = Qt::CTRL+Qt::Key_T;
}

void FCCmdTest1::activated(int iMsg)
{

}

bool FCCmdTest1::isActive(void)
{
    //return (GetActiveOCCDocument()!=NULL);
    return true;
}

//===========================================================================
// Std_Test2
//===========================================================================
DEF_STD_CMD_A(FCCmdTest2);

FCCmdTest2::FCCmdTest2()
  : Command("Std_Test2")
{
    sGroup          = "Standard-Test";
    sMenuText       = "Test2";
    sToolTipText    = "Test function 2";
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Std_Tool2";
    iAccel          = 0;
}


void FCCmdTest2::activated(int iMsg)
{


}

bool FCCmdTest2::isActive(void)
{
    return (getDocument()!=NULL);
}

//===========================================================================
// Std_Test3
//===========================================================================
DEF_STD_CMD_A(FCCmdTest3);

FCCmdTest3::FCCmdTest3()
  : Command("Std_Test3")
{
    sGroup          = "Standard-Test";
    sMenuText       = "Test3";
    sToolTipText    = "Test function 3";
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Std_Tool3";
    iAccel          = 0;
}

void FCCmdTest3::activated(int iMsg)
{
    App::Document *pcDoc = getDocument();
    if (!pcDoc) return;
}


bool FCCmdTest3::isActive(void)
{
    return (getDocument()!=NULL);
}

//===========================================================================
// Std_Test4
//===========================================================================

DEF_STD_CMD_A(FCCmdTest4);

FCCmdTest4::FCCmdTest4()
  : Command("Std_Test4")
{
    sGroup          = "Standard-Test";
    sMenuText       = "Test4";
    sToolTipText    = "Test function 4";
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Std_Tool4";
    iAccel          = 0;
}

void FCCmdTest4::activated(int iMsg)
{
    App::Document *pcDoc = getDocument();
    if(!pcDoc) return;
}


bool FCCmdTest4::isActive(void)
{
    return (getDocument()!=NULL);
}

//===========================================================================
// Std_Test5
//===========================================================================
DEF_STD_CMD_A(FCCmdTest5);

FCCmdTest5::FCCmdTest5()
  : Command("Std_Test5")
{
    sGroup          = "Standard-Test";
    sMenuText       = "Test5";
    sToolTipText    = "Test function 5";
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Std_Tool5";
    iAccel          = 0;
}

void FCCmdTest5::activated(int iMsg)
{
    App::Document *pcDoc = getDocument();
    if(!pcDoc) return;
}

bool FCCmdTest5::isActive(void)
{
  return (getDocument()!=NULL);
}


//===========================================================================
// Std_Test6
//===========================================================================
DEF_STD_CMD_A(FCCmdTest6);

FCCmdTest6::FCCmdTest6()
  : Command("Std_Test6")
{
    sGroup          = "Standard-Test";
    sMenuText       = "Test6";
    sToolTipText    = "Test function 6";
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Std_Tool6";
    iAccel          = 0;
}

void FCCmdTest6::activated(int iMsg)
{
    App::Document *pcDoc = getDocument();
    if(!pcDoc) return;
}

bool FCCmdTest6::isActive(void)
{
    return (getDocument()!=NULL);
}

//===========================================================================
// Std_TestProgress1
//===========================================================================
DEF_STD_CMD_A(CmdTestProgress1);

CmdTestProgress1::CmdTestProgress1()
  : Command("Std_TestProgress1")
{
    sGroup          = "Standard-Test";
    sMenuText       = "Breakable bar";
    sToolTipText    = "Test a breakable progress bar";
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Std_Tool7";
    iAccel          = 0;
}

void CmdTestProgress1::activated(int iMsg)
{
    try
    {
        QMutex mutex;
        mutex.lock();
        unsigned long steps = 1000;
        Base::SequencerLauncher seq("Starting progress bar", steps);

        for (unsigned long i=0; i<steps;i++)
        {
            seq.next(true);
            QWaitCondition().wait(&mutex, 30);
        }

        mutex.unlock();
    }
    catch (...)
    {
    }
}

bool CmdTestProgress1::isActive(void)
{
    return (!Base::Sequencer().isRunning());
}

//===========================================================================
// Std_TestProgress2
//===========================================================================
DEF_STD_CMD_A(CmdTestProgress2);

CmdTestProgress2::CmdTestProgress2()
  : Command("Std_TestProgress2")
{
    sGroup          = "Standard-Test";
    sMenuText       = "Unbreakable bar";
    sToolTipText    = "Test a unbreakable progress bar";
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Std_Tool7";
    iAccel          = 0;
}

void CmdTestProgress2::activated(int iMsg)
{
    try
    {
        QMutex mutex;
        mutex.lock();
        unsigned long steps = 1000;
        Base::SequencerLauncher seq("Starting progress bar", steps);

        for (unsigned long i=0; i<steps;i++)
        {
            seq.next(false);
            QWaitCondition().wait(&mutex, 10);
        }
    }
    catch (...)
    {
    }
}

bool CmdTestProgress2::isActive(void)
{
    return ( !Base::Sequencer().isRunning() );
}

//===========================================================================
// Std_TestProgress3
//===========================================================================
DEF_STD_CMD_A(CmdTestProgress3);

CmdTestProgress3::CmdTestProgress3()
  : Command("Std_TestProgress3")
{
    sGroup          = "Standard-Test";
    sMenuText       = "Nested progress bar";
    sToolTipText    = "Test nested progress bar";
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Std_Tool8";
    iAccel          = 0;
}

void CmdTestProgress3::activated(int iMsg)
{
    try
    {
        // level 1
        QMutex mutex;
        mutex.lock();
        unsigned long steps = 5;
        Base::SequencerLauncher seq1("Starting progress bar", steps);
        for (unsigned long i=0; i<steps;i++)
        {
            QWaitCondition().wait(&mutex, 200);
            seq1.next(true);

            // level 2
            unsigned long steps = 6;
            Base::SequencerLauncher seq2("Starting progress bar", steps);
            for (unsigned long j=0; j<steps;j++)
            {
                QWaitCondition().wait(&mutex, 150);
                seq2.next(true);

                // level 3
                unsigned long steps = 7;
                Base::SequencerLauncher seq3("Starting progress bar", steps);
                for (unsigned long k=0; k<steps;k++)
                {
                    QWaitCondition().wait(&mutex, 100);
                    seq3.next(true);

                    // level 4
                    unsigned long steps = 8;
                    Base::SequencerLauncher seq4("Starting progress bar", steps);
                    for (unsigned long l=0; l<steps;l++)
                    {
                        QWaitCondition().wait(&mutex, 5);
                        seq4.next(true);
                    }
                }
            }
        }
    }
    catch (...)
    {
    }
}

bool CmdTestProgress3::isActive(void)
{
    return ( !Base::Sequencer().isRunning() );
}

//===========================================================================
// Std_TestProgress4
//===========================================================================
DEF_STD_CMD_A(CmdTestProgress4);

CmdTestProgress4::CmdTestProgress4()
  : Command("Std_TestProgress4")
{
    sGroup          = "Standard-Test";
    sMenuText       = "Mixed nested bar";
    sToolTipText    = "Test a mixed up nested progress bar";
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Std_Tool7";
    iAccel          = 0;
}

void CmdTestProgress4::activated(int iMsg)
{
    try
    {
        QMutex mutex;
        mutex.lock();
        unsigned long steps = 50;
        Base::SequencerLauncher* seq = new Base::SequencerLauncher("Starting progress bar", steps);

        for (unsigned long i=0; i<steps;i++)
        {
            QWaitCondition().wait(&mutex, 5);
            if (i == 45) {
                delete seq;
                seq = 0;
            }
            if (seq) {
                seq->next(false);
            }
            Base::SequencerLauncher seq2("Starting second progress bar", steps);
            for (unsigned long j=0; j<steps;j++)
            {
                QWaitCondition().wait(&mutex, (seq ? 5 : 50));
                seq2.next(true);
            }
        }
    }
    catch (...)
    {
    }
}

bool CmdTestProgress4::isActive(void)
{
    return (!Base::Sequencer().isRunning());
}

//===========================================================================
// Std_TestProgress5
//===========================================================================
DEF_STD_CMD_A(CmdTestProgress5);

CmdTestProgress5::CmdTestProgress5()
  : Command("Std_TestProgress5")
{
    sGroup          = "Standard-Test";
    sMenuText       = "From thread";
    sToolTipText    = "Test a progress bar from a thread";
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Std_Tool7";
    iAccel          = 0;
}

class BarThread : public QThread
{
public:
    BarThread(unsigned long s) : steps(s)
    {
    }
    ~BarThread()
    {
    }
    void run()
    {
        try
        {
            QMutex mutex;
            mutex.lock();
            Base::SequencerLauncher seq("Starting progress bar in thread", steps);

            for (unsigned long i=0; i<this->steps;i++)
            {
                seq.next(true);
                QWaitCondition().wait(&mutex, 5);
            }
            mutex.unlock();
        }
        catch (...)
        {
        }

        this->deleteLater();
        Base::Console().Message("Thread with %d steps finished\n",this->steps);
    }

private:
    unsigned long steps;
};

void CmdTestProgress5::activated(int iMsg)
{
    QEventLoop loop;

    BarThread* thr1 = new BarThread(2000);
    QObject::connect(thr1, SIGNAL(finished()), &loop, SLOT(quit()));
    thr1->start();
    loop.exec();

    BarThread* thr2 = new BarThread(1500);

    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    thr2->start();
    timer.start(2000); // 2s timeout
    loop.exec();

    BarThread* thr3 = new BarThread(1000);
    thr3->start();
}

bool CmdTestProgress5::isActive(void)
{
    return (!Base::Sequencer().isRunning());
}

//===========================================================================
// Std_MDITest
//===========================================================================
DEF_STD_CMD_A(CmdTestMDI1);

CmdTestMDI1::CmdTestMDI1()
  : Command("Std_MDITest1")
{
    sGroup          = "Standard-Test";
    sMenuText       = "Remove MDI 1";
    sToolTipText    = "Remove MDI from main window";
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    iAccel          = 0;
}

void CmdTestMDI1::activated(int iMsg)
{
    MDIView* mdi = getMainWindow()->activeWindow();
    getMainWindow()->removeWindow(mdi);
}

bool CmdTestMDI1::isActive(void)
{
    return getMainWindow()->activeWindow();
}

DEF_STD_CMD_A(CmdTestMDI2);

CmdTestMDI2::CmdTestMDI2()
  : Command("Std_MDITest2")
{
    sGroup          = "Standard-Test";
    sMenuText       = "Remove MDI 2";
    sToolTipText    = "Remove view from MDI area";
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    iAccel          = 0;
}

void CmdTestMDI2::activated(int iMsg)
{
    QMdiArea* area = getMainWindow()->findChild<QMdiArea*>();
    if (area) {
        MDIView* mdi = getMainWindow()->activeWindow();
        area->removeSubWindow(mdi->parentWidget());
        mdi->parentWidget()->showNormal();
    }
}

bool CmdTestMDI2::isActive(void)
{
    return getMainWindow()->activeWindow();
}

DEF_STD_CMD_A(CmdTestMDI3);

CmdTestMDI3::CmdTestMDI3()
  : Command("Std_MDITest3")
{
    sGroup          = "Standard-Test";
    sMenuText       = "Remove MDI 3";
    sToolTipText    = "Unset parent and remove from main window";
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    iAccel          = 0;
}

void CmdTestMDI3::activated(int iMsg)
{
    MDIView* mdi = getMainWindow()->activeWindow();
    getMainWindow()->removeWindow(mdi);
    mdi->setParent(0, Qt::Window | Qt::WindowTitleHint |
                   Qt::WindowSystemMenuHint | 
                   Qt::WindowMinMaxButtonsHint);
    mdi->show();
}

bool CmdTestMDI3::isActive(void)
{
    return getMainWindow()->activeWindow();
}


namespace Gui {

void CreateTestCommands(void)
{
    CommandManager &rcCmdMgr = Application::Instance->commandManager();

    rcCmdMgr.addCommand(new FCCmdTest1());
    rcCmdMgr.addCommand(new FCCmdTest2());
    rcCmdMgr.addCommand(new FCCmdTest3());
    rcCmdMgr.addCommand(new FCCmdTest4());
    rcCmdMgr.addCommand(new FCCmdTest5());
    rcCmdMgr.addCommand(new FCCmdTest6());
    rcCmdMgr.addCommand(new CmdTestProgress1());
    rcCmdMgr.addCommand(new CmdTestProgress2());
    rcCmdMgr.addCommand(new CmdTestProgress3());
    rcCmdMgr.addCommand(new CmdTestProgress4());
    rcCmdMgr.addCommand(new CmdTestProgress5());
    rcCmdMgr.addCommand(new CmdTestMDI1());
    rcCmdMgr.addCommand(new CmdTestMDI2());
    rcCmdMgr.addCommand(new CmdTestMDI3());
}

} // namespace Gui
