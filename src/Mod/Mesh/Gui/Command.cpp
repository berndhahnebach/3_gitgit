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


#include "PreCompiled.h"
#ifndef _PreComp_
# include <qaction.h>
# include <qdir.h>
# include <qfileinfo.h>
# include <qinputdialog.h>
# include <qmessagebox.h>
# include <qstringlist.h>
# include <gts.h>
# include <map>
#endif

#include "../App/MeshFeature.h"
#include "../App/MeshAlgos.h"

#include <Base/Console.h>
#include <Base/Exception.h>
#include <App/Document.h>
#include <App/Feature.h>
#include <Gui/Application.h>
#include <Gui/MainWindow.h>
#include <Gui/Command.h>
#include <Gui/Document.h>
#include <Gui/FileDialog.h>
#include <Gui/Selection.h>
#include <Gui/ViewProvider.h>
#include <Gui/View3DInventor.h>
#include <Gui/View3DInventorViewer.h>

#include "DlgEvaluateMeshImp.h"
#include "DlgRegularSolidImp.h"

using namespace Mesh;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//===========================================================================
// Mesh transform
//===========================================================================
DEF_STD_CMD_A(CmdMeshTransform);

CmdMeshTransform::CmdMeshTransform()
  :Command("Mesh_Transform")
{
  sAppModule    = "Mesh";
  sGroup        = "Mesh";
  sMenuText     = QT_TR_NOOP("Transformation (moving) of a mesh");
  sToolTipText  = sMenuText;
  sWhatsThis    = sMenuText;
  sStatusTip    = sMenuText;
  sPixmap       = "Std_Tool1";
  iAccel        = 0;
}

void CmdMeshTransform::activated(int iMsg)
{
  unsigned int n = getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId());
  if ( n!=1 ) return;

  std::string fName = getUniqueObjectName("Move");
  std::vector<Gui::SelectionSingleton::SelObj> cSel = getSelection().getSelection();

  openCommand("Mesh Mesh Create");
  doCommand(Doc,"App.document().addObject(\"Mesh::Transform\",\"%s\")",fName.c_str());
  doCommand(Doc,"App.document().%s.Source = App.document().%s",fName.c_str(),cSel[0].FeatName);
  doCommand(Gui,"Gui.hide(\"%s\")",cSel[0].FeatName);
  commitCommand(); 
 
  updateActive();
}

bool CmdMeshTransform::isActive(void)
{
  //return true;
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) == 1;
}

//===========================================================================
// Mesh demolding
//===========================================================================
DEF_STD_CMD_A(CmdMeshDemolding);

CmdMeshDemolding::CmdMeshDemolding()
  :Command("Mesh_Demolding")
{
  sAppModule    = "Mesh";
  sGroup        = "Mesh";
  sMenuText     = QT_TR_NOOP("Interactive demolding direction");
  sToolTipText  = sMenuText;
  sWhatsThis    = sMenuText;
  sStatusTip    = sMenuText;
  sPixmap       = "Std_Tool1";
  iAccel        = 0;
}

void CmdMeshDemolding::activated(int iMsg)
{
  unsigned int n = getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId());
  if ( n!=1 ) return;

  std::string fName = getUniqueObjectName("Demolding");
  std::vector<Gui::SelectionSingleton::SelObj> cSel = getSelection().getSelection();

  openCommand("Mesh Mesh Create");
  doCommand(Doc,"App.document().addObject(\"Mesh::TransformDemolding\",\"%s\")",fName.c_str());
  doCommand(Doc,"App.document().%s.Source = App.document().%s",fName.c_str(),cSel[0].FeatName);
  doCommand(Gui,"Gui.hide(\"%s\")",cSel[0].FeatName);
  commitCommand(); 
 
  updateActive();
}

bool CmdMeshDemolding::isActive(void)
{
  //return true;
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) == 1;
}
//===========================================================================
// Example MakeMesh
//===========================================================================
DEF_STD_CMD_A(CmdMeshExMakeMesh);

CmdMeshExMakeMesh::CmdMeshExMakeMesh()
  :Command("Mesh_ExMakeMesh")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Make a Box");
  sToolTipText  = sMenuText;
  sWhatsThis    = sMenuText;
  sStatusTip    = sMenuText;
  sPixmap       = "Std_Tool1";
  iAccel        = 0;
}

