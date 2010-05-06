/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mdoCollection.h"
#include <sstream>
#include <iostream>

namespace mdo{

/** Constructor */
Collection::Collection()
{
  m_Id = 0;
}
  
/** Destructor */
Collection::~Collection()
{
  // Clean the items
  std::vector<Item*>::iterator itItem = m_Items.begin();
  while(itItem != m_Items.end())
    {
    Item* item = *itItem;
    itItem++;
    delete item;
    }
  m_Items.clear();  
}
  
/** Load */
bool Collection::Load()
{
  return m_Proxy->Load();
}

void Collection::Clear()
{
  this->m_Name = "";
  this->m_Description = "";
  this->m_IntroductoryText = "";
  this->m_Copyright = "";
}

//--------------------------------------------------------------------------------------------------
void Collection::Print(std::ostream &os, int indent)
{
  os << "Id: " << m_Id << "\n";
  os << "Name: " << m_Name.c_str() << "\n";
  os << "Description: " << m_Description.c_str() << "\n";
  // Display all the bitstream in the list
  os << "# of items: " << m_Items.size() << "\n";
  for(unsigned int i=0; i<m_Items.size(); i++)
    {
    m_Items[i]->Print(os, indent+1);
    }
}

} // end namespace