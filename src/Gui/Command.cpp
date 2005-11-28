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
#endif

#include "Command.h"
#include "Action.h"
#include "Application.h"
#include "Document.h"
#include "Selection.h"
#include "ToolBoxBar.h"
#include "HelpView.h"
#include "Macro.h"
#include "MainWindow.h"
#include "DlgUndoRedo.h"
#include "BitmapFactory.h"
#include "WhatsThis.h"
#include "WaitCursor.h"

#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/Interpreter.h>
#include <Base/Sequencer.h>

using Base::Interpreter;
using namespace Gui;
using namespace Gui::Dialog;
using namespace Gui::DockWnd;

CommandBase::CommandBase( const char* sMenu, const char* sToolTip, const char* sWhat, 
                          const char* sStatus, const char* sPixmap, int iAcc)
 : sMenuText(sMenu), sToolTipText(sToolTip), sWhatsThis(sWhat), sStatusTip(sStatus), 
   sPixmap(sPixmap), iAccel(iAcc), _pcAction(0)
{
  if (sToolTipText && !sWhatsThis) sWhatsThis = sToolTipText;
  if (sToolTipText && !sStatusTip) sStatusTip = sToolTipText;
}

CommandBase::~CommandBase()
{
}

QAction* CommandBase::getAction( bool create ) 
{ 
  if (!_pcAction && create)
    _pcAction = createAction();
  return _pcAction; 
}

QAction * CommandBase::createAction()
{
  // does nothing
  return 0;
}

void CommandBase::languageChange()
{
  if ( _pcAction )
  {
    _pcAction->setText      ( QObject::tr( sMenuText    ) );
    _pcAction->setMenuText  ( QObject::tr( sMenuText    ) );
    _pcAction->setToolTip   ( QObject::tr( sToolTipText ) );
    _pcAction->setStatusTip ( QObject::tr( sStatusTip   ) );
    _pcAction->setWhatsThis ( QObject::tr( sWhatsThis   ) );
  }
}

//===========================================================================
// Command
//===========================================================================

/* TRANSLATOR Gui::Command */

Command::Command(const char* name,CMD_Type eType)
  : CommandBase(0), sName(name), sHelpUrl(0),_eType(eType)
{
  sAppModule  = "FreeCAD";
  sGroup      = QT_TR_NOOP("Standard");
}

Command::~Command()
{
  delete _pcAction;
}

bool Command::addTo(QWidget *pcWidget)
{
  if (!_pcAction)
    _pcAction = createAction();

  return _pcAction->addTo(pcWidget);
}

bool Command::removeFrom(QWidget *pcWidget)
{
  if (!_pcAction)
    return false;
  return _pcAction->removeFrom(pcWidget);
}

Application *Command::getGuiApplication(void)
{
  return Application::Instance;
}

Gui::Document* Command::getActiveGuiDocument(void)
{
  return getGuiApplication()->activeDocument();
}

App::Document* Command::getDocument(const char* Name)
{
  if(Name)
    return App::GetApplication().getDocument(Name);
  else
  {
    Gui::Document * pcDoc = getGuiApplication()->activeDocument();
    if(pcDoc)
      return pcDoc->getDocument();
    else
      return 0l;
  }
}

App::Feature* Command::getFeature(const char* Name)
{
  App::Document*pDoc = getDocument();
  if(pDoc)
    return pDoc->getFeature(Name);
  else
    return 0;
}

bool Command::isToggle(void) const
{
  return (_eType&Cmd_Toggle) != 0; 
}

void Command::activated ()
{
  if(_eType == Cmd_Normal)
  {
    // Do not query _pcAction since it isn't created necessarily
    Base::Console().Log("CmdG: %s\n",sName);
    // set the application module type for the macro
    getGuiApplication()->macroManager()->setModule(sAppModule);
    try{
      activated(0);
    }catch(Base::PyException &e){
      e.ReportException();
      Base::Console().Error("Stack Trace: %s\n",e.getStackTrace().c_str());
    }catch(Base::AbortException&){
    }catch(Base::Exception &e){
      e.ReportException();
    }catch(std::exception &e){
      std::string str;
      str += "C++ exception thrown (";
      str += e.what();
      str += ")";
      Base::Console().Error(str.c_str());
    }catch(Standard_Failure){                                                              
		  Handle(Standard_Failure) e = Standard_Failure::Caught(); 
      std::string str;                                         
      str += "OCC exception thrown (";                         
      str += e->GetMessageString();                            
      str += ")\n";                                            
      Base::Console().Error(str.c_str());  
#ifndef FC_DEBUG
    }catch(...){                                                              
    	Base::Console().Error("Gui::Command::activated(): Unknown C++ exception in command thrown");       
#endif
    }   
  }
}

