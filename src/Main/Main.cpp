#include "../Base/Export.h"

#pragma warning( disable : 4786 )

#include <stdio.h>

#ifndef __linux
#include <direct.h>
#include <windows.h>
#endif


// FreeCAD Base header
#include "../Base/Console.h"
#include "../Base/Interpreter.h"
#include "../Base/Parameter.h"
#include "../Base/Exception.h"

// FreeCAD doc header
#include "../App/Application.h"

#include "Standard_Failure.hxx"
#include "iostream"

// FreeCAD Gui header
#define _FC_GUI_ENABLED_

#ifdef  _FC_GUI_ENABLED_
#  include <qapplication.h>
#  include "DlgSplasher.h"
#  include "../Gui/Application.h"
#  ifdef WNT
#    pragma comment(lib,"qt-mt230nc.lib")
#  endif 
#endif



const char sBanner[] = \
"  #####                 ####  ###   ####  \n" \
"  #                    #      # #   #   # \n" \
"  #     ##  #### ####  #     #   #  #   # \n" \
"  ####  # # #  # #  #  #     #####  #   # \n" \
"  #     #   #### ####  #    #     # #   # \n" \
"  #     #   #    #     #    #     # #   #  ##  ##  ##\n" \
"  #     #   #### ####   ### #     # ####   ##  ##  ##\n\n" ;


#ifndef __linux
// scriptings
#include "InitScript.h"
#include "TestScript.h"
#include "TestEnvScript.h"
#else
// this might be a cleaner approach? (Besides path to scripts)
const char FreeCADInit[]="execfile('./Main/FreeCADInit.py')";
const char FreeCADTest[]="execfile('./Main/FreeCADTest.py')";
const char FreeCADTestEnv[]="execfile('./Main/FreeCADTestEnv.py')";
#endif

// globals
FCParameter *pcGlobalParameter;

// run control
int RunMode = 0;
stlport::string sFileName;
const char*     sScriptName;

QApplication* pcQApp;
// forwards
void Init(int argc, char ** argv );
void ParsOptions(int argc, char ** argv);
void CheckEnv(void);

void PrintInitHelp(void)
{
	cerr << endl << endl
		 << "  An Initializing error was cought. That means mainly" << endl
		 << "  FreeCAD is not proper installed. Type \"FreeCAD -TestEnvironment\""<< endl
		 << "  to gets hints whats wrong." << endl << endl
		 << "  Good luck ;-)" << endl << endl;
}

