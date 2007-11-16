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

#ifndef __Document_h__
#define __Document_h__



#include <Base/PyExport.h>
#include <Base/PyCXX/Objects.hxx>
#include <Base/Observer.h>
#include <Base/Persistance.h>
#include <Base/Type.h>

#include "PropertyContainer.h"
#include "PropertyStandard.h"
//#include "Transaction.h"


#include <map>
#include <vector>
#include <stack>

#include <boost/signals.hpp>
#include <boost/graph/adjacency_list.hpp>


#ifdef _MSC_VER
#	pragma warning( disable : 4251 )
#	pragma warning( disable : 4503 )
#	pragma warning( disable : 4786 )  // specifier longer then 255 chars
#	pragma warning( disable : 4290 )  // not implemented throw specification
#	pragma warning( disable : 4275 )
#endif

namespace Base {
  class Writer;
}

//class FCLabel;
namespace App
{
  class DocumentObject;
  class DocumentObjectExecReturn;
  class Document;
  class AbstractFeature;
  class DocumentPy; // the python document class
  class Application;
  class Transaction;
}



namespace App
{


//


/// The document class
class AppExport Document :public App::PropertyContainer//, public Base::Subject<const DocChanges&>
{
    PROPERTY_HEADER(App::Document);

public:

    /// holds the long name of the document (utf-8 coded)
    PropertyString Name;
    /// full qualified (with path) file name (utf-8 coded)
    PropertyString FileName;
    /// creators name (utf-8)
    PropertyString CreatedBy;
    PropertyString CreationDate;
    PropertyString LastModifiedBy;
    PropertyString LastModifiedDate;
    PropertyString Company;
    PropertyString Comment;

    /** @name Signals of the document */
    //@{
    /// signal on new Object
    boost::signal<void (App::DocumentObject&)> signalNewObject;
    //boost::signal<void (const App::DocumentObject&)>     m_sig;
    /// signal on deleted Object
    boost::signal<void (App::DocumentObject&)> signalDeletedObject;
    /// signal on changed Object
    boost::signal<void (App::DocumentObject&, App::Property&)> signalChangedObject;
    /// signal on renamed Object
    boost::signal<void (App::DocumentObject&)> signalRenamedObject;
    /// signal on activated Object
    boost::signal<void (App::DocumentObject&)> signalActivatedObject;
    /** signal on load/save document
     * this signal is given when the document gets streamed.
     * you can use this hook to write additional information in 
     * the file (like the Gui::Document it does).
     */
    boost::signal<void (Base::Writer &)> signalSaveDocument;
    boost::signal<void (Base::XMLReader&)> signalRestoreDocument;
    //@}

    /** @name File handling of the document */
    //@{
    /// Save the Document under a new Name
    //void saveAs (const char* Name);
    /// Save the document to the file in Property Path
    bool save (void);
    /// Restore the document from the file in Property Path
    void restore (void);
    /// Opens the document from its file name
    //void open (void);
    /// Is the document already saved to a file
    bool isSaved() const;
    /// Get the document name of a saved document 
    const char* getName() const {return Name.getValue();}
    /// Get the path of a saved document 
    //const char* getPath() const;
    //@}

    virtual void Save (Base::Writer &writer) const;
    virtual void Restore(Base::XMLReader &reader);

    /// returns the complet document mermory consumption, including all managed DocObjects and Undo Redo.
    unsigned int getMemSize (void) const;


    /** @name Object handling  */
    //@{
    /// Add a feature of sType with sName (ASCII) to this document and set it active. Unicode names are set through the Label propery
    DocumentObject *addObject(const char* sType, const char* pObjectName=0);
    /// Remove a feature out of the document
    void remObject(const char* sName);
    /// Returns the active Object of this document
    DocumentObject *getActiveObject(void) const;
    /// Returns a Object of this document
    DocumentObject *getObject(const char *Name) const;
    /// Returns a Name of an Object or 0
    const char *getObjectName(DocumentObject *pFeat) const;
    /// Returns a Name of an Object or 0
    std::string getUniqueObjectName(const char *Name) const;
    /// Returns a list of all Objects
    std::vector<DocumentObject*> getObjects() const;
    std::vector<DocumentObject*> getObjectsOfType(const Base::Type& typeId) const;
    int countObjectsOfType(const Base::Type& typeId) const;
    //@}


