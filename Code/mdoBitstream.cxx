/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mdoBitstream.h"
#include "mdoItem.h"
#include "mdoAssetstore.h"
#include <sstream>
#include <iostream>

namespace mdo{

/** Constructor */
Bitstream::Bitstream()
{
  m_Id = 0;
  m_ParentItem = NULL;
  m_Size = "0";
  m_Name = "";
}
  
/** Destructor */
Bitstream::~Bitstream()
{
  delete m_ParentItem;
  for(std::vector<Assetstore*>::iterator i = m_Locations.begin();
      i != m_Locations.end(); ++i)
    {
    delete *i;
    }
}

void Bitstream::Clear()
{
  this->m_Uuid = "";
  this->m_Size = "";
  this->m_Name = "";
  this->m_Parent = "";
  this->m_HasAgreement = "";
}

} // end namespace
