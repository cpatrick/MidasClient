/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mdoItem.h"
#include <sstream>
#include <iostream>

namespace mdo{

/** Constructor */
Item::Item()
{
  m_Id = 0;
}
  
/** Destructor */
Item::~Item()
{
}
  
/** Load */
bool Item::Load()
{
  return m_Proxy->Load();
}

/** Upload a bitstream to MIDAS */
unsigned int Item::UploadBitstream(unsigned int itemid,const char* filename)
{
  return 1;
}
  
/** Download a bitstream from MIDAS.
 *  For now we cache to any of the interfaces that are acting as a cache */
std::string Item::DownloadBitstream(const char* uuid,const char* filename)
{
  return m_Proxy->DownloadBitstream(uuid,filename);
}

/** Print */
void Item::Print(std::ostream &os, int indent)
{
  os << "Id: " << m_Id << "\n";
  os << "Title: " << m_Title.c_str() << "\n";
  os << "Abstract: " << m_Abstract.c_str() << "\n";
  // Display all the bitstream in the list
  os << "# of bistreams: " << m_Bitstreams.size() << "\n";
  for(unsigned int i=0; i<m_Bitstreams.size(); i++)
    {
    m_Bitstreams[i]->Print(os, indent+1);
    }
}

} // end namespace