int main( int argc, char ** argv ) {

	// Init phase ===========================================================
	try{
		// first check the environment variables
		CheckEnv();

		// Ínitialization (phase 1)
		Init(argc,argv);

		// the FreeCAD Application

		GetApplication();
	}
	catch(Standard_Failure e)
	{
		Handle(Standard_Failure) E = Standard_Failure::Caught();
		cout << "An exception was caught " << E << endl;
		PrintInitHelp();
		exit(1);
	}
	catch(FCException e)
	{
		GetConsole().Error("Application init fails:");
		e.ReportException();
		PrintInitHelp();
		exit(2);
	}
	catch(...)
	{
		GetConsole().Error("Application init fails, because of a realy nesty (unknown) error...");
		PrintInitHelp();
		exit(3);
	}


	// Run phase ===========================================================

	int ret;
	try{
		switch(RunMode)
		{
		case 0:{
		// run GUI
	#		ifdef _FC_GUI_ENABLED_
				// A new QApplication
				GetConsole().Log("Creating GUI Application...\n");

				ApplicationWindow * mw = new ApplicationWindow();
				pcQApp->setMainWidget(mw);
				ApplicationWindow::Instance->setCaption( "FreeCAD" );

				GetConsole().Log("Showing GUI Application...\n");
				mw->show();
				pcQApp->connect( pcQApp, SIGNAL(lastWindowClosed()), pcQApp, SLOT(quit()) );
				// run the Application event loop
				GetConsole().Log("Running event loop...\n");
				ret = pcQApp->exec();
				GetConsole().Log("event loop left\n");

	#		else
				GetConsole().Error("GUI mode not possible. This is a FreeCAD compiled without GUI. Use FreeCAD -c\n");
	#		endif
				break;
			}
		case 1:
			// Run the comandline interface
			ret = GetInterpreter().RunCommandLine("Console mode");
			break;
		case 2:
			// run a script
			GetConsole().Log("Runing script: %s\n",sFileName.c_str());
			GetInterpreter().LaunchFile(sFileName.c_str());
			break;
		case 3:
			// run internal script
			GetConsole().Log("Runing internal script: %s\n",sFileName.c_str());
			GetInterpreter().Launch(sScriptName);
			break;
		default:
			assert(0);
		}
	}
	catch(Standard_Failure e)
	{
		GetConsole().Error("Running the application fails, OCC exception caught:");
		Handle(Standard_Failure) E = Standard_Failure::Caught();
		cout << "An exception was caught " << E << endl;
		exit(4);
	}
	catch(FCException e)
	{
		GetConsole().Error("Running the application fails:");
		e.ReportException();
		exit(5); 
	}
	catch(...)
	{
		GetConsole().Error("Running the application fails, because of a realy nesty (unknown) error...");
		exit(6);
	}

	// Destruction phase ===========================================================
#ifdef __linux // what if we are running in a GUI-less mode?
        if (pcQApp)
#endif        
        delete pcQApp;
	delete pcGlobalParameter;

	GetConsole().Log("FreeCAD completly terminated\n\n");
	return 0;
}


//************************************************************************
// Init()
// Initialize all the stuff and running the init script:
// - Launching the FCInterpreter (starting python)
// - Launching the FCConsole
// - Call the Init script ("(FreeCADDir)/scripts/FreeCADInit.py")
//************************************************************************
void Init(int argc, char ** argv )
{
	// init python
	GetInterpreter();
	// std console (Also init the python bindings)
	GetConsole().AttacheObserver(new FCCmdConsoleObserver());
	// file logging fcility
#	ifdef _DEBUG
		GetConsole().AttacheObserver(new FCLoggingConsoleObserver("FreeCAD.log"));
#	endif

	// Banner
	GetConsole().Message("FreeCAD (c) 2001 Juergen Riegel\n\n%s",sBanner);

	pcGlobalParameter = new FCParameter();

	ParsOptions(argc,argv);

	/*
	if(RunMode=0)
	{*/
		pcQApp = new QApplication ( argc, argv );
	/*	SplasherDialog * pcSplasher = new SplasherDialog();
		//pcQApp->setMainWidget(pcSplasher);
		pcSplasher->show();
	}*/

	// Start the python interpreter
	FCInterpreter &rcInterperter = GetInterpreter();
	rcInterperter.SetComLineArgs(argc,argv);
	rcInterperter.Launch("print 'Python started'\n");
	
	// starting the init script
	rcInterperter.Launch(FreeCADInit);

	/*
	if(RunMode=0)
	{
		delete pcSplasher;
	}*/

}

const char sEnvErrorText[] = \
"It seems some of the variables needed by FreeCAD are not set\n"\
"or wrong set. This can happen when you start FreeCAD without\n"\
"installation or you runnig mixed versions. Also while development\n"\
"process. You can set the variables by hand or with a script:\n"\
"CSF_GRAPHICSHR=C:\\CasRoot\\Windows_NT\\dll\\opengl.dll\n"\
"CSF_MDTVFONTDIRECTORY=C:\\CasRoot\\src\\FontMFT\\\n"\
"CSF_MDTVTEXTURESDIRECTORY=C:\\CasRoot\\src\\Textures\\\n"\
"CSF_UNITSDEFINITION=C:\\CasRoot\\src\\UnitsAPI\\Units.dat\n"\
"CSF_UNITSLEXICON=C:\\CasRoot\\src\\UnitsAPI\\Lexi_Expr.dat\n"\
"CSF_PluginDefaults=C:\\Programme\\FreeCAD\\Resources\\\n"\
"CSF_StandardDefaults=C:\\Programme\\FreeCAD\\Resources\\\n\n"\
"You can also use a script providet with the source and the binary\n"\
"distribution of FreeCAD -> SetEnvs.py, which try to figure it out\n"\
"automaticaly.\n\n";


