#ifndef __view_h__
#define __view_h__


#include <qmainwindow.h>
#include <qextmdichildview.h>

class FCGuiDocument;
class View3D;
class FCTree;
class QSplitter;
class QWidget;		
class QPushButton;	
class QVBoxLayout;	
class QHBoxLayout;	
class QWidgetStack;
class QTabBar;


/** Base class of all MDI View windows
 */
class FCView: public QextMdiChildView
{
	Q_OBJECT

public:
	FCView( FCGuiDocument* pcDocument, QWidget* parent, const char* name, int wflags=WDestructiveClose );
	~FCView();
	FCGuiDocument* getDocument();
	virtual const char *GetName(void)=0;

	/// Mesage handler
	virtual bool OnMsg(const char* pMsg);

protected:
    FCGuiDocument*	_pcDocument;


};


#endif