/***************************************************************************
 *   Copyright (c) Riegel         <juergen.riegel@web.de>                  *
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


#ifndef __Base_H__
#define __Base_H__

#include "Type.h"


/// define for subclassing Base::BaseClass
#define TYPESYSTEM_HEADER() \
public: \
  static Base::Type getClassTypeId(void); \
  virtual Base::Type getTypeId(void) const; \
  static void init(void);\
  static void *create(void);\
private: \
  static Base::Type classTypeId




/// define to implement a  subclass of Base::BaseClass
#define TYPESYSTEM_SOURCE_P(_class_) \
Base::Type _class_::getClassTypeId(void) { return _class_::classTypeId; } \
Base::Type _class_::getTypeId(void) const { return _class_::classTypeId; } \
Base::Type _class_::classTypeId = Base::Type::badType();  \
void * _class_::create(void){\
   return new _class_ ();\
}

/// define to implement a  subclass of Base::BaseClass
#define TYPESYSTEM_SOURCE_ABSTRACT_P(_class_) \
Base::Type _class_::getClassTypeId(void) { return _class_::classTypeId; } \
Base::Type _class_::getTypeId(void) const { return _class_::classTypeId; } \
Base::Type _class_::classTypeId = Base::Type::badType();  \
void * _class_::create(void){return 0;}


/// define to implement a subclass of Base::BaseClass
#define TYPESYSTEM_SOURCE(_class_, _parentclass_) \
TYPESYSTEM_SOURCE_P(_class_);\
void _class_::init(void){\
  initSubclass(_class_::classTypeId, #_class_ , #_parentclass_, &(_class_::create) ); \
}

/// define to implement a subclass of Base::BaseClass
#define TYPESYSTEM_SOURCE_ABSTRACT(_class_, _parentclass_) \
TYPESYSTEM_SOURCE_ABSTRACT_P(_class_);\
void _class_::init(void){\
  initSubclass(_class_::classTypeId, #_class_ , #_parentclass_, &(_class_::create) ); \
}

namespace Base
{
  //typedef struct _object PyObject;


/// BaseClass class and root of the type system
class BaseExport BaseClass
{
public: 
  static Type getClassTypeId(void); 
  virtual Type getTypeId(void) const; 
  static void init(void);

  virtual PyObject *getPyObject(void);
  virtual void setPyObject(PyObject *);

  static void *create(void){return 0;}
private: 
  static Type classTypeId;
protected:
  static void initSubclass(Base::Type &toInit,const char* ClassName, const char *ParentName, Type::instantiationMethod method=0);

public:
  /// Construction
  BaseClass();
  /// Destruction
  virtual ~BaseClass();

};





} //namespace Base

#endif // __FILETEMPLATE_H__