void CmdMeshExMakeMesh::activated(int iMsg)
{
  openCommand("Mesh Mesh Create");
  doCommand(Doc,
    "import Mesh\n"
    "mb = Mesh.newMesh()\n"
    "mb.addFacet(0.0,0.0,0.0, 0.0,0.0,1.0, 0.0,1.0,1.0)\n"
    "mb.addFacet(0.0,0.0,0.0, 0.0,1.0,1.0, 0.0,1.0,0.0)\n"
    "mb.addFacet(0.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,1.0)\n"
    "mb.addFacet(0.0,0.0,0.0, 1.0,0.0,1.0, 0.0,0.0,1.0)\n"
    "mb.addFacet(0.0,0.0,0.0, 0.0,1.0,0.0, 1.0,1.0,0.0)\n"
    "mb.addFacet(0.0,0.0,0.0, 1.0,1.0,0.0, 1.0,0.0,0.0)\n"
    "mb.addFacet(0.0,1.0,0.0, 0.0,1.0,1.0, 1.0,1.0,1.0)\n"
    "mb.addFacet(0.0,1.0,0.0, 1.0,1.0,1.0, 1.0,1.0,0.0)\n"
    "mb.addFacet(0.0,1.0,1.0, 0.0,0.0,1.0, 1.0,0.0,1.0)\n"
    "mb.addFacet(0.0,1.0,1.0, 1.0,0.0,1.0, 1.0,1.0,1.0)\n"
    "mb.addFacet(1.0,1.0,0.0, 1.0,1.0,1.0, 1.0,0.0,1.0)\n"
    "mb.addFacet(1.0,1.0,0.0, 1.0,0.0,1.0, 1.0,0.0,0.0)\n"
    "mb.scale(100.0)\n"
    "App.document().addObject(\"Mesh::Feature\",\"MeshBox\")\n"
    "App.document().MeshBox.Mesh=mb\n"
    "App.document().recompute()" );

  doCommand(Gui,"Gui.activeDocument().activeView().fitAll()");
  commitCommand();

  updateActive();
}

bool CmdMeshExMakeMesh::isActive(void)
{
  return hasActiveDocument() && !hasObject("MeshBox");
}

//===========================================================================
// Example MakeTool
//===========================================================================
DEF_STD_CMD_A(CmdMeshExMakeTool);

CmdMeshExMakeTool::CmdMeshExMakeTool()
  :Command("Mesh_ExMakeTool")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Make a Tool");
  sToolTipText  = sMenuText;
  sWhatsThis    = sMenuText;
  sStatusTip    = sMenuText;
  sPixmap       = "Std_Tool2";
  iAccel        = 0;
}

void CmdMeshExMakeTool::activated(int iMsg)
{
  openCommand("Mesh Mesh Create");
  doCommand(Doc,
    "import Mesh,MeshGui\n"
    "mt = Mesh.newMesh()\n"
    "mt.addFacet(0.0,0.0,0.0, 0.0,0.0,1.0, 0.0,1.0,1.0)\n"
    "mt.addFacet(0.0,0.0,0.0, 0.0,1.0,1.0, 0.0,1.0,0.0)\n"
    "mt.addFacet(0.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,1.0)\n"
    "mt.addFacet(0.0,0.0,0.0, 1.0,0.0,1.0, 0.0,0.0,1.0)\n"
    "mt.addFacet(0.0,0.0,0.0, 0.0,1.0,0.0, 1.0,1.0,0.0)\n"
    "mt.addFacet(0.0,0.0,0.0, 1.0,1.0,0.0, 1.0,0.0,0.0)\n"
    "mt.addFacet(0.0,1.0,0.0, 0.0,1.0,1.0, 1.0,1.0,1.0)\n"
    "mt.addFacet(0.0,1.0,0.0, 1.0,1.0,1.0, 1.0,1.0,0.0)\n"
    "mt.addFacet(0.0,1.0,1.0, 0.0,0.0,1.0, 1.0,0.0,1.0)\n"
    "mt.addFacet(0.0,1.0,1.0, 1.0,0.0,1.0, 1.0,1.0,1.0)\n"
    "mt.addFacet(1.0,1.0,0.0, 1.0,1.0,1.0, 1.0,0.0,1.0)\n"
    "mt.addFacet(1.0,1.0,0.0, 1.0,0.0,1.0, 1.0,0.0,0.0)\n"
    "mt.scale(100.0)\n"
    "mt.translate(50.0,50.0,50.0)\n"
    "App.document().addObject(\"Mesh::Feature\",\"MeshTool\")\n"
    "App.document().MeshTool.Mesh=mt\n");

  commitCommand();
 
  updateActive();
}

bool CmdMeshExMakeTool::isActive(void)
{
  return hasObject("MeshBox") && !hasObject("MeshTool");
}


//===========================================================================
// Example MakeUnion
//===========================================================================
DEF_STD_CMD_A(CmdMeshExMakeUnion);

CmdMeshExMakeUnion::CmdMeshExMakeUnion()
  :Command("Mesh_ExMakeUnion")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Make Union");
  sToolTipText  = sMenuText;
  sWhatsThis    = sMenuText;
  sStatusTip    = sMenuText;
  sPixmap       = "Std_Tool3";
  iAccel        = 0;
}

void CmdMeshExMakeUnion::activated(int iMsg)
{
  openCommand("Mesh Mesh Create");
  doCommand(Doc,
    "import Mesh,MeshGui\n"
    "m1 = App.document().MeshBox.Mesh\n"
    "m2 = App.document().MeshTool.Mesh\n"
    "m3 = m1.copy()\n"
    "m3.unite(m2)\n"
    "App.document().addObject(\"Mesh::Feature\",\"MeshUnion\")\n"
    "App.document().MeshUnion.Mesh=m3\n");
 
  updateActive();

  doCommand(Gui,"Gui.hide(\"MeshBox\")");
  doCommand(Gui,"Gui.hide(\"MeshTool\")");

  commitCommand();
}

