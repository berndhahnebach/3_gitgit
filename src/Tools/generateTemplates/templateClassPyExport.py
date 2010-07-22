#! python
# -*- coding: utf-8 -*-
# (c) 2006 Juergen Riegel 

import template,os
import generateBase.generateModel_Module
import generateBase.generateTools

class TemplateClassPyExport (template.ModelTemplate):
	def Generate(self):
		#self.ParentNamespace = "Base"
		#self.Namespace = "Base"
		print "TemplateClassPyExport",self.path + self.export.Name
		# Imp.cpp must not exist, neither in self.path nor in self.dirname
		if(not os.path.exists(self.path + self.export.Name + "Imp.cpp")):
			if(not os.path.exists(self.dirname + self.export.Name + "Imp.cpp")):
				file = open(self.path + self.export.Name + "Imp.cpp",'w')
				generateBase.generateTools.replace(self.TemplateImplement,locals(),file)
		file = open(self.path + self.export.Name + ".cpp",'w')
		generateBase.generateTools.replace(self.TemplateModule,locals(),file)
		file = open(self.path + self.export.Name + ".h",'w')
		generateBase.generateTools.replace(self.TemplateHeader,locals(),file)
		#file.write( generateBase.generateTools.replace(self.Template,locals()))

	TemplateHeader = """
// This file is generated by src/Tools/generateTemaplates/templateClassPyExport.py out of the XML file
// Every change you make here get lost at the next full rebuild!
#ifndef @self.export.Namespace.upper()@_@self.export.Name.upper()@_H
#define @self.export.Namespace.upper()@_@self.export.Name.upper()@_H

#include <@self.export.FatherInclude@>
#include <@self.export.Include@>
#include <string>

namespace @self.export.Namespace@
{

//===========================================================================
// @self.export.Name@ - Python wrapper
//===========================================================================

/** The python export class for @self.export.Twin@
 */
class @self.export.Namespace@Export @self.export.Name@ : public @self.export.FatherNamespace@::@self.export.Father@
{
public:
    static PyTypeObject   Type;
    static PyMethodDef    Methods[];
+ if (self.export.NumberProtocol):
    static PyNumberMethods Number[];
-
+ if (self.export.Sequence):
    static PySequenceMethods Sequence[];
-
+ if (self.export.RichCompare):
    static PyObject * richCompare(PyObject *v, PyObject *w, int op);
-
    static PyGetSetDef    GetterSetter[];
    static PyParentObject Parents[];
    virtual PyTypeObject *GetType(void) {return &Type;};
    virtual PyParentObject *GetParents(void) {return Parents;}

public:
    @self.export.Name@(@self.export.TwinPointer@ *pcObject, PyTypeObject *T = &Type);
    static PyObject *PyMake(struct _typeobject *, PyObject *, PyObject *);
    virtual int PyInit(PyObject* args, PyObject*k);
    ~@self.export.Name@();

    typedef @self.export.TwinPointer@* PointerType ;

    virtual PyObject *_repr(void);        // the representation
    std::string representation(void) const;

    /** @name callbacks and implementers for the python object methods */
    //@{
+ for i in self.export.Methode:
+ if i.Keyword:
    /// callback for the @i.Name@() method
    static PyObject * staticCallback_@i.Name@ (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the @i.Name@() method
    PyObject*  @i.Name@(PyObject *args, PyObject *kwd);
= else:
    /// callback for the @i.Name@() method
    static PyObject * staticCallback_@i.Name@ (PyObject *self, PyObject *args);
    /// implementer for the @i.Name@() method
    PyObject*  @i.Name@(PyObject *args);
-
-
    //@}

+ if (self.export.NumberProtocol):
    /** @name callbacks and implementers for the python object number protocol */
    //@{
    /// callback for the number_add_handler
    static PyObject * number_add_handler (PyObject *self, PyObject *other);
    /// callback for the number_subtract_handler
    static PyObject * number_subtract_handler (PyObject *self, PyObject *other);
    /// callback for the number_multiply_handler
    static PyObject * number_multiply_handler (PyObject *self, PyObject *other);
    //@}
-
+ if (self.export.Sequence):
    /** @name callbacks and implementers for the python object sequence protocol */
    //@{
+ if (self.export.Sequence.sq_length):
    static Py_ssize_t sequence_length(PyObject *);
-
+ if (self.export.Sequence.sq_concat):
    static PyObject* sequence_concat(PyObject *, PyObject *);
-
+ if (self.export.Sequence.sq_repeat):
    static PyObject * sequence_repeat(PyObject *, Py_ssize_t);
-
+ if (self.export.Sequence.sq_item):
    static PyObject * sequence_item(PyObject *, Py_ssize_t);
-
+ if (self.export.Sequence.sq_slice):
    static PyObject * sequence_slice(PyObject *, Py_ssize_t, Py_ssize_t);
-
+ if (self.export.Sequence.sq_ass_item):
    static int sequence_ass_item(PyObject *, Py_ssize_t, PyObject *);
-
+ if (self.export.Sequence.sq_ass_slice):
    static int sequence_ass_slice(PyObject *, Py_ssize_t, Py_ssize_t, PyObject *);
-
+ if (self.export.Sequence.sq_contains):
    static int sequence_contains(PyObject *, PyObject *);
-
+ if (self.export.Sequence.sq_inplace_concat):
    static PyObject* sequence_inplace_concat(PyObject *, PyObject *);
-
+ if (self.export.Sequence.sq_inplace_repeat):
    static PyObject * sequence_inplace_repeat(PyObject *, Py_ssize_t);
-
    //@}
-

    /** @name callbacks and implementers for the python object attributes */
    //@{
+ for i in self.export.Attribute:
    ///getter callback for the @i.Name@ attribute
    static PyObject * staticCallback_get@i.Name@ (PyObject *self, void *closure);
    /// getter for the @i.Name@ attribute
    Py::@i.Parameter.Type@ get@i.Name@(void) const;
    /// setter callback for the @i.Name@ attribute
    static int staticCallback_set@i.Name@ (PyObject *self, PyObject *value, void *closure);
+ if (i.ReadOnly):
    // no setter method,  @i.Name@ is read only!
= else:
    /// setter for the @i.Name@ attribute
    void set@i.Name@(Py::@i.Parameter.Type@ arg);
-
-
    //@}

+ if(self.export.CustomAttributes != None):
    /// getter method for special attributes (e.g. dynamic ones)
    PyObject *getCustomAttributes(const char* attr) const;
    /// setter for special attributes (e.g. dynamic ones)
    int setCustomAttributes(const char* attr, PyObject *obj);
    PyObject *_getattr(char *attr);              // __getattr__ function
    int _setattr(char *attr, PyObject *value);        // __setattr__ function
-

    /// getter for the object handled by this class
    @self.export.TwinPointer@ *get@self.export.Twin@Ptr(void) const;

+ if(self.export.ClassDeclarations != None):
    /** @name additional declarations and methods for the wrapper class */
    //@{
@self.export.ClassDeclarations@
    //@}
-
};

#define PARENTS@self.export.Namespace@@self.export.Name@ &@self.export.Name@::Type,PARENTS@self.export.FatherNamespace@@self.export.Father@

}  //namespace @self.export.Namespace@

#endif  // @self.export.Namespace.upper()@_@self.export.Name.upper()@_H

"""

	TemplateModule = """
// This file is generated by src/Tools/generateTemaplates/templateClassPyExport.py out of the .XML file
// Every change you make here get lost at the next full rebuild!
// This File is normaly build as an include in @self.export.Name@Imp.cpp! Its not intended to be in a project!

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include <Base/PyObjectBase.h>
#include <Base/Console.h>
#include <Base/Exception.h>
#include <CXX/Objects.hxx>

#define new DEBUG_CLIENTBLOCK

using Base::streq;
using namespace @self.export.Namespace@;

/// Type structure of @self.export.Name@
PyTypeObject @self.export.Name@::Type = {
    PyObject_HEAD_INIT(&PyType_Type)
    0,                                                /*ob_size*/
    "@self.export.Namespace@.@self.export.Twin@",     /*tp_name*/
    sizeof(@self.export.Name@),                       /*tp_basicsize*/
    0,                                                /*tp_itemsize*/
    /* methods */
    PyDestructor,                                     /*tp_dealloc*/
    0,                                                /*tp_print*/
    __getattr,                                        /*tp_getattr*/
    __setattr,                                        /*tp_setattr*/
    0,                                                /*tp_compare*/
    __repr,                                           /*tp_repr*/
+ if (self.export.NumberProtocol):
    @self.export.Namespace@::@self.export.Name@::Number,      /*tp_as_number*/
= else:
    0,                                                /*tp_as_number*/
-
+ if (self.export.Sequence):
    @self.export.Namespace@::@self.export.Name@::Sequence,      /*tp_as_sequence*/
= else:
    0,                                                /*tp_as_sequence*/
-
    0,                                                /*tp_as_mapping*/
    0,                                                /*tp_hash*/
    0,                                                /*tp_call */
    0,                                                /*tp_str  */
    0,                                                /*tp_getattro*/
    0,                                                /*tp_setattro*/
    /* --- Functions to access object as input/output buffer ---------*/
    0,                                                /* tp_as_buffer */
    /* --- Flags to define presence of optional/expanded features */
+ if (self.export.RichCompare):
    Py_TPFLAGS_BASETYPE|Py_TPFLAGS_HAVE_CLASS|Py_TPFLAGS_HAVE_RICHCOMPARE,        /*tp_flags */
= else:
    Py_TPFLAGS_BASETYPE|Py_TPFLAGS_HAVE_CLASS,        /*tp_flags */
-
    "@self.export.Documentation.UserDocu.replace('\\n','\\\\n\\"\\n    \\"')@",           /*tp_doc */
    0,                                                /*tp_traverse */
    0,                                                /*tp_clear */
+ if (self.export.RichCompare):
    @self.export.Namespace@::@self.export.Name@::richCompare,      /*tp_richcompare*/
= else:
    0,                                                /*tp_richcompare */
-
    0,                                                /*tp_weaklistoffset */
    0,                                                /*tp_iter */
    0,                                                /*tp_iternext */
    @self.export.Namespace@::@self.export.Name@::Methods,                     /*tp_methods */
    0,                                                /*tp_members */
    @self.export.Namespace@::@self.export.Name@::GetterSetter,                     /*tp_getset */
    &@self.export.FatherNamespace@::@self.export.Father@::Type,                        /*tp_base */
    0,                                                /*tp_dict */
    0,                                                /*tp_descr_get */
    0,                                                /*tp_descr_set */
    0,                                                /*tp_dictoffset */
    __PyInit,                                         /*tp_init */
    0,                                                /*tp_alloc */
    @self.export.Namespace@::@self.export.Name@::PyMake,/*tp_new */
    0,                                                /*tp_free   Low-level free-memory routine */
    0,                                                /*tp_is_gc  For PyObject_IS_GC */
    0,                                                /*tp_bases */
    0,                                                /*tp_mro    method resolution order */
    0,                                                /*tp_cache */
    0,                                                /*tp_subclasses */
    0,                                                /*tp_weaklist */
    0                                                 /*tp_del */
};

/// Methods structure of @self.export.Name@
PyMethodDef @self.export.Name@::Methods[] = {
+ for i in self.export.Methode:
    {"@i.Name@",
        (PyCFunction) staticCallback_@i.Name@,
+ if i.Keyword:
        METH_VARARGS|METH_KEYWORDS,
= else:
        METH_VARARGS,
-
        "@i.Documentation.UserDocu.replace('\\n','\\\\n')@"
    },
-
    {NULL, NULL, 0, NULL}		/* Sentinel */
};

+ if (self.export.NumberProtocol):
PyNumberMethods @self.export.Name@::Number[] = { {
    number_add_handler,
    number_subtract_handler,
    number_multiply_handler,
    NULL
} };
-

+ if (self.export.Sequence):
PySequenceMethods VectorPy::Sequence[] = { {
+ if (self.export.Sequence.sq_length):
    sequence_length,
= else:
    0,
-
+ if (self.export.Sequence.sq_concat):
    sequence_concat,
= else:
    0,
-
+ if (self.export.Sequence.sq_repeat):
    sequence_repeat,
= else:
    0,
-
+ if (self.export.Sequence.sq_item):
    sequence_item,
= else:
    0,
-
+ if (self.export.Sequence.sq_slice):
    sequence_slice,
= else:
    0,
-
+ if (self.export.Sequence.sq_ass_item):
    sequence_ass_item,
= else:
    0,
-
+ if (self.export.Sequence.sq_ass_slice):
    sequence_ass_slice,
= else:
    0,
-
+ if (self.export.Sequence.sq_contains):
    sequence_contains,
= else:
    0,
-
+ if (self.export.Sequence.sq_inplace_concat):
    sequence_inplace_concat,
= else:
    0,
-
+ if (self.export.Sequence.sq_inplace_repeat):
    sequence_inplace_repeat,
= else:
    0
-
} };
-

/// Attribute structure of @self.export.Name@
PyGetSetDef @self.export.Name@::GetterSetter[] = {
+ for i in self.export.Attribute:
    {"@i.Name@",
        (getter) staticCallback_get@i.Name@,
        (setter) staticCallback_set@i.Name@, 
        "@i.Documentation.UserDocu.replace('\\n','\\\\n')@",
        NULL
    },
-
    {NULL, NULL, NULL, NULL, NULL}		/* Sentinel */
};

+ for i in self.export.Methode:
// @i.Name@() callback and implementer
// PyObject*  @self.export.Name@::@i.Name@(PyObject *args){};
// has to be implemented in @self.export.Name@Imp.cpp
+ if i.Keyword:
PyObject * @self.export.Name@::staticCallback_@i.Name@ (PyObject *self, PyObject *args, PyObject * kwd)
= else:
PyObject * @self.export.Name@::staticCallback_@i.Name@ (PyObject *self, PyObject *args)
-
{
    // test if twin object not allready deleted
    if (!((PyObjectBase*) self)->isValid()){
        PyErr_SetString(PyExc_ReferenceError, "This object is already deleted most likely through closing a document. This reference is no longer valid!");
        return NULL;
    }

+	if (not i.Const):
    // test if object is set Const
    if (((PyObjectBase*) self)->isConst()){
        PyErr_SetString(PyExc_ReferenceError, "This object is immutable, you can not set any attribute or call a non const method");
        return NULL;
    }
-

    try { // catches all exeptions coming up from c++ and generate a python exeption
+ if i.Keyword:
        return ((@self.export.Name@*)self)->@i.Name@(args, kwd);
= else:
        return ((@self.export.Name@*)self)->@i.Name@(args);
-
    }
    catch(Base::Exception &e) // catch the FreeCAD exeptions
    {
        std::string str;
        str += "FreeCAD exception thrown (";
        str += e.what();
        str += ")";
        e.ReportException();
        PyErr_SetString(PyExc_Exception,str.c_str());
        return NULL;
    }
    catch(boost::filesystem::filesystem_error &e) // catch boost filesystem exeption
    {
        std::string str;
        str += "File system exeption thrown (";
        //str += e.who();
        //str += ", ";
        str += e.what();
        str += ")\\n";
        Base::Console().Error(str.c_str());
        PyErr_SetString(PyExc_Exception,str.c_str());
        return NULL;
    }
    catch(const Py::Exception&)
    {
        // The exception text is already set
        return NULL;
    }
    catch(const char *e) // catch simple string exceptions
    {
        Base::Console().Error(e);
        PyErr_SetString(PyExc_Exception,e);
        return NULL;
    }
    // in debug not all exceptions will be catched to get the attention of the developer!
#ifndef DONT_CATCH_CXX_EXCEPTIONS 
    catch(std::exception &e) // catch other c++ exeptions
    {
        std::string str;
        str += "FC++ exception thrown (";
        str += e.what();
        str += ")";
        Base::Console().Error(str.c_str());
        PyErr_SetString(PyExc_Exception,str.c_str());
        return NULL;
    }
    catch(...)  // catch the rest!
    {
        PyErr_SetString(PyExc_Exception,"Unknown C++ exception");
        return NULL;
    }
#endif
}

-
+ for i in self.export.Attribute:
// @i.Name@() callback and implementer
// PyObject*  @self.export.Name@::@i.Name@(PyObject *args){};
// has to be implemented in @self.export.Name@Imp.cpp
PyObject * @self.export.Name@::staticCallback_get@i.Name@ (PyObject *self, void * /*closure*/)
{
    if (!((PyObjectBase*) self)->isValid()){
        PyErr_SetString(PyExc_ReferenceError, "This object is already deleted most likely through closing a document. This reference is no longer valid!");
        return NULL;
    }

    try {
        return Py::new_reference_to(((@self.export.Name@*)self)->get@i.Name@());
    } catch (const Py::Exception&) {
        // The exception text is already set
        return NULL;
    } catch (...) {
        PyErr_SetString(PyExc_Exception, "Unknown exception while reading attribute '@i.Name@' of object '@self.export.Twin@'");
        return NULL;
    }
}

+ if (i.ReadOnly):
int @self.export.Name@::staticCallback_set@i.Name@ (PyObject *self, PyObject * /*value*/, void * /*closure*/)
{
    if (!((PyObjectBase*) self)->isValid()){
        PyErr_SetString(PyExc_ReferenceError, "This object is already deleted most likely through closing a document. This reference is no longer valid!");
        return -1;
    }

    PyErr_SetString(PyExc_AttributeError, "Attribute '@i.Name@' of object '@self.export.Twin@' is read-only");
    return -1;
}
= else:
int @self.export.Name@::staticCallback_set@i.Name@ (PyObject *self, PyObject *value, void * /*closure*/)
{    
    if (!((PyObjectBase*) self)->isValid()){
        PyErr_SetString(PyExc_ReferenceError, "This object is already deleted most likely through closing a document. This reference is no longer valid!");
        return -1;
    }
    if (((PyObjectBase*) self)->isConst()){
        PyErr_SetString(PyExc_ReferenceError, "This object is immutable, you can not set any attribute or call a method");
        return -1;
    }

    try {
        ((@self.export.Name@*)self)->set@i.Name@(Py::@i.Parameter.Type@(value,false));
        return 0;
    } catch (const Py::Exception&) {
        // The exception text is already set
        return -1;
    } catch (...) {
        PyErr_SetString(PyExc_Exception, "Unknown exception while writing attribute '@i.Name@' of object '@self.export.Twin@'");
        return -1;
    }
}
-

-


//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject @self.export.Name@::Parents[] = { PARENTS@self.export.Namespace@@self.export.Name@ };

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
@self.export.Name@::@self.export.Name@(@self.export.TwinPointer@ *pcObject, PyTypeObject *T)
    : @self.export.Father@(reinterpret_cast<@self.export.Father@::PointerType>(pcObject), T)
{
+ if (self.export.Reference):
    pcObject->ref();
-
}

+ if not (self.export.Constructor):
PyObject *@self.export.Name@::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // never create such objects with the constructor
    PyErr_SetString(PyExc_RuntimeError, "You cannot create directly an instance of '@self.export.Name@'.");
 
    return 0;
}

int @self.export.Name@::PyInit(PyObject* /*args*/, PyObject* /*kwd*/)
{
    return 0;
}
-

//--------------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------------
@self.export.Name@::~@self.export.Name@()                                // Everything handled in parent
{
+ if (self.export.Reference):
    get@self.export.Twin@Ptr()->unref();
-
+ if (self.export.Delete):
    // delete the handled object when the PyObject dies
    @self.export.Name@::PointerType ptr = reinterpret_cast<@self.export.Name@::PointerType>(_pcTwinPointer);
    delete ptr;
-
}

//--------------------------------------------------------------------------
// @self.export.Name@ representation
//--------------------------------------------------------------------------
PyObject *@self.export.Name@::_repr(void)
{
    return Py_BuildValue("s", representation().c_str());
}

+ if(self.export.CustomAttributes != None):
//--------------------------------------------------------------------------
// @self.export.Name@ Attributes
//--------------------------------------------------------------------------
PyObject *@self.export.Name@::_getattr(char *attr)				// __getattr__ function: note only need to handle new state
{
    try {
        // getter method for special Attributes (e.g. dynamic ones)
        PyObject *r = getCustomAttributes(attr);
        if(r) return r;
    }
#ifndef DONT_CATCH_CXX_EXCEPTIONS 
    catch(Base::Exception &e) // catch the FreeCAD exeptions
    {
        std::string str;
        str += "FreeCAD exception thrown (";
        str += e.what();
        str += ")";
        e.ReportException();
        PyErr_SetString(PyExc_Exception,str.c_str());
        return NULL;
    }
    catch(std::exception &e) // catch other c++ exeptions
    {
        std::string str;
        str += "FC++ exception thrown (";
        str += e.what();
        str += ")";
        Base::Console().Error(str.c_str());
        PyErr_SetString(PyExc_Exception,str.c_str());
        return NULL;
    }
    catch(const Py::Exception&)
    {
        // The exception text is already set
        return NULL;
    }
    catch(...)  // catch the rest!
    {
        PyErr_SetString(PyExc_Exception,"Unknown C++ exception");
        return NULL;
    }
#else  // DONT_CATCH_CXX_EXCEPTIONS  
    catch(Base::Exception &e) // catch the FreeCAD exeptions
    {
        std::string str;
        str += "FreeCAD exception thrown (";
        str += e.what();
        str += ")";
        e.ReportException();
        PyErr_SetString(PyExc_Exception,str.c_str());
        return NULL;
    }
    catch(const Py::Exception&)
    {
        // The exception text is already set
        return NULL;
    }
#endif  // DONT_CATCH_CXX_EXCEPTIONS

    _getattr_up(@self.export.Father@);
}

int @self.export.Name@::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
{
    try {
        // setter for  special Attributes (e.g. dynamic ones)
        int r = setCustomAttributes(attr, value);
        if(r==1) return 0;
    }
#ifndef DONT_CATCH_CXX_EXCEPTIONS 
    catch(Base::Exception &e) // catch the FreeCAD exeptions
    {
        std::string str;
        str += "FreeCAD exception thrown (";
        str += e.what();
        str += ")";
        e.ReportException();
        PyErr_SetString(PyExc_Exception,str.c_str());
        return -1;
    }
    catch(std::exception &e) // catch other c++ exeptions
    {
        std::string str;
        str += "FC++ exception thrown (";
        str += e.what();
        str += ")";
        Base::Console().Error(str.c_str());
        PyErr_SetString(PyExc_Exception,str.c_str());
        return -1;
    }
    catch(const Py::Exception&)
    {
        // The exception text is already set
        return -1;
    }
    catch(...)  // catch the rest!
    {
        PyErr_SetString(PyExc_Exception,"Unknown C++ exception");
        return -1;
    }
#else  // DONT_CATCH_CXX_EXCEPTIONS  
    catch(Base::Exception &e) // catch the FreeCAD exeptions
    {
        std::string str;
        str += "FreeCAD exception thrown (";
        str += e.what();
        str += ")";
        e.ReportException();
        PyErr_SetString(PyExc_Exception,str.c_str());
        return -1;
    }
    catch(const Py::Exception&)
    {
        // The exception text is already set
        return -1;
    }
#endif  // DONT_CATCH_CXX_EXCEPTIONS

    return @self.export.Father@::_setattr(attr, value);
}
-

@self.export.TwinPointer@ *@self.export.Name@::get@self.export.Twin@Ptr(void) const
{
    return static_cast<@self.export.TwinPointer@ *>(_pcTwinPointer);
}

#if 0
/* From here on come the methods you have to implement, but NOT in this module. Implement in @self.export.Name@Imp.cpp! This prototypes 
 * are just for convenience when you add a new method.
 */

+ if (self.export.Constructor):
PyObject *@self.export.Name@::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of @self.export.Name@ and the Twin object 
    return new @self.export.Name@(new @self.export.TwinPointer@);
}

// constructor method
int @self.export.Name@::PyInit(PyObject* /*args*/, PyObject* /*kwd*/)
{
    return 0;
}
-

// returns a string which represents the object e.g. when printed in python
std::string @self.export.Name@::representation(void) const
{
    return std::string("<@self.export.Twin@ object>");
}
+ for i in self.export.Methode:

+ if i.Keyword:
PyObject* @self.export.Name@::@i.Name@(PyObject *args, PyObject *kwds)
= else:
PyObject* @self.export.Name@::@i.Name@(PyObject *args)
-
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}
-

+ if (self.export.NumberProtocol):
PyObject* @self.export.Name@::number_add_handler(PyObject *self, PyObject *other)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* @self.export.Name@::number_subtract_handler(PyObject *self, PyObject *other)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* @self.export.Name@::number_multiply_handler(PyObject *self, PyObject *other)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}
-
+ if (self.export.Sequence):
+ if (self.export.Sequence.sq_length):

Py_ssize_t @self.export.Name@::sequence_length(PyObject *)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return -1;
}
-
+ if (self.export.Sequence.sq_concat):

PyObject* @self.export.Name@::sequence_concat(PyObject *, PyObject *)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}
-
+ if (self.export.Sequence.sq_repeat):

PyObject * @self.export.Name@::sequence_repeat(PyObject *, Py_ssize_t)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}
-
+ if (self.export.Sequence.sq_item):

PyObject * @self.export.Name@::sequence_item(PyObject *, Py_ssize_t)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}
-
+ if (self.export.Sequence.sq_slice):

PyObject * @self.export.Name@::sequence_slice(PyObject *, Py_ssize_t, Py_ssize_t)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}
-
+ if (self.export.Sequence.sq_ass_item):

int @self.export.Name@::sequence_ass_item(PyObject *, Py_ssize_t, PyObject *)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return -1;
}
-
+ if (self.export.Sequence.sq_ass_slice):

int @self.export.Name@::sequence_ass_slice(PyObject *, Py_ssize_t, Py_ssize_t, PyObject *)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return -1;
}
-
+ if (self.export.Sequence.sq_contains):

int @self.export.Name@::sequence_contains(PyObject *, PyObject *)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return -1;
}
-
+ if (self.export.Sequence.sq_inplace_concat):

PyObject* @self.export.Name@::sequence_inplace_concat(PyObject *, PyObject *)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}
-
+ if (self.export.Sequence.sq_inplace_repeat):

PyObject * @self.export.Name@::sequence_inplace_repeat(PyObject *, Py_ssize_t)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}
-
-

+ if (self.export.RichCompare):
PyObject* @self.export.Name@::richCompare(PyObject *v, PyObject *w, int op)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}
-
+ for i in self.export.Attribute:

Py::@i.Parameter.Type@ @self.export.Name@::get@i.Name@(void) const
{
    //return Py::@i.Parameter.Type@();
    throw Py::AttributeError("Not yet implemented");
}
+ if (i.ReadOnly):
= else:

void  @self.export.Name@::set@i.Name@(Py::@i.Parameter.Type@ arg)
{
    throw Py::AttributeError("Not yet implemented");
}
-
-
+ if(self.export.CustomAttributes != None):

PyObject *@self.export.Name@::getCustomAttributes(const char* attr) const
{
    return 0;
}

int @self.export.Name@::setCustomAttributes(const char* attr, PyObject *obj)
{
    return 0; 
}
-
#endif


"""	

	# here the template for the user part of the implementation. This gets NOT overridden when already existing
	TemplateImplement = """
#include "PreCompiled.h"

#include "@self.export.Include@"

// inclusion of the generated files (generated out of @self.export.Name@.xml)
#include "@self.export.Name@.h"
#include "@self.export.Name@.cpp"

using namespace @self.export.Namespace@;

// returns a string which represents the object e.g. when printed in python
std::string @self.export.Name@::representation(void) const
{
    return std::string("<@self.export.Twin@ object>");
}

+ if (self.export.Constructor):
PyObject *@self.export.Name@::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of @self.export.Name@ and the Twin object 
    return new @self.export.Name@(new @self.export.TwinPointer@);
}

// constructor method
int @self.export.Name@::PyInit(PyObject* /*args*/, PyObject* /*kwd*/)
{
    return 0;
}
-

+ for i in self.export.Methode:

+ if i.Keyword:
PyObject* @self.export.Name@::@i.Name@(PyObject * /*args*/, PyObject * /*kwds*/)
= else:
PyObject* @self.export.Name@::@i.Name@(PyObject * /*args*/)
-
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}
-

+ if (self.export.NumberProtocol):
PyObject* @self.export.Name@::number_add_handler(PyObject *self, PyObject *other)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* @self.export.Name@::number_subtract_handler(PyObject *self, PyObject *other)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* @self.export.Name@::number_multiply_handler(PyObject *self, PyObject *other)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}
-

+ if (self.export.Sequence):
+ if (self.export.Sequence.sq_length):

Py_ssize_t @self.export.Name@::sequence_length(PyObject *)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return -1;
}
-
+ if (self.export.Sequence.sq_concat):

PyObject* @self.export.Name@::sequence_concat(PyObject *, PyObject *)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}
-
+ if (self.export.Sequence.sq_repeat):

PyObject * @self.export.Name@::sequence_repeat(PyObject *, Py_ssize_t)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}
-
+ if (self.export.Sequence.sq_item):

PyObject * @self.export.Name@::sequence_item(PyObject *, Py_ssize_t)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}
-
+ if (self.export.Sequence.sq_slice):

PyObject * @self.export.Name@::sequence_slice(PyObject *, Py_ssize_t, Py_ssize_t)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}
-
+ if (self.export.Sequence.sq_ass_item):

int @self.export.Name@::sequence_ass_item(PyObject *, Py_ssize_t, PyObject *)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return -1;
}
-
+ if (self.export.Sequence.sq_ass_slice):

int @self.export.Name@::sequence_ass_slice(PyObject *, Py_ssize_t, Py_ssize_t, PyObject *)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return -1;
}
-
+ if (self.export.Sequence.sq_contains):

int @self.export.Name@::sequence_contains(PyObject *, PyObject *)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return -1;
}
-
+ if (self.export.Sequence.sq_inplace_concat):

PyObject* @self.export.Name@::sequence_inplace_concat(PyObject *, PyObject *)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}
-
+ if (self.export.Sequence.sq_inplace_repeat):

PyObject * @self.export.Name@::sequence_inplace_repeat(PyObject *, Py_ssize_t)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}
-
-

+ if (self.export.RichCompare):
PyObject* @self.export.Name@::richCompare(PyObject *v, PyObject *w, int op)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}
-

+ for i in self.export.Attribute:

Py::@i.Parameter.Type@ @self.export.Name@::get@i.Name@(void) const
{
    //return Py::@i.Parameter.Type@();
    throw Py::AttributeError("Not yet implemented");
}
+ if (i.ReadOnly):
= else:

void @self.export.Name@::set@i.Name@(Py::@i.Parameter.Type@ /*arg*/)
{
    throw Py::AttributeError("Not yet implemented");
}
-
-
+ if(self.export.CustomAttributes != None):

PyObject *@self.export.Name@::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int @self.export.Name@::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}
-

"""
