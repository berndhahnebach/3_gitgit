// File:	BRepMeshAdapt_Triangle.cxx
// Created:	Thu Sep 23 13:10:58 1993
// Author:	Didier PIFFAULT
//		<dpf@zerox>

#include <BRepMeshAdapt_Triangle.ixx>
  
BRepMeshAdapt_Triangle::BRepMeshAdapt_Triangle()
     : Edge1(0), Edge2(0), Edge3(0), myDomain(0),myMovability(MeshDS_Free)
{}

BRepMeshAdapt_Triangle::BRepMeshAdapt_Triangle (const Standard_Integer e1, 
				      const Standard_Integer e2,
				      const Standard_Integer e3,
				      const Standard_Boolean o1, 
				      const Standard_Boolean o2,
				      const Standard_Boolean o3,
				      const MeshDS_DegreeOfFreedom canMove,
				      const Standard_Integer domain)
     : Edge1(e1),  Orientation1(o1),Edge2(e2), Orientation2(o2), 
       Edge3(e3), Orientation3(o3), 
       myDomain(domain), myMovability(canMove)
{}

void  BRepMeshAdapt_Triangle::Initialize(const Standard_Integer e1,
				    const Standard_Integer e2,
				    const Standard_Integer e3,
				    const Standard_Boolean o1, 
				    const Standard_Boolean o2,
				    const Standard_Boolean o3,
				    const MeshDS_DegreeOfFreedom canMove,
				    const Standard_Integer domain)
{
  Edge1        =e1;
  Edge2        =e2;
  Edge3        =e3;
  Orientation1 =o1;
  Orientation2 =o2;
  Orientation3 =o3;
  myDomain     =domain;
  myMovability =canMove;
}

void  BRepMeshAdapt_Triangle::Edges(Standard_Integer& e1,
			       Standard_Integer& e2,
			       Standard_Integer& e3,
			       Standard_Boolean& o1, 
			       Standard_Boolean& o2,
			       Standard_Boolean& o3)const 
{
  e1=Edge1;
  e2=Edge2;
  e3=Edge3;
  o1=Orientation1;
  o2=Orientation2;
  o3=Orientation3;
}

void  BRepMeshAdapt_Triangle::SetMovability(const MeshDS_DegreeOfFreedom Move)
{
  myMovability =Move;
}

Standard_Integer BRepMeshAdapt_Triangle::HashCode
  (const Standard_Integer Upper)const 
{
  return ::HashCode(Edge1+Edge2+Edge3, Upper);
}

Standard_Boolean BRepMeshAdapt_Triangle::IsEqual
  (const BRepMeshAdapt_Triangle& Other)const 
{
  if (myMovability==MeshDS_Deleted || Other.myMovability==MeshDS_Deleted)
    return Standard_False;
  if (Edge1==Other.Edge1 && Edge2==Other.Edge2 && Edge3==Other.Edge3)
    return Standard_True;
  if (Edge1==Other.Edge2 && Edge2==Other.Edge3 && Edge3==Other.Edge1)
    return Standard_True;
  if (Edge1==Other.Edge3 && Edge2==Other.Edge1 && Edge3==Other.Edge2)
    return Standard_True;
  return Standard_False;
}