bool CmdMeshExMakeUnion::isActive(void)
{
  return hasObject("MeshTool") && !hasObject("MeshUnion");
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//===========================================================================
// CmdMeshImport
//===========================================================================
DEF_STD_CMD_A(CmdMeshImport);

CmdMeshImport::CmdMeshImport()
  :Command("Mesh_Import")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Import mesh...");
  sToolTipText  = QT_TR_NOOP("Imports a mesh from file");
  sWhatsThis    = QT_TR_NOOP("Imports a mesh from file");
  sStatusTip    = QT_TR_NOOP("Imports a mesh from file");
  sPixmap       = "import_mesh";
  iAccel        = 0;
}

void CmdMeshImport::activated(int iMsg)
{
  // use current path as default
  QString dir = Gui::FileDialog::getWorkingDirectory();

  QStringList filter;
  filter << "All Mesh Files (*.stl *.ast *.bms)";
  filter << "Binary STL (*.stl)";
  filter << "ASCII STL (*.ast)";
  filter << "Binary Mesh (*.bms)";
  filter << "Inventor V2.1 ascii (*.iv)";
  //filter << "Nastran (*.nas *.bdf)";
  filter << "All Files (*.*)";

  // Allow multi selection
  QStringList fn = Gui::FileDialog::getOpenFileNames( filter.join(";;"), dir, Gui::getMainWindow(), 0, QObject::tr("Import mesh") );
  for ( QStringList::Iterator it = fn.begin(); it != fn.end(); ++it )
  {
    QFileInfo fi;
    fi.setFile(*it);

    openCommand("Import Mesh");
    doCommand(Doc,"f = App.document().addObject(\"Mesh::Import\",\"%s\")", fi.baseName().latin1());
    doCommand(Doc,"f.FileName = \"%s\"",(*it).ascii());
    commitCommand();
    updateActive();

    if ( it == fn.begin() )
      Gui::FileDialog::setWorkingDirectory(*it);
  }
}

bool CmdMeshImport::isActive(void)
{
  if( getActiveGuiDocument() )
    return true;
  else
    return false;
}

//===========================================================================
// CmdMeshExport
//===========================================================================
DEF_STD_CMD_A(CmdMeshExport);

CmdMeshExport::CmdMeshExport()
  :Command("Mesh_Export")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Export mesh...");
  sToolTipText  = QT_TR_NOOP("Exports a mesh to file");
  sWhatsThis    = QT_TR_NOOP("Exports a mesh to file");
  sStatusTip    = QT_TR_NOOP("Exports a mesh to file");
  sPixmap       = "export_mesh";
  iAccel        = 0;
}

void CmdMeshExport::activated(int iMsg)
{
  std::vector<App::DocumentObject*> docObjs = Gui::Selection().getObjectsOfType(Mesh::Feature::getClassTypeId());
  if ( docObjs.size() != 1 )
    return;

  App::DocumentObject* docObj = docObjs.front();

  QString dir = Gui::FileDialog::getWorkingDirectory();
  dir += "/";
  dir += docObj->name.getValue();

  QString filter = "Binary STL (*.stl);;ASCII STL (*.stl);;ASCII STL (*.ast);;Binary Mesh (*.bms);;Inventor V2.1 ascii (*.iv);;"
                   "VRML V2.0 (*.wrl *.vrml);;Compressed VRML 2.0 (*.wrz);;Nastran (*.nas *.bdf);;All Files (*.*)";
  QString format;
  QString fn = Gui::FileDialog::getSaveFileName( dir, filter, Gui::getMainWindow(), 0,
                                                 QObject::tr("Export mesh"), &format, true, QObject::tr("Export") );

  if (! fn.isEmpty() )
  {
    if ( format.startsWith("Binary STL") )
      format = "Binary STL";
    else if ( format.startsWith("ASCII STL") )
      format = "ASCII STL";

    QFileInfo fi; fi.setFile(fn);
    openCommand("Mesh ExportSTL Create");
    doCommand(Doc,"f = App.document().addObject(\"Mesh::Export\",\"%s\")", fi.baseName().ascii());
    doCommand(Doc,"f.FileName = \"%s\"",fn.ascii());
    doCommand(Doc,"f.Format = \"%s\"",format.ascii());
    doCommand(Doc,"f.Source = App.document().%s",docObj->name.getValue());
    commitCommand();
    updateActive();

    Gui::FileDialog::setWorkingDirectory(fn);
  }
}

bool CmdMeshExport::isActive(void)
{
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) == 1;
}

DEF_STD_CMD_A(CmdMeshVertexCurvature);

