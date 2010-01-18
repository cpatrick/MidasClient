/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef _mwsDicom_h_
#define _mwsDicom_h_

#include "mwsObject.h"

namespace mdo{
  class Dicom;
}

namespace mws{

/** This class represent an Dicom on the MIDAS server. 
 *   */
class Dicom : public Object
{
public:

  Dicom();
  ~Dicom();

  // Fetch data from the MIDAS server
  bool Fetch();
  
  // Commit the data on the server 
  bool Commit();
  
  // Add the object
  void SetObject(mdo::Object* object);

  // Upload a bitstream to MIDAS
  const unsigned int GetItemId();
   
protected:

  friend class DicomXMLParser;
  mdo::Dicom* m_Dicom;

};

} //end namespace

#endif //_mwsDicom_h_
