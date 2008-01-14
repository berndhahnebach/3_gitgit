// File generated by CPPExt (Value)
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

#ifndef _BRepMeshAdapt_IncrementalMesh_HeaderFile
#define _BRepMeshAdapt_IncrementalMesh_HeaderFile

#ifndef _Standard_Real_HeaderFile
#include <Standard_Real.hxx>
#endif
#ifndef _Standard_Boolean_HeaderFile
#include <Standard_Boolean.hxx>
#endif
#ifndef _TopTools_MapOfShape_HeaderFile
#include <TopTools_MapOfShape.hxx>
#endif
#ifndef _Handle_BRepMeshAdapt_FastDiscret_HeaderFile
#include <Handle_BRepMeshAdapt_FastDiscret.hxx>
#endif
#ifndef _BRepMeshAdapt_DataMapOfShapeReal_HeaderFile
#include <BRepMeshAdapt_DataMapOfShapeReal.hxx>
#endif
#ifndef _TopoDS_Shape_HeaderFile
#include <TopoDS_Shape.hxx>
#endif
#ifndef _TopTools_IndexedDataMapOfShapeListOfShape_HeaderFile
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#endif
#ifndef _Bnd_Box_HeaderFile
#include <Bnd_Box.hxx>
#endif

#ifndef _TriangleAdapt_Parameters_HeaderFile
#include <TriangleAdapt_Parameters.h>
#endif

class BRepMeshAdapt_FastDiscret;
class TopoDS_Shape;
class TopoDS_Edge;
class TopoDS_Face;


#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

//! Builds the mesh of a shape with respect of their <br>
//!          correctly triangulated parts <br>
//! <br>
class BRepMeshAdapt_IncrementalMesh  {

public:

    void* operator new(size_t,void* anAddress) 
      {
        return anAddress;
      }
    void* operator new(size_t size) 
      { 
        return Standard::Allocate(size); 
      }
    void  operator delete(void *anAddress) 
      { 
        if (anAddress) Standard::Free((Standard_Address&)anAddress); 
      }
 // Methods PUBLIC
 // 


Standard_EXPORT BRepMeshAdapt_IncrementalMesh();

//! if the  boolean    <Relatif>   is  True,    the <br>
//!          deflection used   for the polygonalisation   of <br>
//!          each edge will be <D> * Size of Edge. <br>
//!          the deflection used for the faces will be the maximum <br>
//!          deflection of their edges. <br>
Standard_EXPORT BRepMeshAdapt_IncrementalMesh(const TopoDS_Shape& S,const Standard_Real D,const Standard_Boolean Relatif = Standard_False,const Standard_Real Ang = 0.5);

Standard_EXPORT BRepMeshAdapt_IncrementalMesh(const TopoDS_Shape& S,const Standard_Real D,const TriangleAdapt_Parameters& params,const Standard_Boolean Rel = Standard_False, const Standard_Real Ang = 0.5);

Standard_EXPORT   void SetDeflection(const Standard_Real D) ;


Standard_EXPORT   void SetAngle(const Standard_Real Ang) ;


Standard_EXPORT   void SetRatio(const Standard_Real R) ;

//! Builds the incremental mesh of the shape <br>
Standard_EXPORT   void Update(const TopoDS_Shape& S) ;


Standard_EXPORT   Standard_Boolean IsModified() const;





protected:

 // Methods PROTECTED
 // 


 // Fields PROTECTED
 //


private: 

 // Methods PRIVATE
 // 

//! Locate a correct discretisation if it exists <br>
//!          Set no one otherwise <br>
Standard_EXPORT   void Update(const TopoDS_Edge& E) ;

//! if the face is  not correctly triangulated, or <br>
//!          if  one  of its edges  is  to be discretisated <br>
//!          correctly, the triangulation  of this face  is <br>
//!          built. <br>
Standard_EXPORT   void Update(const TopoDS_Face& F) ;


 // Fields PRIVATE
 //
Standard_Real myDeflection;
Standard_Real myAngle;
Standard_Boolean myRelative;
TopTools_MapOfShape myMap;
Handle_BRepMeshAdapt_FastDiscret myMesh;
Standard_Boolean myModified;
BRepMeshAdapt_DataMapOfShapeReal mymapedge;
TopoDS_Shape myshape;
TopTools_IndexedDataMapOfShapeListOfShape myancestors;
Standard_Real mydtotale;
Bnd_Box myBox;


};





// other Inline functions and methods (like "C++: function call" methods)
//


#endif