CmdMeshVertexCurvature::CmdMeshVertexCurvature()
  :Command("Mesh_VertexCurvature")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Curvature info");
  sToolTipText  = QT_TR_NOOP("Calculates the curvature of the vertices of a mesh");
  sWhatsThis    = QT_TR_NOOP("Calculates the curvature of the vertices of a mesh");
  sStatusTip    = QT_TR_NOOP("Calculates the curvature of the vertices of a mesh");
  sPixmap       = "curv_info";
}

void CmdMeshVertexCurvature::activated(int iMsg)
{
  std::vector<App::DocumentObject*> meshes = getSelection().getObjectsOfType(Mesh::Feature::getClassTypeId());
  for ( std::vector<App::DocumentObject*>::const_iterator it = meshes.begin(); it != meshes.end(); ++it )
  {
    std::string fName = (*it)->name.getValue();
    fName += "_Curvature";
    fName = getUniqueObjectName(fName.c_str());

    openCommand("Mesh VertexCurvature");
    doCommand(Doc,"App.document().addObject(\"Mesh::Curvature\",\"%s\")",fName.c_str());
    doCommand(Doc,"App.document().%s.Source = App.document().%s",fName.c_str(),(*it)->name.getValue());
    commitCommand();
    updateActive();
    doCommand(Gui,"Gui.hide(\"%s\")",(*it)->name.getValue());
  }

  getSelection().clearSelection();
}

bool CmdMeshVertexCurvature::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) > 0;
}

DEF_STD_CMD_A(CmdMeshPolyPick);

CmdMeshPolyPick::CmdMeshPolyPick()
  :Command("Mesh_PolyPick")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Surface info");
  sToolTipText  = QT_TR_NOOP("Creates a segment from a picked polygon");
  sWhatsThis    = QT_TR_NOOP("Creates a segment from a picked polygon");
  sStatusTip    = QT_TR_NOOP("Creates a segment from a picked polygon");
  sPixmap       = "PolygonPick";
}

void CmdMeshPolyPick::activated(int iMsg)
{
  std::vector<App::DocumentObject*> fea = Gui::Selection().getObjectsOfType(Mesh::Feature::getClassTypeId());
  if ( fea.size() == 1 )
  {
    Gui::ViewProvider* pVP = getActiveGuiDocument()->getViewProvider(fea.front());
    pVP->setEdit();
  }
}

bool CmdMeshPolyPick::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) == 1;
}

DEF_STD_CMD_A(CmdMeshPolyCut);

CmdMeshPolyCut::CmdMeshPolyCut()
  :Command("Mesh_PolyCut")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Cut mesh");
  sToolTipText  = QT_TR_NOOP("Cuts a mesh with a picked polygon");
  sWhatsThis    = QT_TR_NOOP("Cuts a mesh with a picked polygon");
  sStatusTip    = QT_TR_NOOP("Cuts a mesh with a picked polygon");
  sPixmap       = "PolygonPick";
}

void CmdMeshPolyCut::activated(int iMsg)
{
  std::vector<App::DocumentObject*> docObj = Gui::Selection().getObjectsOfType(Mesh::Feature::getClassTypeId());
  for ( std::vector<App::DocumentObject*>::iterator it = docObj.begin(); it != docObj.end(); ++it )
  {
    if ( it == docObj.begin() ) {
      Gui::Document* doc = getActiveGuiDocument();
      Gui::MDIView* view = doc->getActiveView();
      if ( view->getTypeId().isDerivedFrom(Gui::View3DInventor::getClassTypeId()) ) {
        Gui::View3DInventorViewer* viewer = ((Gui::View3DInventor*)view)->getViewer();
        viewer->startPicking( Gui::View3DInventorViewer::Lasso );
      } else {
        return;
      }
    }

    Gui::ViewProvider* pVP = getActiveGuiDocument()->getViewProvider( *it );
    pVP->setEdit();
  }
}

bool CmdMeshPolyCut::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) > 0;
}

DEF_STD_CMD_A(CmdMeshToolMesh);

CmdMeshToolMesh::CmdMeshToolMesh()
  :Command("Mesh_ToolMesh")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Segment by tool mesh");
  sToolTipText  = QT_TR_NOOP("Creates a segment from a given tool mesh");
  sWhatsThis    = QT_TR_NOOP("Creates a segment from a given tool mesh");
  sStatusTip    = QT_TR_NOOP("Creates a segment from a given tool mesh");
  sPixmap       = "Std_Tool4";
}