void CheckEnv(void)
{
	bool bFailure = false;
	// set the resource env variables
/*  dont work!!! keeps the path from registry
	char  szString [256] ;
	char  szDirectory [256] ;

	_getcwd (szDirectory,sizeof szDirectory);
	if (szDirectory[strlen(szDirectory)-1] != '\\') {
		strcat(szDirectory,"\\");
	}
	
	SetEnvironmentVariable ( "CSF_ResourcesDefaults",szDirectory);
	sprintf(szString,"CSF_ResourcesDefaults=%s",szDirectory);
	putenv (szString);
	cout<<szString<<endl;

	SetEnvironmentVariable ( "CSF_PluginDefaults",szDirectory);
	sprintf(szString,"CSF_PluginDefaults=%s",szDirectory);
	putenv (szString);
	cout<<szString<<endl;
*/

/*  Attic ?
	if( ! getenv("CSF_GRAPHICSHR") ){
		GetConsole().Message("Environment (CSF_GRAPHICSHR) not set!\n");
		bFailure = true;
	}*/
        
        
#define TEST_ENVVAR_EXISTS(envvar) \
	if (!getenv(envvar)){ \
          cerr<<"Environment variable "<<envvar<<" is not set!"<<endl; \
          bFailure=true;\
        }  
        //TEST_ENVVAR_EXISTS("CSF_GraphicShr")
        TEST_ENVVAR_EXISTS("CSF_MdtvFontDirectory")
        TEST_ENVVAR_EXISTS("CSF_MdtvTexturesDirectory")
        TEST_ENVVAR_EXISTS("CSF_UnitsDefinition")
        TEST_ENVVAR_EXISTS("CSF_UnitsLexicon")
        TEST_ENVVAR_EXISTS("CSF_PluginDefaults")
        TEST_ENVVAR_EXISTS("CSF_StandardDefaults")
        if (bFailure) {    
         	cerr<<"Environment Error(s)"<<endl<<sEnvErrorText;
		exit(0);
        }
#undef TEST_ENVVAR_EXISTS         
        
/*	if( ! getenv("CSF_MDTVFONTDIRECTORY") ){
		printf("Environment (CSF_MDTVFONTDIRECTORY) not set!\n");
		bFailure = true;
	}
	if( ! getenv("CSF_MDTVTEXTURESDIRECTORY") ){
		printf("Environment (CSF_MDTVTEXTURESDIRECTORY) not set!\n");
		bFailure = true;
	}
	if( ! getenv("CSF_UNITSDEFINITION") ){
		printf("Environment (CSF_UNITSDEFINITION) not set!\n");
		bFailure = true;
	}
	if( ! getenv("CSF_UNITSLEXICON") ){
		printf("Environment (CSF_UNITSLEXICON) not set!\n");
		bFailure = true;
	}
	if( ! getenv("CSF_PluginDefaults") ){
		printf("Environment (CSF_PluginDefaults) not set!\n");
		bFailure = true;
	}
	if( ! getenv("CSF_StandardDefaults") ){
		printf("Environment (CSF_StandardDefaults) not set!\n");
		bFailure = true;
	}

	if(bFailure){
		printf("Environment Error(s)\n%s",sEnvErrorText);
		exit(0);
	}  */

/*	
	if(  getenv("CASROOT") )
	{
		RSstring CasRoot = getenv("CASROOT");
		TRACE("Environment CASROOT defined, using Cascade from %s\n",CasRoot.c_str());
		_putenv(("CSF_GRAPHICSHR="+CasRoot+"\\Windows_NT\\dll\\opengl.dll").c_str());
		_putenv(("CSF_MDTVFONTDIRECTORY="+CasRoot+"\\src\\FontMFT\\").c_str());
		_putenv(("CSF_MDTVTEXTURESDIRECTORY="+CasRoot+"\\src\\Textures\\").c_str());
		_putenv(("CSF_UNITSDEFINITION="+CasRoot+"\\src\\UnitsAPI\\Units.dat").c_str());
		_putenv(("CSF_UNITSLEXICON="+CasRoot+"\\src\\UnitsAPI\\Lexi_Expr.dat").c_str());
	}else
	{
		TRACE("Environment CASROOT NOT defined, using Cascade from intallation\n");
		_putenv("CSF_GRAPHICSHR=.\\opengl.dll");
		TRACE("CSF_GRAPHICSHR=%s\n",getenv("CSF_GRAPHICSHR"));
		_putenv("CSF_MDTVFONTDIRECTORY=.\\data\\");
		_putenv("CSF_MDTVTEXTURESDIRECTORY=.\\data\\");
		_putenv("CSF_UNITSDEFINITION=.\\data\\Units.dat");
		_putenv("CSF_UNITSLEXICON=.\\data\\Lexi_Expr.dat");
	}
*/

}