void Command::toggled (bool b)
{
  // if the app is busy do not allow to drop new commands
  if ( Base::Sequencer().isRunning() )
    return; 
  if(_eType == Cmd_Toggle)
  {
    // Do not query _pcAction since it isn't created necessarily
    Base::Console().Log("CmdG: Toggled %s\n",sName);
    if(b)
      activated(1);
    else
      activated(0);
  }
}

void Command::testActive(void)
{
  if(!_pcAction) return;

  if ( StdCmdDescription::inDescriptionMode () )
  {
    _pcAction->setEnabled( true );
    return;
  }

  bool bActive = isActive();
  _pcAction->setEnabled ( bActive );
}

//--------------------------------------------------------------------------
// Helper methodes
//--------------------------------------------------------------------------

bool Command::hasActiveDocument(void)
{
  return getActiveGuiDocument() != 0;
}
/// true when there is a document and a Feature with Name
bool Command::hasFeature(const char* Name)
{
  return getDocument() != 0 && getDocument()->getFeature(Name) != 0;
}

Gui::SelectionSingelton&  Command::getSelection(void)
{
  return Gui::Selection();
}

std::string Command::getUniqueFeatureName(const char *BaseName)
{
  assert(hasActiveDocument());

  return getActiveGuiDocument()->getDocument()->getUniqueFeatureName(BaseName);
}


//--------------------------------------------------------------------------
// UNDO REDO transaction handling  
//--------------------------------------------------------------------------
/** Open a new Undo transaction on the active document
 *  This methode open a new UNDO transaction on the active document. This transaction
 *  will later apear in the UNDO REDO dialog with the name of the command. If the user 
 *  recall the transaction everything changed on the document between OpenCommand() and 
 *  CommitCommand will be undone (or redone). You can use an alternetive name for the 
 *  operation default is the Command name.
 *  @see CommitCommand(),AbortCommand()
 */
void Command::openCommand(const char* sCmdName)
{
  // Using OpenCommand with no active document !
  assert(getGuiApplication()->activeDocument());

  if(sCmdName)
    getGuiApplication()->activeDocument()->openCommand(sCmdName);
  else
    getGuiApplication()->activeDocument()->openCommand(sName);
}

void Command::commitCommand(void)
{
  getGuiApplication()->activeDocument()->commitCommand();
}

void Command::abortCommand(void)
{
  getGuiApplication()->activeDocument()->abortCommand();
}

/// Run a App level Action 
void Command::doCommand(DoCmd_Type eType,const char* sCmd,...)
{
  // temp buffer
  char* format = (char*) malloc(strlen(sCmd)+4024);
  va_list namelessVars;
  va_start(namelessVars, sCmd);  // Get the "..." vars
  vsprintf(format, sCmd, namelessVars);
  va_end(namelessVars);

  if(eType == Gui)
    getGuiApplication()->macroManager()->addLine(MacroManager::Gui,format);
  else
    getGuiApplication()->macroManager()->addLine(MacroManager::Base,format);
  Interpreter().runString(format);

  //Base::Console().Log("#(%s): %s\n",sName.c_str(),format);
  Base::Console().Log("CmdC: %s\n",format);

  free (format);
}


const std::string Command::strToPython(const char* Str)
{
  return Base::InterpreterSingleton::strToPython(Str);
}


/// Activate an other Commands
void Command::activateCommand(const char* sCmdName)
{
  Command* pcCmd = getGuiApplication()->commandManager().getCommandByName(sCmdName);
  if(pcCmd)
  {
    assert(!(pcCmd->isToggle()));
    pcCmd->activated(0);
  }
}

/// Toggles other Commands
void Command::toggleCommand(const char* sCmdName,bool bToggle)
{
  Command* pcCmd = getGuiApplication()->commandManager().getCommandByName(sCmdName);
  if(pcCmd)
  {
    assert(pcCmd->isToggle());
    // check if the QAction is already created
    if ( pcCmd->_pcAction )
      pcCmd->_pcAction->setOn(bToggle?1:0);
  }
}

/// Updates the (active) document (propagate changes)
void Command::updateActive(void)
{
  WaitCursor wc;

  getGuiApplication()->activeDocument()->getDocument()->Recompute();
  //getGuiApplication()->UpdateActive();
}