void CmdMeshToolMesh::activated(int iMsg)
{
  std::vector<App::DocumentObject*> fea = Gui::Selection().getObjectsOfType(Mesh::Feature::getClassTypeId());
  if ( fea.size() == 2 )
  {
    std::string fName = getUniqueObjectName("MeshSegment");
    App::DocumentObject* mesh = fea.front();
    App::DocumentObject* tool = fea.back();

    openCommand("Segment by tool mesh");
    doCommand(Doc, "import Mesh");
    doCommand(Gui, "import MeshGui");
    doCommand(Doc,
      "App.document().addObject(\"Mesh::SegmentByMesh\",\"%s\")\n"
      "App.document().%s.Source = App.document().%s\n"
      "App.document().%s.Tool = App.document().%s\n",
      fName.c_str(), fName.c_str(),  mesh->name.getValue(), fName.c_str(), tool->name.getValue() );

    commitCommand();
    updateActive();

    App::Document* pDoc = getDocument();
    App::DocumentObject * pObj = pDoc->getObject( fName.c_str() );

    if ( pObj )
    {
      doCommand(Gui,"Gui.hide(\"%s\")", mesh->name.getValue());
      doCommand(Gui,"Gui.hide(\"%s\")", tool->name.getValue());
      getSelection().clearSelection();
    }
  }
}

bool CmdMeshToolMesh::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) == 2;
}

DEF_STD_CMD_A(CmdMeshEvaluation);

CmdMeshEvaluation::CmdMeshEvaluation()
  :Command("Mesh_Evaluation")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Evaluate mesh...");
  sToolTipText  = QT_TR_NOOP("Opens a dialog to analyze and repair a mesh");
  sWhatsThis    = QT_TR_NOOP("Opens a dialog to analyze and repair a mesh");
  sStatusTip    = QT_TR_NOOP("Opens a dialog to analyze and repair a mesh");
//  sPixmap       = "curv_info";
}

void CmdMeshEvaluation::activated(int iMsg)
{
  std::vector<App::DocumentObject*> meshes = getSelection().getObjectsOfType(Mesh::Feature::getClassTypeId());
  for ( std::vector<App::DocumentObject*>::const_iterator it = meshes.begin(); it != meshes.end(); ++it )
  {
    MeshGui::DockEvaluateMeshImp::instance()->setMesh( (Mesh::Feature*)(*it) );
    break;
  }
}

bool CmdMeshEvaluation::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return (!MeshGui::DockEvaluateMeshImp::hasInstance()) && (getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) == 1);
}

DEF_STD_CMD_A(CmdMeshEvaluateSolid);

CmdMeshEvaluateSolid::CmdMeshEvaluateSolid()
  :Command("Mesh_EvaluateSolid")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Check solid mesh");
  sToolTipText  = QT_TR_NOOP("Checks whether the mesh is a solid");
  sWhatsThis    = QT_TR_NOOP("Checks whether the mesh is a solid");
  sStatusTip    = QT_TR_NOOP("Checks whether the mesh is a solid");
//  sPixmap       = "curv_info";
}

void CmdMeshEvaluateSolid::activated(int iMsg)
{
  std::vector<App::DocumentObject*> meshes = getSelection().getObjectsOfType(Mesh::Feature::getClassTypeId());
  for ( std::vector<App::DocumentObject*>::const_iterator it = meshes.begin(); it != meshes.end(); ++it )
  {
    Mesh::Feature* mesh = (Mesh::Feature*)(*it);
    QString msg = QString("The mesh '%1' is ").arg(mesh->name.getValue());
    if ( mesh->getMesh().HasOpenEdges() )
      msg += "not a solid.";
    else
      msg += "a solid.";
    QMessageBox::information(Gui::getMainWindow(), QObject::tr("Solid Mesh"), msg);
  }
}

bool CmdMeshEvaluateSolid::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) == 1;
}

DEF_STD_CMD_A(CmdMeshHarmonizeNormals);

CmdMeshHarmonizeNormals::CmdMeshHarmonizeNormals()
  :Command("Mesh_HarmonizeNormals")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Harmoinze normals");
  sToolTipText  = QT_TR_NOOP("Hormonizes the normals of the mesh");
  sWhatsThis    = QT_TR_NOOP("Hormonizes the normals of the mesh");
  sStatusTip    = QT_TR_NOOP("Hormonizes the normals of the mesh");
//  sPixmap       = "curv_info";
}

void CmdMeshHarmonizeNormals::activated(int iMsg)
{
  std::vector<App::DocumentObject*> meshes = getSelection().getObjectsOfType(Mesh::Feature::getClassTypeId());
  for ( std::vector<App::DocumentObject*>::const_iterator it = meshes.begin(); it != meshes.end(); ++it )
  {
    std::string fName = (*it)->name.getValue();
    fName += "_Harmonize";
    fName = getUniqueObjectName(fName.c_str());
    openCommand("Mesh Harmonize Normals");
    doCommand(Doc,"App.activeDocument().addObject(\"Mesh::HarmonizeNormals\",\"%s\")",fName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Source = App.document().%s",fName.c_str(),(*it)->name.getValue());
    commitCommand();
    updateActive();
    doCommand(Gui,"Gui.hide(\"%s\")",(*it)->name.getValue());
  }
}

bool CmdMeshHarmonizeNormals::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) > 0;
}

DEF_STD_CMD_A(CmdMeshFlipNormals);

