/******************************************************************************
 * Copyright 2011 Kitware Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

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