const char Usage[] = \
" [Options] files..."\
"Options:\n"\
"  -h             Display this information "\
"  -c             Runs FreeCAD in console mode (no windows)\n"\
"  -cf file-name  Runs FreeCAD in server mode with script file-name\n"\
"  -te            Runs FreeCAD to test environment\n"\
"  -t0            Runs FreeCAD self test function\n"\
"\n consult also the HTML documentation\n"\
"";


void ParsOptions(int argc, char ** argv)
{
	// scan command line arguments for user input. 
	for (int i = 1; i < argc; i++) 
	{ 
		if (*argv[i] == '-' ) 
		{ 
			switch (argv[i][1]) 
			{ 
			// Console modes 
			case 'c': 
			case 'C':  
				switch (argv[i][2])  
				{   
					// Console with file
					case 'f':  
					case 'F':  
						RunMode = 2;
						if(argc <= i+1)
						{
							GetConsole().Error("Expecting a file\n");  
							GetConsole().Error("\nUsage: %s %s",argv[0],Usage);  
						}
						sFileName = argv[i+1];
						i++;
						break;   
					case '\0':  
						RunMode = 1;
						break;   
					default:  
						GetConsole().Error("Invalid Input %s\n",argv[i]);  
						GetConsole().Error("\nUsage: %s %s",argv[0],Usage);  
						throw FCException("Comandline error(s)");  
				};  
				break;  
			case 't': 
			case 'T':  
				switch (argv[i][2])  
				{   
					// run the test environment script
					case 'e':  
					case 'E':  
						RunMode = 3;
						sFileName = "TestEnv";
						sScriptName = FreeCADTestEnv;
						break;   
					case '0':  
						// test script level 0
						RunMode = 3;
						sFileName = "Test";
						sScriptName = FreeCADTest;
						break;   
					default:  
						//default testing level 0
						RunMode = 3;
						sFileName = "Test";
						sScriptName = FreeCADTest;
						break;   
				};  
				break;  
			case '?': 
			case 'h': 
			case 'H': 
				GetConsole().Message("\nUsage: %s %s",argv[0],Usage);
				throw FCException("Comandline break");  
				break;  
			default: 
				GetConsole().Error("Invalid Option: %s\n",argv[i]); 
				GetConsole().Error("\nUsage: %s %s",argv[0],Usage); 
				throw FCException("Comandline error(s)");  
			} 
		} 
		else  
		{ 
			GetConsole().Error("Illegal command line argument #%d, %s\n",i,argv[i]); 
			GetConsole().Error("\nUsage: %s %s",argv[0],Usage); 
			throw FCException("Comandline error(s)");  
		} 
	}  
}  