/// Updates the (all or listed) documents (propagate changes)
void Command::updateAll(std::list<Gui::Document*> cList)
{
  if(cList.size()>0)
  {
    for(std::list<Gui::Document*>::iterator It= cList.begin();It!=cList.end();It++)
      (*It)->onUpdate();
  }else{
    getGuiApplication()->onUpdate();
  }
}

//--------------------------------------------------------------------------
// Online help handling  
//--------------------------------------------------------------------------

/// returns the begin of a online help page
const char * Command::beginCmdHelp(void)
{
  return  "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">\n"
      "<html>\n"
      "<head>\n"
      "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=ISO-8859-1\">\n"
      "<title>FreeCAD Main Index</title>\n"
      "</head>\n"
      "<body bgcolor=\"#ffffff\">\n\n";
}
/// returns the end of a online help page
const char * Command::endCmdHelp(void)
{
  return "</body></html>\n\n";
}

std::string Command::getResource(const char* sName) const
{
  return "";
}

QAction * Command::createAction(void)
{
  QAction *pcAction;

  pcAction = new Action(this,getMainWindow(),sName,(_eType&Cmd_Toggle) != 0);
  pcAction->setText(QObject::tr(sMenuText));
  pcAction->setMenuText(QObject::tr(sMenuText));
  pcAction->setToolTip(QObject::tr(sToolTipText));
  if ( sStatusTip )
    pcAction->setStatusTip(QObject::tr(sStatusTip));
  else
    pcAction->setStatusTip(QObject::tr(sToolTipText));
  if ( sWhatsThis )
    pcAction->setWhatsThis(QObject::tr(sWhatsThis));
  else
    pcAction->setWhatsThis(QObject::tr(sToolTipText));
  if(sPixmap)
    pcAction->setIconSet(Gui::BitmapFactory().pixmap(sPixmap));
  pcAction->setAccel(iAccel);

  return pcAction;
}

// -------------------------------------------------------------------------

ToggleCommand::ToggleCommand(const char* name,CMD_Type eType)
  :Command(name,eType)
{
}

QAction * ToggleCommand::createAction(void)
{
  QAction *pcAction;
  pcAction = new Action(this,getMainWindow(),sName);
  pcAction->setToggleAction( true );
  pcAction->setText(QObject::tr(sMenuText));
  pcAction->setMenuText(QObject::tr(sMenuText));
  pcAction->setToolTip(QObject::tr(sToolTipText));
  pcAction->setStatusTip(QObject::tr(sStatusTip));
  pcAction->setWhatsThis(QObject::tr(sWhatsThis));
  if(sPixmap)
    pcAction->setIconSet(Gui::BitmapFactory().pixmap(sPixmap));
  pcAction->setAccel(iAccel);

  return pcAction;
}

// -------------------------------------------------------------------------

CommandGroup::CommandGroup(const char* name, bool dropdown,CMD_Type eType)
  :Command(name,eType), _dropdown(dropdown)
{
}

CommandGroup::~CommandGroup()
{
  _aCommands.clear();
}

QAction * CommandGroup::createAction(void)
{
  QActionGroup *pcAction;
  pcAction = new ActionGroup( this, getMainWindow(), sName );
  pcAction->setExclusive( true );
  pcAction->setUsesDropDown( _dropdown );
  pcAction->setText(QObject::tr(sMenuText));
  pcAction->setMenuText(QObject::tr(sMenuText));
  pcAction->setToolTip(QObject::tr(sToolTipText));
  pcAction->setStatusTip(QObject::tr(sStatusTip));
  pcAction->setWhatsThis(QObject::tr(sWhatsThis));
  if(sPixmap)
    pcAction->setIconSet(Gui::BitmapFactory().pixmap(sPixmap));
  pcAction->setAccel(iAccel);

  for ( std::vector<CommandBase*>::iterator it = _aCommands.begin(); it != _aCommands.end(); ++it )
  {
    if ( strcmp((*it)->getMenuText(), "Separator") == 0)
    {
      pcAction->addSeparator();
    }
    else
    {
      QAction* pSubAction = new QAction( pcAction );
      pSubAction->setText      ( QObject::tr( (*it)->getMenuText() ) );
      pSubAction->setMenuText  ( QObject::tr( (*it)->getMenuText() ) );

      // use the tooltip, status tip and what's this text of the parent group if not set
      // 
      // set tool tip
      if ( (*it)->getToolTipText() )
        pSubAction->setToolTip ( QObject::tr( (*it)->getToolTipText() ) );
      else
        pSubAction->setToolTip ( QObject::tr( getToolTipText() ) );
      // set status tip
      if ( (*it)->getStatusTip() )
        pSubAction->setStatusTip ( QObject::tr( (*it)->getStatusTip() ) );
      else if ( (*it)->getToolTipText() )
        pSubAction->setStatusTip ( QObject::tr( (*it)->getToolTipText() ) );
      else
        pSubAction->setStatusTip ( QObject::tr( getStatusTip() ) );
      // set what's this
      if ( (*it)->getWhatsThis() )
        pSubAction->setWhatsThis ( QObject::tr( (*it)->getWhatsThis() ) );
      else if ( (*it)->getToolTipText() )
        pSubAction->setWhatsThis ( QObject::tr( (*it)->getToolTipText() ) );
      else
        pSubAction->setWhatsThis ( QObject::tr( getWhatsThis() ) );
      if( sPixmap )
        pSubAction->setIconSet( Gui::BitmapFactory().pixmap( sPixmap ) );
      pSubAction->setAccel( (*it)->getAccel() );

      pSubAction->setToggleAction( true );
    }
  }

  return pcAction;
}

