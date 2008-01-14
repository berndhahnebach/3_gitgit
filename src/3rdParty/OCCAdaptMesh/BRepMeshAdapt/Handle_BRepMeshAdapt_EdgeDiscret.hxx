// File generated by CPPExt (Transient)
//
//                     Copyright (C) 1991 - 2000 by  
//                      Matra Datavision SA.  All rights reserved.
//  
//                     Copyright (C) 2001 - 2004 by
//                     Open CASCADE SA.  All rights reserved.
// 
// This file is part of the Open CASCADE Technology software.
//
// This software may be distributed and/or modified under the terms and
// conditions of the Open CASCADE Public License as defined by Open CASCADE SA
// and appearing in the file LICENSE included in the packaging of this file.
//  
// This software is distributed on an "AS IS" basis, without warranty of any
// kind, and Open CASCADE SA hereby disclaims all such warranties,
// including without limitation, any warranties of merchantability, fitness
// for a particular purpose or non-infringement. Please see the License for
// the specific terms and conditions governing rights and limitations under the
// License.

#ifndef _Handle_BRepMeshAdapt_EdgeDiscret_HeaderFile
#define _Handle_BRepMeshAdapt_EdgeDiscret_HeaderFile

#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif
#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif

#ifndef _Handle_MMgt_TShared_HeaderFile
#include <Handle_MMgt_TShared.hxx>
#endif

class Standard_Transient;
class Handle_Standard_Type;
class Handle(MMgt_TShared);
class BRepMeshAdapt_EdgeDiscret;
Standard_EXPORT Handle_Standard_Type& STANDARD_TYPE(BRepMeshAdapt_EdgeDiscret);

class Handle(BRepMeshAdapt_EdgeDiscret) : public Handle(MMgt_TShared) {
  public:
    Handle(BRepMeshAdapt_EdgeDiscret)():Handle(MMgt_TShared)() {} 
    Handle(BRepMeshAdapt_EdgeDiscret)(const Handle(BRepMeshAdapt_EdgeDiscret)& aHandle) : Handle(MMgt_TShared)(aHandle) 
     {
     }

    Handle(BRepMeshAdapt_EdgeDiscret)(const BRepMeshAdapt_EdgeDiscret* anItem) : Handle(MMgt_TShared)((MMgt_TShared *)anItem) 
     {
     }

    Handle(BRepMeshAdapt_EdgeDiscret)& operator=(const Handle(BRepMeshAdapt_EdgeDiscret)& aHandle)
     {
      Assign(aHandle.Access());
      return *this;
     }

    Handle(BRepMeshAdapt_EdgeDiscret)& operator=(const BRepMeshAdapt_EdgeDiscret* anItem)
     {
      Assign((Standard_Transient *)anItem);
      return *this;
     }

    BRepMeshAdapt_EdgeDiscret* operator->() const
     {
      return (BRepMeshAdapt_EdgeDiscret *)ControlAccess();
     }

//   Standard_EXPORT ~Handle(BRepMeshAdapt_EdgeDiscret)();
 
   Standard_EXPORT static const Handle(BRepMeshAdapt_EdgeDiscret) DownCast(const Handle(Standard_Transient)& AnObject);
};
#endif
