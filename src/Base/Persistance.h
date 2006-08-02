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


#ifndef __Persistance_H__
#define __Persistance_H__

// Std. configurations


#include <assert.h>

#include "Base.h"
#include "zipios/zipinputstream.h"

namespace Base
{
  typedef std::istream Reader;
  class Writer;
  class XMLReader;

/// Persistance class and root of the type system
class BaseExport Persistance : public BaseClass
{

  TYPESYSTEM_HEADER();

public: 
  /** This method is used to get the size of objects
    * Its not ment to have the exact size, is more or less an estimation which run fast!
    * Is it two byte or an GB?
    */
  virtual unsigned int getMemSize (void) const{
    // you have to implement this methode in all descenting classes!
    assert(0);
    return 0;
  } 
  /** This method is used to save properties to an XML document.
    * An good example you'll find in PropertyStandard.cpp, e.g. the vector:
    * \code
    *  void PropertyVector::Save (Writer &writer) const
    *  {
    *     writer << writer.ind() << "<PropertyVector valueX=\"" <<  _cVec.x << "\" valueY=\"" <<  _cVec.y << "\" valueZ=\"" <<  _cVec.z <<"\"/>" << endl;
    *  }
    * \endcode
    * The writer.ind() expresion writes the indention, just for preaty printing of the XML.
    * @see Base::Writer
    */
  virtual void Save (Writer &writer) const {
    // you have to implement this methode in all descenting classes!
    assert(0);
  } 
  /** This method is used to restore properties from an XML document.
    * It uses the XMLReader class, which bases on SAX, to read the in Save() 
    * written information. Again the Vector as an example:
    * \code
    * void PropertyVector::Restore(Base::XMLReader &reader)
    * {
    *   // read my Element
    *   reader.readElement("PropertyVector");
    *   // get the value of my Attribute
    *   _cVec.x = (float)reader.getAttributeAsFloat("valueX");
    *   _cVec.y = (float)reader.getAttributeAsFloat("valueY");
    *   _cVec.z = (float)reader.getAttributeAsFloat("valueZ");
    * }
    * \endcode
    */
  virtual void Restore(XMLReader &reader){
    // you have to implement this methode in all descenting classes!
    assert(0);
  } 
  /** This method is used to save large amounts of data to a binary file.
    * Somtimes it makes no sense to write property data not as XML. In Case the 
    * amount of data is to big, or the data type has a more effective way to 
    * save itself. In this cases its possible to write the data in a seperate file
    * inside the Document archive. In case you whant do so you have to reimplement 
    * SaveDocFile(). First you have to inform the Framwork in Save() that you whant do so.
    * Here an example from the Mesh module which can save a (pontetionaly big) triangle mesh:
    * \code 
    * void PropertyMeshKernel::Save (Base::Writer &writer) const
    * {
    *   if( writer.isForceXML() )
    *   {
    *     writer << writer.ind() << "<Mesh>" << std::endl;
    *     MeshCore::MeshDocXML saver(*_pcMesh);
    *     saver.Save(writer);
    *   }else{
    *    writer << writer.ind() << "<Mesh file=\"" << writer.addFile("MeshKernel.bms", this) << "\"/>" << std::endl;
    * }
    * \endcode
    * The writer.isForceXML() is an indication to force you to write XML. Regardles to size and effectivness.
    * The second part inform the Base::writer through writer.addFile("MeshKernel.bms", this) that this object whants to 
    * write a file with the given name. The methode addFile() returns a unique name that then is written in the XML stream.
    * This allows your RestoreDocFile() methode to identify and read the file again.
    * Later your SaveDocFile() methode is called as many times you issued the addFile() call:
    * \code
    * void PropertyMeshKernel::SaveDocFile (Base::Writer &writer) const
    * {
    *     _pcMesh->Write( writer );
    * }
    * \endcode
    * In this methode you can simply stream your content to the file (Base::Writer inheriting from ostream).
    */
  virtual void SaveDocFile (Writer &writer) const{
    // you have to implement this methode in all descenting classes!
    assert(0);
  } 
  /** This method is used to restore large amounts of data from a file
    * In this method you simply stream in your with SaveDocFile() saved data.
    * Again you have to apply for the call of this method in the Restore() call:
    * \code
    * void PropertyMeshKernel::Restore(Base::XMLReader &reader)
    * {
    *   reader.readElement("Mesh");
    *   std::string file (reader.getAttribute("file") );
    * 
    *   if(file == "")
    *   {
    *     // read XML
    *     MeshCore::MeshDocXML restorer(*_pcMesh);
    *     restorer.Restore(reader);
    *   }else{
    *     // initate a file read
    *     reader.addFile(file.c_str(),this);
    *  }
    * }
    * \endcode
    * After you issued the reader.addFile() your RestoreDocFile() is called:
    * \code
    * void PropertyMeshKernel::RestoreDocFile(Base::Reader &reader)
    * {
    *     _pcMesh->Read( reader );
    * }
    * \endcode
    * @see Base::Reader,Base::XMLReader
    */
  virtual void RestoreDocFile(Reader &reader){
    // you have to implement this methode in all descenting classes!
    assert(0);
  } 

};





} //namespace Base

#endif // __Persistance_H__