//===========================================================================
// MacroCommand 
//===========================================================================

/* TRANSLATOR Gui::MacroCommand */

MacroCommand::MacroCommand(const char* name)
  :Command(name,Cmd_Normal)
{
  sGroup        = QT_TR_NOOP("Macros");
}

void MacroCommand::activated(int iMsg)
{
//  OpenCommand("Excecute Macro");
//
//  DoCommand(Doc,"execfile(%s)",_sScriptName.c_str());
//
//  void CommitCommand(void);
  std::string cMacroPath = App::GetApplication().GetParameterGroupByPath
    ("User parameter:BaseApp/Preferences/Macro")->GetASCII("MacroPath",
    App::GetApplication().GetHomePath());

  QDir d( cMacroPath.c_str() );
  QFileInfo fi( d, scriptName );
  Application::Instance->macroManager()->run(MacroManager::File,( fi.filePath() ).latin1());
}

void MacroCommand::setScriptName ( const QString& s )
{
  scriptName = s;
}

void MacroCommand::setWhatsThis( const QString& s )
{
  sWhatsThis = s;
  if ( _pcAction )
    _pcAction->setWhatsThis(QObject::tr(sWhatsThis));
}

void MacroCommand::setMenuText( const QString& s )
{
  sMenuText = s;
  if ( _pcAction )
  {
    _pcAction->setText    (QObject::tr(sMenuText));
    _pcAction->setMenuText(QObject::tr(sMenuText));
  }
}

void MacroCommand::setToolTipText( const QString& s )
{
  sToolTipText = s;
  if ( _pcAction )
    _pcAction->setToolTip(QObject::tr(sToolTipText));
}

void MacroCommand::setStatusTip( const QString& s )
{
  sStatusTip = s;
  if ( _pcAction )
    _pcAction->setStatusTip(QObject::tr(sStatusTip));
}

void MacroCommand::setPixmap( const QString& s )
{
  sPixmap = s;
  if ( _pcAction )
  {
    if ( sPixmap )
      _pcAction->setIconSet(Gui::BitmapFactory().pixmap(sPixmap));
    else
      _pcAction->setIconSet(QPixmap());
  }
}

void MacroCommand::setAccel(int i)
{
  iAccel = i;
  if ( _pcAction )
    _pcAction->setAccel(iAccel);
}

void MacroCommand::load()
{
  ParameterGrp::handle hGrp = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Macro");

  if (hGrp->HasGroup("Macros"))
  {
    hGrp = hGrp->GetGroup("Macros");
    std::vector<FCHandle<ParameterGrp> > macros = hGrp->GetGroups();
    for (std::vector<FCHandle<ParameterGrp> >::iterator it = macros.begin(); it!=macros.end(); ++it )
    {
      MacroCommand* macro = new MacroCommand((*it)->GetGroupName());
      macro->setScriptName  ( (*it)->GetASCII( "Script"     ).c_str() );
      macro->setMenuText    ( (*it)->GetASCII( "Menu"       ).c_str() );
      macro->setToolTipText ( (*it)->GetASCII( "Tooltip"    ).c_str() );
      macro->setWhatsThis   ( (*it)->GetASCII( "WhatsThis"  ).c_str() );
      macro->setStatusTip   ( (*it)->GetASCII( "Statustip"  ).c_str() );
      if ((*it)->GetASCII("Pixmap", "nix") != "nix")
        macro->setPixmap    ( (*it)->GetASCII( "Pixmap"     ).c_str() );
      macro->setAccel       ( (*it)->GetInt  ( "Accel",0    )         );
      Application::Instance->commandManager().addCommand( macro );
    }
  }
}

