/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef _mdoDicom_h_
#define _mdoDicom_h_

#include <string>

#include "mdoObject.h"

namespace mdo{

/** This class represent a Dicom to send to the DICOM plugin */
class Dicom : public Object
{
public:

  Dicom();
  ~Dicom();

  // Load
  bool Load();

  // Set/Get the AETitle
  void SetAETitle(const std::string aetitle) { this->m_AETitle = aetitle; }
  const std::string GetAETitle() {return this->m_AETitle; }
  
  // Set/Get the ID of the item
  void SetItemId(const unsigned int itemid) { this->m_ItemId = itemid; }
  const unsigned int GetItemId() {return this->m_ItemId; }
  
  // Upload a DICOM to MIDAS
  unsigned int UploadDicom(const char* filename);
    
  // Display all the tags and their respective names and values.
  void Print(std::ostream &os, int indent=-1);
 
protected:

  friend class DicomXMLParser;
  
  unsigned int m_ItemId;
  std::string  m_AETitle;

};

} //end namespace

#endif //_mdoItem_h_
