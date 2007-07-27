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




#ifndef _Feature_h_
#define _Feature_h_

#include <string>

#include <Base/Factory.h>
#include <Base/PyExport.h>
#include <Base/TimeInfo.h>

#include <App/DocumentObject.h>
#include <App/Material.h>
#include <App/PropertyStandard.h>


namespace App
{

//class FeaturePy;
class Property;
class Document;
class FeatureDataProvider;
class FeaturePy;

/** Base class of all Feature classes in FreeCAD
 */
class AppExport AbstractFeature: public App::DocumentObject
{
    PROPERTY_HEADER(App::AbstractFeature);

public:

  PropertyEnumeration status;


	/// Constructor
	AbstractFeature(void);
  virtual ~AbstractFeature();

  virtual void onChanged(const Property* prop);
  //void setModified(bool);
  //bool isModified() const;


	/** @name methods to overide with a new feature type */
	//@{

	/** Validate
	 *  We compute the object and topologically name it.
	 *  If during the execution we found something wrong,
	 *  we return the number of the failure. For example:
	 *  1 - an attribute hasn't been found,
	 *  2 - algorithm failed
	 *  0 - no mistakes were found.
	 */
	virtual int execute(void)=0;


  //@}

	/** @name status methods of the feature */
	//@{
  /// status types
  enum Status 
  {
    Valid =0   ,/**< enum The Feature is Valid */
    New=1      ,/**< enum The Feature is new */
    Inactive=2 ,/**< enum Will not recalculated */
    Recompute=3,/**< enum Fetature is in recomputation*/
    Error=4     /**< enum Feture is in error state */
  };

  /// gets the status
  int getStatus(void) const { return /*_eStatus;*/status.getValue();}
  /// set the status, e.g. after recoputation in Execute()
 // void setStatus(const Status &s){_eStatus = s;}
  /// get the status Message
  const char *getStatusString(void) const;//{return _cStatusMessage.c_str();}
  /// get the error Message (if any)
  const char *getErrorString(void) const{return _cErrorMessage.c_str();}
  /// set an error on recoputation
  void setError(const char* pMsg,...);
  /// checks if valid
  bool isValid(void){return /*_eStatus == Valid;*/ status.getValue() == Valid; }
  /// Recompute only this feature and makes it valid again
  void recompute(void);
	/** MustExecute
	 *  We call this method to check if the object was modified to
	 *  be invoked. If the object label or an argument is modified.
	 *  If we must recompute the object - to call the method Execute().
	 */
	virtual bool mustExecute(void);
  //@}




	virtual PyObject *getPyObject(void);

  friend class FeaturePy;
  friend class Document;


protected:

  std::string _cErrorMessage;
  bool _execute;

};




} //namespace App



#endif