void MacroCommand::save()
{
  ParameterGrp::handle hGrp = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Macro")->GetGroup("Macros");
  hGrp->Clear();

  std::vector<Command*> macros = Application::Instance->commandManager().getGroupCommands("Macros");
  if ( macros.size() > 0 )
  {
    for (std::vector<Command*>::iterator it = macros.begin(); it!=macros.end(); ++it )
    {
      MacroCommand* macro = (MacroCommand*)(*it);
      ParameterGrp::handle hMacro = hGrp->GetGroup(macro->getName());
      hMacro->SetASCII( "Script",    macro->getScriptName () );
      hMacro->SetASCII( "Menu",      macro->getMenuText   () );
      hMacro->SetASCII( "Tooltip",   macro->getToolTipText() );
      hMacro->SetASCII( "WhatsThis", macro->getWhatsThis  () );
      hMacro->SetASCII( "Statustip", macro->getStatusTip  () );
      hMacro->SetASCII( "Pixmap",    macro->getPixmap     () );
      hMacro->SetInt  ( "Accel",     macro->getAccel      () );
    }
  }
}

//===========================================================================
// FCPythonCommand
//===========================================================================


PythonCommand::PythonCommand(const char* name,PyObject * pcPyCommand)
  :Command(name),_pcPyCommand(pcPyCommand)
{
  sGroup = "Python";
  Py_INCREF(_pcPyCommand);

  // call the methode "GetResources()" of the command object
  _pcPyResourceDict = Interpreter().runMethodObject(_pcPyCommand, "GetResources");
  // check if the "GetResources()" methode returns a Dict object
  if(! PyDict_Check(_pcPyResourceDict) )
    throw Base::Exception("FCPythonCommand::FCPythonCommand(): Methode GetResources() of the python command object returns the wrong type (has to be Py Dictonary)");
}

std::string PythonCommand::getResource(const char* sName) const
{
  PyObject* pcTemp;
  Base::PyBuf ResName(sName);


  // get the "MenuText" resource string
  pcTemp = PyDict_GetItemString(_pcPyResourceDict,ResName.str);
  if(! pcTemp )
    return std::string();
  if(! PyString_Check(pcTemp) )
    throw Base::Exception("FCPythonCommand::FCPythonCommand(): Methode GetResources() of the python command object returns a dictionary which holds not only strings");

  return std::string(PyString_AsString(pcTemp) );
}


void PythonCommand::activated(int iMsg)
{
  try{
    Interpreter().runMethodVoid(_pcPyCommand, "Activated");
  }catch (Base::Exception e){
    Base::Console().Error("Running the python command %s failed,try to resume",sName);
  }
}

bool PythonCommand::isActive(void)
{
  return true;
}

std::string PythonCommand::cmdHelpURL(void)
{
  PyObject* pcTemp;

  pcTemp = Interpreter().runMethodObject(_pcPyCommand, "CmdHelpURL"); 

  if(! pcTemp ) 
    return std::string();
  if(! PyString_Check(pcTemp) ) 
    throw Base::Exception("FCPythonCommand::CmdHelpURL(): Methode CmdHelpURL() of the python command object returns no string");
  
  return std::string( PyString_AsString(pcTemp) );
}

void PythonCommand::cmdHelpPage(std::string &rcHelpPage)
{
  PyObject* pcTemp;

  pcTemp = Interpreter().runMethodObject(_pcPyCommand, "CmdHelpPage"); 

  if(! pcTemp ) 
    return ;
  if(! PyString_Check(pcTemp) ) 
    throw Base::Exception("FCPythonCommand::CmdHelpURL(): Methode CmdHelpURL() of the python command object returns no string");

  rcHelpPage = PyString_AsString(pcTemp) ;
}

QAction * PythonCommand::createAction(void)
{
  QAction *pcAction;

  pcAction = new Action(this,getMainWindow(),sName,(_eType&Cmd_Toggle) != 0);
  pcAction->setText(sName);
  pcAction->setMenuText(getResource("MenuText").c_str());
  pcAction->setToolTip(getResource("ToolTip").c_str());
  pcAction->setStatusTip(getResource("StatusTip").c_str());
  pcAction->setWhatsThis(getResource("WhatsThis").c_str());
  if(getResource("Pixmap") != "")
    pcAction->setIconSet(Gui::BitmapFactory().pixmap(getResource("Pixmap").c_str()));

  return pcAction;
}