CmdMeshFlipNormals::CmdMeshFlipNormals()
  :Command("Mesh_FlipNormals")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Flip normals");
  sToolTipText  = QT_TR_NOOP("Flips the normals of the mesh");
  sWhatsThis    = QT_TR_NOOP("Flips the normals of the mesh");
  sStatusTip    = QT_TR_NOOP("Flips the normals of the mesh");
//  sPixmap       = "curv_info";
}

void CmdMeshFlipNormals::activated(int iMsg)
{
  std::vector<App::DocumentObject*> meshes = getSelection().getObjectsOfType(Mesh::Feature::getClassTypeId());
  for ( std::vector<App::DocumentObject*>::const_iterator it = meshes.begin(); it != meshes.end(); ++it )
  {
    std::string fName = (*it)->name.getValue();
    fName += "_Flip";
    fName = getUniqueObjectName(fName.c_str());
    openCommand("Mesh Flip Normals");
    doCommand(Doc,"App.activeDocument().addObject(\"Mesh::FlipNormals\",\"%s\")",fName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Source = App.document().%s",fName.c_str(),(*it)->name.getValue());
    commitCommand();
    updateActive();
    doCommand(Gui,"Gui.hide(\"%s\")",(*it)->name.getValue());
  }
}

bool CmdMeshFlipNormals::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) > 0;
}

DEF_STD_CMD_A(CmdMeshFixDegenerations);

CmdMeshFixDegenerations::CmdMeshFixDegenerations()
  :Command("Mesh_FixDegenerations")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Remove degenerated faces");
  sToolTipText  = QT_TR_NOOP("Remove degenerated faces from the mesh");
  sWhatsThis    = QT_TR_NOOP("Remove degenerated faces from the mesh");
  sStatusTip    = QT_TR_NOOP("Remove degenerated faces from the mesh");
//  sPixmap       = "curv_info";
}

void CmdMeshFixDegenerations::activated(int iMsg)
{
  std::vector<App::DocumentObject*> meshes = getSelection().getObjectsOfType(Mesh::Feature::getClassTypeId());
  for ( std::vector<App::DocumentObject*>::const_iterator it = meshes.begin(); it != meshes.end(); ++it )
  {
    std::string fName = (*it)->name.getValue();
    fName += "_Fixed";
    fName = getUniqueObjectName(fName.c_str());
    openCommand("Mesh Harmonize Normals");
    doCommand(Doc,"App.activeDocument().addObject(\"Mesh::FixDegenerations\",\"%s\")",fName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Source = App.document().%s",fName.c_str(),(*it)->name.getValue());
    commitCommand();
    updateActive();
    doCommand(Gui,"Gui.hide(\"%s\")",(*it)->name.getValue());
  }
}

bool CmdMeshFixDegenerations::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) > 0;
}

DEF_STD_CMD_A(CmdMeshFixDuplicateFaces);

CmdMeshFixDuplicateFaces::CmdMeshFixDuplicateFaces()
  :Command("Mesh_FixDuplicateFaces")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Remove duplicated faces");
  sToolTipText  = QT_TR_NOOP("Remove duplicated faces from the mesh");
  sWhatsThis    = QT_TR_NOOP("Remove duplicated faces from the mesh");
  sStatusTip    = QT_TR_NOOP("Remove duplicated faces from the mesh");
//  sPixmap       = "curv_info";
}

void CmdMeshFixDuplicateFaces::activated(int iMsg)
{
  std::vector<App::DocumentObject*> meshes = getSelection().getObjectsOfType(Mesh::Feature::getClassTypeId());
  for ( std::vector<App::DocumentObject*>::const_iterator it = meshes.begin(); it != meshes.end(); ++it )
  {
    std::string fName = (*it)->name.getValue();
    fName += "_Fixed";
    fName = getUniqueObjectName(fName.c_str());
    openCommand("Mesh Harmonize Normals");
    doCommand(Doc,"App.activeDocument().addObject(\"Mesh::FixDuplicatedFaces\",\"%s\")",fName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Source = App.document().%s",fName.c_str(),(*it)->name.getValue());
    commitCommand();
    updateActive();
    doCommand(Gui,"Gui.hide(\"%s\")",(*it)->name.getValue());
  }
}

bool CmdMeshFixDuplicateFaces::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) > 0;
}

DEF_STD_CMD_A(CmdMeshFixDuplicatePoints);

CmdMeshFixDuplicatePoints::CmdMeshFixDuplicatePoints()
  :Command("Mesh_FixDuplicatePoints")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Remove duplicated points");
  sToolTipText  = QT_TR_NOOP("Remove duplicated points from the mesh");
  sWhatsThis    = QT_TR_NOOP("Remove duplicated points from the mesh");
  sStatusTip    = QT_TR_NOOP("Remove duplicated points from the mesh");
//  sPixmap       = "curv_info";
}

