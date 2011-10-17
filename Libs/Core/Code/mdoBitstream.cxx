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

namespace mdo
{

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
  for( std::vector<Assetstore *>::iterator i = m_Locations.begin();
       i != m_Locations.end(); ++i )
    {
    delete *i;
    }
}

//-----------------------------------------------------------------------------
void Bitstream::Clear()
{
  this->m_Uuid = "";
  this->m_Size = "";
  this->m_Name = "";
  this->m_Parent = "";
  this->m_HasAgreement = "";
}

//-----------------------------------------------------------------------------
void Bitstream::SetName(const char* name)
{
  m_Name = name;
}

//-----------------------------------------------------------------------------
std::string & Bitstream::GetName()
{
  return m_Name;
}

//-----------------------------------------------------------------------------
void Bitstream::SetLastModified(unsigned int time)
{
  m_LastModified = time;
}

//-----------------------------------------------------------------------------
long int Bitstream::GetLastModified()
{
  return m_LastModified;
}

//-----------------------------------------------------------------------------
void Bitstream::SetPath(std::string path)
{
  m_Path = path;
}

//-----------------------------------------------------------------------------
std::string Bitstream::GetPath()
{
  return m_Path;
}

//-----------------------------------------------------------------------------
std::string Bitstream::GetTypeName()
{
  return "Bitstream";
}

//-----------------------------------------------------------------------------
int Bitstream::GetResourceType()
{
  return midasResourceType::BITSTREAM;
}

//-----------------------------------------------------------------------------
void Bitstream::AddLocation(Assetstore* location)
{
  m_Locations.push_back(location);
}

//-----------------------------------------------------------------------------
void Bitstream::SetLocations(std::vector<Assetstore *> locations)
{
  m_Locations = locations;
}

//-----------------------------------------------------------------------------
std::vector<Assetstore *> & Bitstream::GetLocations()
{
  return m_Locations;
}

//-----------------------------------------------------------------------------
Item * Bitstream::GetParentItem()
{
  return m_ParentItem;
}

//-----------------------------------------------------------------------------
void Bitstream::SetParentItem(Item* item)
{
  m_ParentItem = item;
}

} // end namespace