const char* PythonCommand::getWhatsThis() const
{
  return getResource("WhatsThis").c_str();
}

const char* PythonCommand::getMenuText() const
{
  return getResource("MenuText").c_str();
}

const char* PythonCommand::getToolTipText() const
{
  return getResource("ToolTip").c_str();
}

const char* PythonCommand::getStatusTip() const
{
  return getResource("StatusTip").c_str();
}

const char* PythonCommand::getPixmap() const
{
  return getResource("Pixmap").c_str();
}

int PythonCommand::getAccel() const
{
  return 0;
}

//===========================================================================
// CommandManager 
//===========================================================================


void CommandManager::addCommand(Command* pCom)
{
  _sCommands[pCom->getName()] = pCom;//	pCom->Init();
}

void CommandManager::removeCommand(Command* pCom)
{
  std::map <std::string,Command*>::iterator It = _sCommands.find(pCom->getName());
  if (It != _sCommands.end())
  {
    delete It->second;
    _sCommands.erase(It);
  }
}

void CommandManager::addTo(const char* Name,QWidget *pcWidget)
{
  if (_sCommands.find(Name) == _sCommands.end())
  {
    Base::Console().Error("CommandManager::AddTo() try to add an unknown command (%s) to a widget!\n",Name);
  }
  else
  {
    Command* pCom = _sCommands[Name];
    pCom->addTo(pcWidget);
  }
}

void CommandManager::removeFrom(const char* Name,QWidget *pcWidget)
{
  if (_sCommands.find(Name) != _sCommands.end())
  {
    Command* pCom = _sCommands[Name];
    pCom->removeFrom(pcWidget);
  }
}

std::vector <Command*> CommandManager::getModuleCommands(const char *sModName)
{
  std::vector <Command*> vCmds;

  for( std::map<std::string, Command*>::iterator It= _sCommands.begin();It!=_sCommands.end();It++)
  {
    if( strcmp(It->second->getAppModuleName(),sModName) == 0)
      vCmds.push_back(It->second);
  }

  return vCmds;
}

std::string CommandManager::getAppModuleName(QAction* pAction)
{
  for( std::map<std::string, Command*>::iterator It= _sCommands.begin();It!=_sCommands.end();++It)
  {
    if ( It->second->getAction() == pAction )
      return It->second->getAppModuleName();
  }

  return "Not found";
}

std::string CommandManager::getAppModuleNameByName(const char* sName)
{
  for( std::map<std::string, Command*>::iterator It= _sCommands.begin();It!=_sCommands.end();++It)
  {
    if ( strcmp(It->second->getName(), sName) == 0 )
      return It->second->getAppModuleName();
  }

  return "Not found";
}

std::vector <Command*> CommandManager::getAllCommands(void)
{
  std::vector <Command*> vCmds;

  for( std::map<std::string, Command*>::iterator It= _sCommands.begin();It!=_sCommands.end();It++)
  {
    vCmds.push_back(It->second);
  }

  return vCmds;
}

std::vector <Command*> CommandManager::getGroupCommands(const char *sGrpName)
{
  std::vector <Command*> vCmds;

  for( std::map<std::string, Command*>::iterator It= _sCommands.begin();It!=_sCommands.end();It++)
  {
    if( strcmp(It->second->getGroupName(),sGrpName) == 0)
      vCmds.push_back(It->second);
  }

  return vCmds;
}

Command* CommandManager::getCommandByName(const char* sName)
{
  Command* pCom = NULL;
  if (_sCommands.find(sName) != _sCommands.end())
  {
    pCom = _sCommands[sName];
  }

  return pCom;
}

Command* CommandManager::getCommandByActionText(const char* sName)
{
  for( std::map<std::string, Command*>::iterator It= _sCommands.begin();It!=_sCommands.end();It++)
  {
    if (It->second->getAction())
    {
      if (It->second->getAction()->text() == sName)
      {
        return It->second;
      }
    }
  }

  return NULL;
}

void CommandManager::runCommandByName (const char* sName)
{
  Command* pCmd = getCommandByName(sName);

  if (pCmd)
    pCmd->activated();
}

void CommandManager::testActive(void)
{
  for( std::map<std::string, Command*>::iterator It= _sCommands.begin();It!=_sCommands.end();It++)
  {
    It->second->testActive();
  }
}
