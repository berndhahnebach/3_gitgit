/***************************************************************************
 *   Copyright (c) 2004 J�rgen Riegel <juergen.riegel@web.de>              *
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


#ifndef GUI_MACRO_H
#define GUI_MACRO_H

// Std. configurations
#include <QString>
#include <Base/Observer.h>
#include <Base/Parameter.h>


namespace Gui {
struct ApplicationP;
class PythonConsole;

/** Macro recording and play back management
 * The purpos of this class is to handle record function calls from a command and save it in
 * a macro file (so far).
 * \author J�rgen Riegel
 */
class GuiExport MacroManager: public Base::Observer<const char*> 
{
protected:
    MacroManager();
    ~MacroManager();

public:

    /** Macro type enumeration  */
    enum MacroType { 
        File, /**< The macro will be saved in a file */  
        App,  /**< The macro belongs to the Application and will be saved in the UserParameter */  
        Doc   /**< Teh macro belongs to the Document and will be saved and restored with the Document */  
    }; 

    /** Line type enumeration  */
    enum LineType { 
        Base,  /**< The line effects only the document and Application (FreeCAD) */  
        Gui,   /**< The line effects the Gui (FreeCADGui) */
    }; 

    /** Opens a new Macro recording sassion
     * Starts a sassion with the Type and the Name of the Macro.
     * All user interactions will be recorded as long as the commit() or cancel() is called. 
     * There is only one recording sassion possible. To open a second one cause a exception:
     * @param eType Type of the macro
     * @param sName Name or Path of the Macro
     * @see commit()
     * @see cancel()
     */
    void open(MacroType eType,const char *sName);
    /// close (and save) the recording session
    void commit(void);
    /// cancels the recording session
    void cancel(void);
    /// indicates if a macro recording in in progress
    bool isOpen(void){return openMacro;}
    /// insert a new line in the macro
    void addLine(LineType Type,const char* sLine);
    /** Set the active module 
     * This is normaly done by the workbench switch. It sets
     * the actually active application module so when the macro
     * gets started the right import can be issued.
     */
    void setModule(const char* sModule);
    void run(MacroType eType,const char *sName);
    /** Observes its parameter group. */
    void OnChange(Base::Subject<const char*> &rCaller, const char * sReason);

protected:
    QString macroInProgress;        /**< Container for the macro */
    QString macroName;              /**< name of the macro */
    bool openMacro;
    bool recordGui;
    bool guiAsComment;
    bool scriptToPyConsole;
    PythonConsole* pyConsole;       // link to the python console
    Base::Reference<ParameterGrp> params;  // link to the Macro parameter group

    friend struct ApplicationP;
};

} // namespace Gui


#endif // GUI_MACRO_H