void CmdMeshFixDuplicatePoints::activated(int iMsg)
{
  std::vector<App::DocumentObject*> meshes = getSelection().getObjectsOfType(Mesh::Feature::getClassTypeId());
  for ( std::vector<App::DocumentObject*>::const_iterator it = meshes.begin(); it != meshes.end(); ++it )
  {
    std::string fName = (*it)->name.getValue();
    fName += "_Fixed";
    fName = getUniqueObjectName(fName.c_str());
    openCommand("Mesh Harmonize Normals");
    doCommand(Doc,"App.activeDocument().addObject(\"Mesh::FixDuplicatedFaces\",\"%s\")",fName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Source = App.document().%s",fName.c_str(),(*it)->name.getValue());
    commitCommand();
    updateActive();
    doCommand(Gui,"Gui.hide(\"%s\")",(*it)->name.getValue());
  }
}

bool CmdMeshFixDuplicatePoints::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) > 0;
}

DEF_STD_CMD_A(CmdMeshFixIndices);

CmdMeshFixIndices::CmdMeshFixIndices()
  :Command("Mesh_FixIndices")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Fix indices");
  sToolTipText  = QT_TR_NOOP("Fixes invalid indices in the mesh structure");
  sWhatsThis    = QT_TR_NOOP("Fixes invalid indices in the mesh structure");
  sStatusTip    = QT_TR_NOOP("Fixes invalid indices in the mesh structure");
//  sPixmap       = "curv_info";
}

void CmdMeshFixIndices::activated(int iMsg)
{
  std::vector<App::DocumentObject*> meshes = getSelection().getObjectsOfType(Mesh::Feature::getClassTypeId());
  for ( std::vector<App::DocumentObject*>::const_iterator it = meshes.begin(); it != meshes.end(); ++it )
  {
    std::string fName = (*it)->name.getValue();
    fName += "_Fixed";
    fName = getUniqueObjectName(fName.c_str());
    openCommand("Mesh Harmonize Normals");
    doCommand(Doc,"App.activeDocument().addObject(\"Mesh::FixIndices\",\"%s\")",fName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Source = App.document().%s",fName.c_str(),(*it)->name.getValue());
    commitCommand();
    updateActive();
    doCommand(Gui,"Gui.hide(\"%s\")",(*it)->name.getValue());
  }
}

bool CmdMeshFixIndices::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) > 0;
}

DEF_STD_CMD_A(CmdMeshBoundingBox);

CmdMeshBoundingBox::CmdMeshBoundingBox()
  :Command("Mesh_BoundingBox")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Boundings info");
  sToolTipText  = QT_TR_NOOP("Shows the boundings of the selected mesh");
  sWhatsThis    = QT_TR_NOOP("Shows the boundings of the selected mesh");
  sStatusTip    = QT_TR_NOOP("Shows the boundings of the selected mesh");
//  sPixmap       = "curv_info";
}

void CmdMeshBoundingBox::activated(int iMsg)
{
  std::vector<App::DocumentObject*> meshes = getSelection().getObjectsOfType(Mesh::Feature::getClassTypeId());
  for ( std::vector<App::DocumentObject*>::const_iterator it = meshes.begin(); it != meshes.end(); ++it )
  {
    const MeshCore::MeshKernel& rMesh = ((Mesh::Feature*)(*it))->getMesh();
    const Base::BoundBox3f& box = rMesh.GetBoundBox();
    QString minX, maxX, minY, maxY, minZ, maxZ;
    minX.sprintf("Min X=%f", box.MinX);
    maxX.sprintf("Max X=%f", box.MaxX);
    minY.sprintf("Min Y=%f", box.MinY);
    maxY.sprintf("Min Y=%f", box.MaxY);
    minZ.sprintf("Min Z=%f", box.MinZ);
    maxZ.sprintf("Max Z=%f", box.MaxZ);

    QString msg = QString("%1\t%2\t%3\n\n%4\t%5\t%6\n\n").arg(minX).arg(minY).arg(minZ).arg(maxX).arg(maxY).arg(maxZ);
    QMessageBox::information(Gui::getMainWindow(), QObject::tr("Boundings"), msg);
    break;
  }
}

bool CmdMeshBoundingBox::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) == 1;
}

DEF_STD_CMD_A(CmdMeshBuildRegularSolid);

CmdMeshBuildRegularSolid::CmdMeshBuildRegularSolid()
  :Command("Mesh_BuildRegularSolid")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Regular solid...");
  sToolTipText  = QT_TR_NOOP("Builds a regular solid");
  sWhatsThis    = QT_TR_NOOP("Builds a regular solid");
  sStatusTip    = QT_TR_NOOP("Builds a regular solid");
  sPixmap       = "solid_mesh";
}

void CmdMeshBuildRegularSolid::activated(int iMsg)
{
  MeshGui::SingleDlgRegularSolidImp::instance()->show();
}

bool CmdMeshBuildRegularSolid::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return (!MeshGui::SingleDlgRegularSolidImp::hasInstance())&&hasActiveDocument();
}

DEF_STD_CMD_A(CmdMeshFillupHoles);

CmdMeshFillupHoles::CmdMeshFillupHoles()
  :Command("Mesh_FillupHoles")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Fill holes...");
  sToolTipText  = QT_TR_NOOP("Fill holes of the mesh");
  sWhatsThis    = QT_TR_NOOP("Fill holes of the mesh");
  sStatusTip    = QT_TR_NOOP("Fill holes of the mesh");
