/***************************************************************************
 *   Copyright (c) J�rgen Riegel          (juergen.riegel@web.de) 2002     *
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
# include <QDir>
# include <QFileInfo>
# include <QLineEdit>
# include <QPointer>
# include <Standard_math.hxx>
# include <Inventor/events/SoMouseButtonEvent.h>
#endif

#include <Base/Exception.h>
#include <App/Document.h>
#include <Gui/Application.h>
#include <Gui/BitmapFactory.h>
#include <Gui/Command.h>
#include <Gui/Document.h>
#include <Gui/FileDialog.h>
#include <Gui/MainWindow.h>
#include <Gui/Selection.h>
#include <Gui/View3DInventor.h>
#include <Gui/View3DInventorViewer.h>

#include "../App/PartFeature.h"
#include "DlgPartImportStepImp.h"
#include "DlgBooleanOperation.h"
#include "DlgExtrusion.h"
#include "DlgFilletEdges.h"
#include "DlgPrimitives.h"
#include "ViewProvider.h"


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//===========================================================================
// Part_PickCurveNet
//===========================================================================
DEF_STD_CMD(CmdPartPickCurveNet);

CmdPartPickCurveNet::CmdPartPickCurveNet()
  :Command("Part_PickCurveNet")
{
    sAppModule    = "Part";
    sGroup        = QT_TR_NOOP("Part");
    sMenuText     = QT_TR_NOOP("Pick curve network");
    sToolTipText  = QT_TR_NOOP("Pick a curve network");
    sWhatsThis    = sToolTipText;
    sStatusTip    = sToolTipText;
    sPixmap       = "Test1";
    iAccel        = 0;
}

void CmdPartPickCurveNet::activated(int iMsg)
{

}

//===========================================================================
// Part_NewDoc
//===========================================================================
DEF_STD_CMD(CmdPartNewDoc);

CmdPartNewDoc::CmdPartNewDoc()
  :Command("Part_NewDoc")
{
    sAppModule    = "Part";
    sGroup        = "Part";
    sMenuText     = "New document";
    sToolTipText  = "Create an empty part document";
    sWhatsThis    = sToolTipText;
    sStatusTip    = sToolTipText;
    sPixmap       = "New";
    iAccel        = 0;
}

void CmdPartNewDoc::activated(int iMsg)
{
    doCommand(Doc,"d = App.New()");
    updateActive();
}

//===========================================================================
// Part_Box2
//===========================================================================
DEF_STD_CMD_A(CmdPartBox2);

CmdPartBox2::CmdPartBox2()
  :Command("Part_Box2")
{
    sAppModule    = "Part";
    sGroup        = QT_TR_NOOP("Part");
    sMenuText     = QT_TR_NOOP("Box fix 1");
    sToolTipText  = QT_TR_NOOP("Create a box feature without dialog");
    sWhatsThis    = "Part_Box2";
    sStatusTip    = sToolTipText;
    sPixmap       = "Part_Box";
    iAccel        = 0;
}


void CmdPartBox2::activated(int iMsg)
{
    openCommand("Part Box Create");
    doCommand(Doc,"from FreeCAD import Base");
    doCommand(Doc,"import Part");
    doCommand(Doc,"__fb__ = App.ActiveDocument.addObject(\"Part::Box\",\"PartBox\")");
    doCommand(Doc,"__fb__.Location = Base.Vector(0.0,0.0,0.0)");
    doCommand(Doc,"__fb__.Length = 100.0");
    doCommand(Doc,"__fb__.Width = 100.0");
    doCommand(Doc,"__fb__.Height = 100.0");
    doCommand(Doc,"del __fb__");
    commitCommand();
    updateActive();
}

bool CmdPartBox2::isActive(void)
{
    if (getActiveGuiDocument())
        return true;
    else
        return false;
}

//===========================================================================
// Part_Box3
//===========================================================================
DEF_STD_CMD_A(CmdPartBox3);

CmdPartBox3::CmdPartBox3()
  :Command("Part_Box3")
{
    sAppModule    = "Part";
    sGroup        = QT_TR_NOOP("Part");
    sMenuText     = QT_TR_NOOP("Box fix 2");
    sToolTipText  = QT_TR_NOOP("Create a box feature without dialog");
    sWhatsThis    = "Part_Box3";
    sStatusTip    = sToolTipText;
    sPixmap       = "Part_Box";
    iAccel        = 0;
}

void CmdPartBox3::activated(int iMsg)
{
    openCommand("Part Box Create");
    doCommand(Doc,"from FreeCAD import Base");
    doCommand(Doc,"import Part");
    doCommand(Doc,"__fb__ = App.ActiveDocument.addObject(\"Part::Box\",\"PartBox\")");
    doCommand(Doc,"__fb__.Location = Base.Vector(50.0,50.0,50.0)");
    doCommand(Doc,"__fb__.Length = 100.0");
    doCommand(Doc,"__fb__.Width = 100.0");
    doCommand(Doc,"__fb__.Height = 100.0");
    doCommand(Doc,"del __fb__");
    commitCommand();
    updateActive();
}

bool CmdPartBox3::isActive(void)
{
    if (getActiveGuiDocument())
        return true;
    else
        return false;
}

//===========================================================================
// Part_Primitives
//===========================================================================
DEF_STD_CMD_A(CmdPartPrimitives);

CmdPartPrimitives::CmdPartPrimitives()
  :Command("Part_Primitives")
{
    sAppModule    = "Part";
    sGroup        = QT_TR_NOOP("Part");
    sMenuText     = QT_TR_NOOP("Create primitives...");
    sToolTipText  = QT_TR_NOOP("Creation of parametrized geometric primitives");
    sWhatsThis    = "Part_Primitives";
    sStatusTip    = sToolTipText;
    //sPixmap       = "Part_Box";
    iAccel        = 0;
}

void CmdPartPrimitives::activated(int iMsg)
{
    static QPointer<QDialog> dlg = 0;
    if (!dlg)
        dlg = new PartGui::DlgPrimitives(Gui::getMainWindow());
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->show();
}

bool CmdPartPrimitives::isActive(void)
{
    return hasActiveDocument();
}

//===========================================================================
// Part_Cut
//===========================================================================
DEF_STD_CMD_A(CmdPartCut);

CmdPartCut::CmdPartCut()
  :Command("Part_Cut")
{
    sAppModule    = "Part";
    sGroup        = QT_TR_NOOP("Part");
    sMenuText     = QT_TR_NOOP("Cut");
    sToolTipText  = QT_TR_NOOP("Create a Cut feature");
    sWhatsThis    = "Part_Cut";
    sStatusTip    = sToolTipText;
    sPixmap       = "Part_Cut";
    iAccel        = 0;
}


void CmdPartCut::activated(int iMsg)
{
    unsigned int n = getSelection().countObjectsOfType(Part::Feature::getClassTypeId());
    if (n != 2) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select two shapes please."));
        return;
    }

    std::string FeatName = getUniqueObjectName("Cut");

    std::vector<Gui::SelectionSingleton::SelObj> Sel = getSelection().getSelection();

    openCommand("Part Cut");
    doCommand(Doc,"App.activeDocument().addObject(\"Part::Cut\",\"%s\")",FeatName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Base = App.activeDocument().%s",FeatName.c_str(),Sel[0].FeatName);
    doCommand(Doc,"App.activeDocument().%s.Tool = App.activeDocument().%s",FeatName.c_str(),Sel[1].FeatName);
    doCommand(Gui,"Gui.activeDocument().hide(\"%s\")",Sel[0].FeatName);
    doCommand(Gui,"Gui.activeDocument().hide(\"%s\")",Sel[1].FeatName);
    updateActive();
    commitCommand();
}

bool CmdPartCut::isActive(void)
{
    return getSelection().countObjectsOfType(Part::Feature::getClassTypeId())==2;
}

//===========================================================================
// Part_Common
//===========================================================================
DEF_STD_CMD_A(CmdPartCommon);

CmdPartCommon::CmdPartCommon()
  :Command("Part_Common")
{
    sAppModule    = "Part";
    sGroup        = QT_TR_NOOP("Part");
    sMenuText     = QT_TR_NOOP("Intersection");
    sToolTipText  = QT_TR_NOOP("Intersection of two shapes");
    sWhatsThis    = "Part_Common";
    sStatusTip    = sToolTipText;
    sPixmap       = "Part_Common";
    iAccel        = 0;
}

void CmdPartCommon::activated(int iMsg)
{
    unsigned int n = getSelection().countObjectsOfType(Part::Feature::getClassTypeId());
    if (n != 2) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select two shapes please."));
        return;
    }

    std::string FeatName = getUniqueObjectName("Common");

    std::vector<Gui::SelectionSingleton::SelObj> Sel = getSelection().getSelection();

    openCommand("Common");
    doCommand(Doc,"App.activeDocument().addObject(\"Part::Common\",\"%s\")",FeatName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Base = App.activeDocument().%s",FeatName.c_str(),Sel[0].FeatName);
    doCommand(Doc,"App.activeDocument().%s.Tool = App.activeDocument().%s",FeatName.c_str(),Sel[1].FeatName);
    doCommand(Gui,"Gui.activeDocument().hide(\"%s\")",Sel[0].FeatName);
    doCommand(Gui,"Gui.activeDocument().hide(\"%s\")",Sel[1].FeatName);
    updateActive();
    commitCommand();
}

bool CmdPartCommon::isActive(void)
{
    return getSelection().countObjectsOfType(Part::Feature::getClassTypeId())==2;
}

//===========================================================================
// Part_Fuse
//===========================================================================
DEF_STD_CMD_A(CmdPartFuse);

CmdPartFuse::CmdPartFuse()
  :Command("Part_Fuse")
{
    sAppModule    = "Part";
    sGroup        = QT_TR_NOOP("Part");
    sMenuText     = QT_TR_NOOP("Union");
    sToolTipText  = QT_TR_NOOP("Make union of two shapes");
    sWhatsThis    = "Part_Fuse";
    sStatusTip    = sToolTipText;
    sPixmap       = "Part_Fuse";
    iAccel        = 0;
}


void CmdPartFuse::activated(int iMsg)
{
    unsigned int n = getSelection().countObjectsOfType(Part::Feature::getClassTypeId());
    if (n != 2) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select two shapes please."));
        return;
    }

    std::string FeatName = getUniqueObjectName("Fusion");

    std::vector<Gui::SelectionSingleton::SelObj> Sel = getSelection().getSelection();

    openCommand("Fusion");
    doCommand(Doc,"App.activeDocument().addObject(\"Part::Fuse\",\"%s\")",FeatName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Base = App.activeDocument().%s",FeatName.c_str(),Sel[0].FeatName);
    doCommand(Doc,"App.activeDocument().%s.Tool = App.activeDocument().%s",FeatName.c_str(),Sel[1].FeatName);
    doCommand(Gui,"Gui.activeDocument().hide(\"%s\")",Sel[0].FeatName);
    doCommand(Gui,"Gui.activeDocument().hide(\"%s\")",Sel[1].FeatName);
    updateActive();
    commitCommand();
}

bool CmdPartFuse::isActive(void)
{
    return getSelection().countObjectsOfType(Part::Feature::getClassTypeId())==2;
}

//===========================================================================
// Part_Section
//===========================================================================
DEF_STD_CMD_A(CmdPartSection);

CmdPartSection::CmdPartSection()
  :Command("Part_Section")
{
    sAppModule    = "Part";
    sGroup        = QT_TR_NOOP("Part");
    sMenuText     = QT_TR_NOOP("Section");
    sToolTipText  = QT_TR_NOOP("Make section of two shapes");
    sWhatsThis    = "Part_Section";
    sStatusTip    = sToolTipText;
    sPixmap       = "Part_Section";
    iAccel        = 0;
}


void CmdPartSection::activated(int iMsg)
{
    unsigned int n = getSelection().countObjectsOfType(Part::Feature::getClassTypeId());
    if (n != 2) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select two shapes please."));
        return;
    }

    std::string FeatName = getUniqueObjectName("Section");

    std::vector<Gui::SelectionSingleton::SelObj> Sel = getSelection().getSelection();

    openCommand("Section");
    doCommand(Doc,"App.activeDocument().addObject(\"Part::Section\",\"%s\")",FeatName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Base = App.activeDocument().%s",FeatName.c_str(),Sel[0].FeatName);
    doCommand(Doc,"App.activeDocument().%s.Tool = App.activeDocument().%s",FeatName.c_str(),Sel[1].FeatName);
    doCommand(Gui,"Gui.activeDocument().hide(\"%s\")",Sel[0].FeatName);
    doCommand(Gui,"Gui.activeDocument().hide(\"%s\")",Sel[1].FeatName);
    updateActive();
    commitCommand();
}

bool CmdPartSection::isActive(void)
{
    return getSelection().countObjectsOfType(Part::Feature::getClassTypeId())==2;
}

//===========================================================================
// CmdPartImport
//===========================================================================
DEF_STD_CMD_A(CmdPartImport);

CmdPartImport::CmdPartImport()
  :Command("Part_Import")
{
    sAppModule    = "Part";
    sGroup        = QT_TR_NOOP("Part");
    sMenuText     = QT_TR_NOOP("Import CAD...");
    sToolTipText  = QT_TR_NOOP("Imports a CAD file");
    sWhatsThis    = "Part_Import";
    sStatusTip    = sToolTipText;
    sPixmap       = "Part_Box";
    iAccel        = 0;
}


void CmdPartImport::activated(int iMsg)
{
    QStringList filter;
    filter << QObject::tr("All CAD Files (*.stp *.step *.igs *.iges *.brp *.brep)");
    filter << QObject::tr("STEP (*.stp *.step)");
    filter << QObject::tr("IGES (*.igs *.iges)");
    filter << QObject::tr("BREP (*.brp *.brep)");
    filter << QObject::tr("All Files (*.*)");

    QString fn = Gui::FileDialog::getOpenFileName(Gui::getMainWindow(), QString(), QString(), filter.join(QLatin1String(";;")));
    if (!fn.isEmpty()) {
        App::Document* pDoc = getDocument();
        if (!pDoc) return; // no document
        openCommand("Part Import Create");
        doCommand(Doc, "import Part");
        doCommand(Doc, "Part.insert(\"%s\",\"%s\")", (const char*)fn.toAscii(), pDoc->getName());
        commitCommand();
    }
}

bool CmdPartImport::isActive(void)
{
    if (getActiveGuiDocument())
        return true;
    else
        return false;
}

//===========================================================================
// PartImportCurveNet
//===========================================================================
DEF_STD_CMD_A(CmdPartImportCurveNet);

CmdPartImportCurveNet::CmdPartImportCurveNet()
  :Command("Part_ImportCurveNet")
{
    sAppModule  = "Part";
    sGroup      = QT_TR_NOOP("Part");
    sMenuText   = QT_TR_NOOP("Import curve network...");
    sToolTipText= QT_TR_NOOP("Import a curve network");
    sWhatsThis  = "Part_ImportCurveNet";
    sStatusTip  = sToolTipText;
    sPixmap     = "Part_Box";
    iAccel      = 0;
}

void CmdPartImportCurveNet::activated(int iMsg)
{
    QStringList filter;
    filter << QObject::tr("All CAD Files (*.stp *.step *.igs *.iges *.brp *.brep)");
    filter << QObject::tr("STEP (*.stp *.step)");
    filter << QObject::tr("IGES (*.igs *.iges)");
    filter << QObject::tr("BREP (*.brp *.brep)");
    filter << QObject::tr("All Files (*.*)");

    QString fn = Gui::FileDialog::getOpenFileName(Gui::getMainWindow(), QString(), QString(), filter.join(QLatin1String(";;")));
    if (!fn.isEmpty()) {
        QFileInfo fi; fi.setFile(fn);
        openCommand("Part Import Curve Net");
        doCommand(Doc,"f = App.activeDocument().addObject(\"Part::CurveNet\",\"%s\")", (const char*)fi.baseName().toAscii());
        doCommand(Doc,"f.FileName = \"%s\"",(const char*)fn.toAscii());
        commitCommand();
        updateActive();
    }
}

bool CmdPartImportCurveNet::isActive(void)
{
    if (getActiveGuiDocument())
        return true;
    else
        return false;
}

//===========================================================================
// Part_Boolean
//===========================================================================
DEF_STD_CMD_A(CmdPartBoolean);

CmdPartBoolean::CmdPartBoolean()
  :Command("Part_Boolean")
{
    sAppModule    = "Part";
    sGroup        = QT_TR_NOOP("Part");
    sMenuText     = QT_TR_NOOP("Boolean...");
    sToolTipText  = QT_TR_NOOP("Run a boolean operation with two shapes selected");
    sWhatsThis    = "Part_Boolean";
    sStatusTip    = sToolTipText;
    sPixmap       = "Part_Booleans";
    iAccel        = 0;
}

void CmdPartBoolean::activated(int iMsg)
{
    PartGui::DlgBooleanOperation dlg(Gui::getMainWindow());
    dlg.exec();
}

bool CmdPartBoolean::isActive(void)
{
    return hasActiveDocument();
}

//===========================================================================
// Part_Extrude
//===========================================================================
DEF_STD_CMD_A(CmdPartExtrude);

CmdPartExtrude::CmdPartExtrude()
  :Command("Part_Extrude")
{
    sAppModule    = "Part";
    sGroup        = QT_TR_NOOP("Part");
    sMenuText     = QT_TR_NOOP("Extrude...");
    sToolTipText  = QT_TR_NOOP("Extrude a selected sketch");
    sWhatsThis    = sToolTipText;
    sStatusTip    = sToolTipText;
    sPixmap       = "Part_Extrude";
    iAccel        = 0;
}

void CmdPartExtrude::activated(int iMsg)
{
    PartGui::DlgExtrusion dlg(Gui::getMainWindow());
    dlg.exec();
}

bool CmdPartExtrude::isActive(void)
{
    return hasActiveDocument();
}

//===========================================================================
// Part_Revolve
//===========================================================================
DEF_STD_CMD_A(CmdPartRevolve);

CmdPartRevolve::CmdPartRevolve()
  :Command("Part_Revolve")
{
    sAppModule    = "Part";
    sGroup        = QT_TR_NOOP("Part");
    sMenuText     = QT_TR_NOOP("Revolve...");
    sToolTipText  = QT_TR_NOOP("Revolve a selected shape");
    sWhatsThis    = sToolTipText;
    sStatusTip    = sToolTipText;
    sPixmap       = "Part_Revolve";
    iAccel        = 0;
}

void CmdPartRevolve::activated(int iMsg)
{
    QMessageBox::warning(Gui::getMainWindow(), QObject::tr("No yet implemented"), QObject::tr("No yet implemented"));
}

bool CmdPartRevolve::isActive(void)
{
    return hasActiveDocument();
}

//===========================================================================
// Part_FilletEdges
//===========================================================================
DEF_STD_CMD_A(CmdPartFilletEdges);

CmdPartFilletEdges::CmdPartFilletEdges()
  :Command("Part_Fillet")
{
    sAppModule    = "Part";
    sGroup        = QT_TR_NOOP("Part");
    sMenuText     = QT_TR_NOOP("Fillet...");
    sToolTipText  = QT_TR_NOOP("Fillet the selected edges of a shape");
    sWhatsThis    = sToolTipText;
    sStatusTip    = sToolTipText;
    sPixmap       = "Part_Fillet";
    iAccel        = 0;
}

void CmdPartFilletEdges::activated(int iMsg)
{
    PartGui::DlgFilletEdges dlg(Gui::getMainWindow());
    dlg.exec();
}

bool CmdPartFilletEdges::isActive(void)
{
    return hasActiveDocument();
}

//--------------------------------------------------------------------------------------

DEF_STD_CMD_A(CmdShapeInfo);

CmdShapeInfo::CmdShapeInfo()
  :Command("Part_ShapeInfo")
{
    sAppModule    = "Part";
    sGroup        = "Part";
    sMenuText     = "Shape info...";
    sToolTipText  = "Info about shape";
    sWhatsThis    = sToolTipText;
    sStatusTip    = sToolTipText;
}

void CmdShapeInfo::activated(int iMsg)
{
    static const char * const part_pipette[]={
        "32 32 17 1",
        "# c #000000",
        "j c #080808",
        "b c #101010",
        "f c #1c1c1c",
        "g c #4c4c4c",
        "c c #777777",
        "a c #848484",
        "i c #9c9c9c",
        "l c #b9b9b9",
        "e c #cacaca",
        "n c #d6d6d6",
        "k c #dedede",
        "d c #e7e7e7",
        "m c #efefef",
        "h c #f7f7f7",
        "w c #ffffff",
        ". c None",
        "................................",
        ".....................#####......",
        "...................#######......",
        "...................#########....",
        "..................##########....",
        "..................##########....",
        "..................##########....",
        ".................###########....",
        "...............#############....",
        ".............###############....",
        ".............#############......",
        ".............#############......",
        "...............ab######.........",
        "..............cdef#####.........",
        ".............ghdacf####.........",
        "............#ehiacj####.........",
        "............awiaaf####..........",
        "...........iheacf##.............",
        "..........#kdaag##..............",
        ".........gedaacb#...............",
        ".........lwiac##................",
        ".......#amlaaf##................",
        ".......cheaag#..................",
        "......#ndaag##..................",
        ".....#imaacb#...................",
        ".....iwlacf#....................",
        "....#nlaag##....................",
        "....feaagj#.....................",
        "....caag##......................",
        "....ffbj##......................",
        "................................",
        "................................"};

    Gui::Document* doc = Gui::Application::Instance->activeDocument();
    Gui::View3DInventor* view = static_cast<Gui::View3DInventor*>(doc->getActiveView());
    if (view) {
        Gui::View3DInventorViewer* viewer = view->getViewer();
        viewer->setEditing(true);
        viewer->getWidget()->setCursor(QCursor(QPixmap(part_pipette),4,29));
        viewer->addEventCallback(SoMouseButtonEvent::getClassTypeId(), PartGui::ViewProviderPart::shapeInfoCallback);
     }
}

bool CmdShapeInfo::isActive(void)
{
    App::Document* doc = App::GetApplication().getActiveDocument();
    if (!doc || doc->countObjectsOfType(Part::Feature::getClassTypeId()) == 0)
        return false;

    Gui::MDIView* view = Gui::getMainWindow()->activeWindow();
    if (view && view->isDerivedFrom(Gui::View3DInventor::getClassTypeId())) {
        Gui::View3DInventorViewer* viewer = static_cast<Gui::View3DInventor*>(view)->getViewer();
        return !viewer->isEditing();
    }

    return false;
}


void CreatePartCommands(void)
{
    Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();

    rcCmdMgr.addCommand(new CmdPartBoolean());
    rcCmdMgr.addCommand(new CmdPartExtrude());
    rcCmdMgr.addCommand(new CmdPartRevolve());
    rcCmdMgr.addCommand(new CmdPartFilletEdges());
    rcCmdMgr.addCommand(new CmdPartCommon());
    rcCmdMgr.addCommand(new CmdPartCut());
    rcCmdMgr.addCommand(new CmdPartFuse());
    rcCmdMgr.addCommand(new CmdPartSection());
    //rcCmdMgr.addCommand(new CmdPartBox2());
    //rcCmdMgr.addCommand(new CmdPartBox3());
    rcCmdMgr.addCommand(new CmdPartPrimitives());

    rcCmdMgr.addCommand(new CmdPartImport());
    rcCmdMgr.addCommand(new CmdPartImportCurveNet());
    rcCmdMgr.addCommand(new CmdPartPickCurveNet());
    rcCmdMgr.addCommand(new CmdShapeInfo());
} 

