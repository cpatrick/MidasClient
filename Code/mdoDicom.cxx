/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mdoDicom.h"
#include <sstream>
#include <iostream>

namespace mdo{

/** Constructor */
Dicom::Dicom()
{
  m_ItemId = 0;
  m_AETitle = "";
}
  
/** Destructor */
Dicom::~Dicom()
{
}
  
/** Load */
bool Dicom::Load()
{
  return m_Proxy->Load();
}

/** Upload a bitstream to MIDAS */
unsigned int Dicom::UploadDicom(const char* filename)
{
  return 1;
}
  
/** Print */
void Dicom::Print(std::ostream &os, int indent)
{
  os << "ItemId: " << m_ItemId << "\n";
  os << "AETitle: " << m_AETitle << "\n";
}

} // end namespace
