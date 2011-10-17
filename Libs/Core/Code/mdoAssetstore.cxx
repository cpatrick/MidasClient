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
