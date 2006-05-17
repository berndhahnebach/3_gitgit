# Microsoft Developer Studio Project File - Name="AppImageGui" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=AppImageGui - Win32 Debug
!MESSAGE Dies ist kein g�ltiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und f�hren Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "AppImageGui.mak".
!MESSAGE 
!MESSAGE Sie k�nnen beim Ausf�hren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "AppImageGui.mak" CFG="AppImageGui - Win32 Debug"
!MESSAGE 
!MESSAGE F�r die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "AppImageGui - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AppImageGui - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AppImageGui - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "APPImageGUI_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "../../../" /D "NDEBUG" /D "_USRDLL" /D "FCAppImageGui" /Yu"PreCompiled.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 MSVCRT.LIB MSVCPRT.LIB kernel32.lib  python23.lib coin2.lib qt-mtnc321.lib opengl32.lib glu32.lib /nologo /dll /machine:I386 /nodefaultlib /out:"Release/ImageGui.pyd" /libpath:"$(QTDIR)\lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy  /y  /f  Release\ImageGui.pyd  ..\..\..\..\Mod\Image\ 	xcopy  /y  /f  ..\InitGui.py  ..\..\..\..\Mod\Image\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "AppImageGui - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "APPImageGUI_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "../../../" /D "_USRDLL" /D "FCAppImageGui" /D "FC_DEBUG" /FR /Yu"PreCompiled.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 MSVCRTD.LIB MSVCPRTD.LIB kernel32.lib  python23_d.lib coin2d.lib qt-mtnc321.lib opengl32.lib  glu32.lib /nologo /dll /debug /machine:I386 /nodefaultlib /out:"Debug/ImageGui_d.pyd" /pdbtype:sept /libpath:"$(QTDIR)\lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy  /y  /f  Debug\ImageGui_d.pyd  ..\..\..\..\Mod\Image\ 	xcopy  /y  /f  ..\InitGui.py  ..\..\..\..\Mod\Image\ 
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "AppImageGui - Win32 Release"
# Name "AppImageGui - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AppImageGui.cpp
# End Source File
# Begin Source File

SOURCE=.\AppImageGuiPy.cpp
# End Source File
# Begin Source File

SOURCE=.\Command.cpp
# End Source File
# Begin Source File

SOURCE=.\GLImageBox.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageView.cpp
# End Source File
# Begin Source File

SOURCE=.\Libs.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\PreCompiled.cpp
# ADD CPP /Yc"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\Workbench.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GLImageBox.h

!IF  "$(CFG)" == "AppImageGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\GLImageBox.h
InputName=GLImageBox

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "AppImageGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\GLImageBox.h
InputName=GLImageBox

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ImageView.h

!IF  "$(CFG)" == "AppImageGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\ImageView.h
InputName=ImageView

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "AppImageGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\ImageView.h
InputName=ImageView

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PreCompiled.h
# End Source File
# Begin Source File

SOURCE=.\Workbench.h
# End Source File
# End Group
# Begin Group "Dialogs"

# PROP Default_Filter ""
# End Group
# Begin Group "Libs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\lib\FreeCADApp.lib

!IF  "$(CFG)" == "AppImageGui - Win32 Release"

!ELSEIF  "$(CFG)" == "AppImageGui - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\lib\FreeCADAppD.lib

!IF  "$(CFG)" == "AppImageGui - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AppImageGui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\lib\FreeCADBase.lib

!IF  "$(CFG)" == "AppImageGui - Win32 Release"

!ELSEIF  "$(CFG)" == "AppImageGui - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\lib\FreeCADBaseD.lib

!IF  "$(CFG)" == "AppImageGui - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AppImageGui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\lib\FreeCADGui.lib

!IF  "$(CFG)" == "AppImageGui - Win32 Release"

!ELSEIF  "$(CFG)" == "AppImageGui - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\lib\FreeCADGuiD.lib

!IF  "$(CFG)" == "AppImageGui - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AppImageGui - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
