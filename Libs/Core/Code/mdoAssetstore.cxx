/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "mdoAssetstore.h"

namespace mdo
{

Assetstore::Assetstore() 
: m_Type(ASSETSTORE_LOCAL)
{
}

Assetstore::~Assetstore()
{
}

void Assetstore::SetId(int id)
{
  m_Id = id;
}

int Assetstore::GetId()
{
  return m_Id;
}

void Assetstore::SetEnabled(bool val)
{
  m_Enabled = val;
}

bool Assetstore::IsEnabled()
{
  return m_Enabled;
}

void Assetstore::SetName(std::string name)
{
  m_Name = name;
}
std::string & Assetstore::GetName()
{
  return m_Name;
}
// Set/Get the path or URL of the assetstore
void Assetstore::SetPath(std::string path)
{
  m_Path = path;
}
std::string Assetstore::GetPath()
{
  return m_Path;
}
// Set/Get the type of the assetstore
void Assetstore::SetType(AssetstoreType type)
{
  m_Type = type;
}
  Assetstore::AssetstoreType Assetstore::GetType()
{
  return m_Type;
}

// Set/Get the internal id of a bitstream in the assetstore
// (used for bitstream location)
void Assetstore::SetInternalId(std::string str)
{
  m_InternalId = str;
}

std::string Assetstore::GetInternalId()
{
  return m_InternalId;
}

}; // end namespace mdo
