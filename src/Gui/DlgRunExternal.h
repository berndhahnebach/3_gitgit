/***************************************************************************
 *   Copyright (c) 2009 J�rgen Riegel <juergen.riegel@web.de>              *
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


#ifndef GUI_DIALOG_DlgRunExternal_H
#define GUI_DIALOG_DlgRunExternal_H

#include <Gui/ui_DlgRunExternal.h>

#include <QProcess>

namespace Gui {
namespace Dialog {

/**
 * The DlgRunExternal class implements a dialog to start and control external
 * programms to edit FreeCAD controled content.
 * \author J�rgen Riegel
 */
class GuiExport DlgRunExternal : public QDialog, public Ui_DlgRunExternal 
{ 
    Q_OBJECT

public:
    DlgRunExternal( QWidget* parent = 0, Qt::WFlags fl = 0 );
    virtual ~DlgRunExternal();

	int Do(void);

	QString ProcName;
	QStringList arguments;

protected Q_SLOTS:
    virtual void reject (void);
	virtual void accept (void);
	virtual void abort  (void);
	virtual void advanced  (void);
	void finished ( int exitCode, QProcess::ExitStatus exitStatus );
protected:
	QProcess process;
	bool advancedHidden;

};

} // namespace Dialog
} // namespace Gui

#endif // GUI_DIALOG_DlgRunExternal_H
