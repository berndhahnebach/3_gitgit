# FreeCAD install script of the part module  
# (c) 2001 J�rgen Riegel
#
# Searching and installing new packages and set up the environment at first
# start.

#***************************************************************************
#*   (c) J�rgen Riegel (juergen.riegel@web.de) 2002                        *   
#*                                                                         *
#*   This file is part of the FreeCAD CAx development system.              *
#*                                                                         *
#*   This program is free software; you can redistribute it and/or modify  *
#*   it under the terms of the GNU General Public License (GPL)            *
#*   as published by the Free Software Foundation; either version 2 of     *
#*   the License, or (at your option) any later version.                   *
#*   for detail see the LICENCE text file.                                 *
#*                                                                         *
#*   FreeCAD is distributed in the hope that it will be useful,            *
#*   but WITHOUT ANY WARRANTY; without even the implied warranty of        * 
#*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
#*   GNU Library General Public License for more details.                  *
#*                                                                         *
#*   You should have received a copy of the GNU Library General Public     *
#*   License along with FreeCAD; if not, write to the Free Software        * 
#*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
#*   USA                                                                   *
#*                                                                         *
#*   Juergen Riegel 2002                                                   *
#***************************************************************************/

# Get the Parameter Group of this module
ParGrp = App.ParamGet("Modules").GetGroup("Part")

# Set the needed information
ParGrp.SetString("HelpIndex","Part/Help/index.html")
ParGrp.SetString("DocTemplateName","Part")
ParGrp.SetString("DocTemplateScript","TemplPart.py")
ParGrp.SetString("WorkBenchName","Part Design")
ParGrp.SetString("WorkBenchModule","PartWorkbench.py")

# mark the installation done
ParGrp.SetBool("Installed",1)