    /** @name methods for modification and state handling
     */
    //@{
    /// Remove all modifications. After this call The document becomes again Valid.
    void purgeTouched();
    /// check if there is any touched object in this document
    bool isTouched(void) const;
    /// returns all touched objects
    std::vector<App::DocumentObject *> getTouched(void) const;
    /// Recompute all touched features
    void recompute();
    /// Recompute only one feature
    void recomputeFeature(AbstractFeature* Feat);
    /// get the error log from the recompute run
    const std::vector<App::DocumentObjectExecReturn*> &getRecomputeLog(void)const{return _RecomputeLog;}
    /// get the text of the error of a spezified object
    const char* getErrorDescription(const App::DocumentObject*) const;
    //@}


    /** @name methods for the UNDO REDO and Transaction handling */
    //@{
    /// switch the level of Undo/Redo
    void setUndoMode(int iMode);  
    /// switch the level of Undo/Redo
    int getUndoMode(void) const;  
    /// switch the tranaction mode
    void setTransactionMode(int iMode);
    /// Open a new command Undo/Redo, an UTF-8 name can be specified
    void openTransaction(const char* name=0);
    // Commit the Command transaction. Do nothing If there is no Command transaction open.
    void commitTransaction();
    /// Abort the  actually runing transaction. 
    void abortTransaction();
    /// Set the Undo limit in Byte!
    void SetUndoLimit(unsigned int MemSize=0);
    /// Remove all stored Undos and Redos
    void clearUndos();
    /// Returns the  number  of stored Undos. If greater than 0 Undo will be effective.
    int getAvailableUndos() const;
    /// Returns a list of the Undo names
    std::vector<std::string> getAvailableUndoNames() const;
    /// Returns the actual memory consumption of the Undo redo stuff.
    unsigned int getUndoMemSize (void) const;
    /// Will UNDO  one step, returns  False if no undo was done (Undos == 0).
    bool undo();
    /// Returns the number of stored Redos. If greater than 0 Redo will be effective.
    int getAvailableRedos() const;
    /// Returns a list of the Redo names.
    std::vector<std::string> getAvailableRedoNames() const;
    /// Will REDO  one step, returns  False if no redo was done (Redos == 0).
    bool redo() ;
    //@}

    /** @name dependency stuff */
    //@{
    /// write GraphViz file
    void writeDependencyGraphViz(std::ostream &out);
    bool checkOnCycle(void);
    // set Changed
    //void setChanged(DocumentObject* change);
    //@}

    virtual PyObject *getPyObject(void);


    friend class DocumentPy;
    friend class Application;
    friend class AbstractFeature;
    /// because of transaction handling
    friend class DocumentObject;
    friend class Transaction;
    friend class TransactionObject;

    /// Destruction 
    virtual ~Document();

protected:

    /// Construction
    Document(void);

    void _remObject(DocumentObject* pcObject);
    void _addObject(DocumentObject* pcObject, const char* pObjectName);

    /// callback from the Document objects bevor property will be changed
    void onBevorChangeProperty(const DocumentObject *Who, const Property *What);
    /// callback from the Document objects after property was changed
    void onChangedProperty(const DocumentObject *Who, const Property *What);
    /// helper which Recompute only this feature
    bool _recomputeFeature(DocumentObject* Feat);

    // # Data Member of the document +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    int _iTransactionMode;
    int iTransactionCount;
    std::map<int,Transaction*> mTransactions;
    Transaction *activTransaction;
    bool bRollback;

    int _iUndoMode;
    std::list<Transaction*> mUndoTransactions;
    std::list<Transaction*> mRedoTransactions;
    Transaction *activUndoTransaction;
    void _clearRedos();


    DocumentObject* pActiveObject;
    std::map<std::string,DocumentObject*> ObjectMap;
    // Array to preserve the creation order of created objects
    std::vector<DocumentObject*> ObjectArray;

    // recompute log
    std::vector<App::DocumentObjectExecReturn*> _RecomputeLog;

    // pointer to the python class
    Py::Object DocumentPythonObject;
};


} //namespace App

#endif // __Document_h__