//  sPixmap       = "curv_info";
}

void CmdMeshFillupHoles::activated(int iMsg)
{
  std::vector<App::DocumentObject*> meshes = getSelection().getObjectsOfType(Mesh::Feature::getClassTypeId());
  bool ok;
  int FillupHolesOfLength = QInputDialog::getInteger( QObject::tr("Fill holes"), QObject::tr("Fill holes with maximum number of edges:"), 
                                                      3, 3, 10000, 1, &ok, Gui::getMainWindow() );
  if (!ok) return;

  openCommand("Fill up holes");
  for ( std::vector<App::DocumentObject*>::const_iterator it = meshes.begin(); it != meshes.end(); ++it )
  {
    std::string fName = (*it)->name.getValue();
    fName += "_fill";
    fName = getUniqueObjectName(fName.c_str());
    doCommand(Doc,"App.activeDocument().addObject(\"Mesh::FillHoles\",\"%s\")",fName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Source = App.document().%s",fName.c_str(),(*it)->name.getValue());
    doCommand(Doc,"App.activeDocument().%s.FillupHolesOfLength = %d",fName.c_str(), FillupHolesOfLength);
  }
  commitCommand();
  updateActive();
}

bool CmdMeshFillupHoles::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) > 0;
}

DEF_STD_CMD_A(CmdMeshRemoveComponents);

CmdMeshRemoveComponents::CmdMeshRemoveComponents()
  :Command("Mesh_RemoveComponents")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Remove components...");
  sToolTipText  = QT_TR_NOOP("Remove topologic independant components from the mesh");
  sWhatsThis    = QT_TR_NOOP("Remove topologic independant components from the mesh");
  sStatusTip    = QT_TR_NOOP("Remove topologic independant components from the mesh");
//  sPixmap       = "curv_info";
}

void CmdMeshRemoveComponents::activated(int iMsg)
{
  std::vector<App::DocumentObject*> meshes = getSelection().getObjectsOfType(Mesh::Feature::getClassTypeId());
  bool ok;
  int RemoveCompOfSize = QInputDialog::getInteger( QObject::tr("Remove components"), QObject::tr("Removes components up to a maximum number of triangles:"), 
                                                      3, 1, 10000000, 1, &ok, Gui::getMainWindow() );
  if (!ok) return;

  openCommand("Remove components");
  for ( std::vector<App::DocumentObject*>::const_iterator it = meshes.begin(); it != meshes.end(); ++it )
  {
    std::string fName = (*it)->name.getValue();
    fName += "_rem_comps";
    fName = getUniqueObjectName(fName.c_str());
    doCommand(Doc,"App.activeDocument().addObject(\"Mesh::RemoveComponents\",\"%s\")",fName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Source = App.document().%s",fName.c_str(),(*it)->name.getValue());
    doCommand(Doc,"App.activeDocument().%s.RemoveCompOfSize = %d",fName.c_str(), RemoveCompOfSize);
  }
  commitCommand();
  updateActive();
}

bool CmdMeshRemoveComponents::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) > 0;
}

void CreateMeshCommands(void)
{
  Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();
  rcCmdMgr.addCommand(new CmdMeshImport());
  rcCmdMgr.addCommand(new CmdMeshExport());
  rcCmdMgr.addCommand(new CmdMeshVertexCurvature());
  rcCmdMgr.addCommand(new CmdMeshExMakeMesh());
  rcCmdMgr.addCommand(new CmdMeshExMakeTool());
  rcCmdMgr.addCommand(new CmdMeshExMakeUnion());
  rcCmdMgr.addCommand(new CmdMeshDemolding());
  rcCmdMgr.addCommand(new CmdMeshPolyPick());
  rcCmdMgr.addCommand(new CmdMeshPolyCut());
  rcCmdMgr.addCommand(new CmdMeshToolMesh());
  rcCmdMgr.addCommand(new CmdMeshTransform());
  rcCmdMgr.addCommand(new CmdMeshEvaluation());
  rcCmdMgr.addCommand(new CmdMeshEvaluateSolid());
  rcCmdMgr.addCommand(new CmdMeshHarmonizeNormals());
  rcCmdMgr.addCommand(new CmdMeshFlipNormals());
  rcCmdMgr.addCommand(new CmdMeshFixDegenerations());
  rcCmdMgr.addCommand(new CmdMeshFixDuplicateFaces());
  rcCmdMgr.addCommand(new CmdMeshFixDuplicatePoints());
  rcCmdMgr.addCommand(new CmdMeshFixIndices());
  rcCmdMgr.addCommand(new CmdMeshBoundingBox());
  rcCmdMgr.addCommand(new CmdMeshBuildRegularSolid());
  rcCmdMgr.addCommand(new CmdMeshFillupHoles());
  rcCmdMgr.addCommand(new CmdMeshRemoveComponents());
}
