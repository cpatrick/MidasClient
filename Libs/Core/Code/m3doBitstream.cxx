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

#include "m3doBitstream.h"
#include "m3doItem.h"

#include <QString>

namespace m3do
{

Bitstream::Bitstream()
{
  m_Id = 0;
  m_BitstreamCount = 0;
  m_ParentItem = NULL;
}

// copy constructor
Bitstream::Bitstream(Bitstream* other)
{
  m_Id = other->GetId();
  m_Uuid = other->GetUuid();
  m_ParentItem = other->GetParentItem();
  m_Path = other->GetPath();
  m_Name = other->GetName();
  m_Checksum = other->GetChecksum();
  m_LastModified = other->GetLastModified();
}

Bitstream::~Bitstream()
{
}

void Bitstream::SetName(const char* name)
{
  m_Name = name;
}

std::string & Bitstream::GetName()
{
  return m_Name;
}

std::string Bitstream::GetTypeName()
{
  return "Bitstream";
}

int Bitstream::GetResourceType()
{
  return midas3ResourceType::BITSTREAM;
}

void Bitstream::SetParentItem(Item* item)
{
  m_ParentItem = item;
}

Item * Bitstream::GetParentItem()
{
  return m_ParentItem;
}

void Bitstream::SetPath(const std::string& path)
{
  m_Path = path;
}

std::string & Bitstream::GetPath()
{
  return m_Path;
}

void Bitstream::SetChecksum(const std::string& checksum)
{
  m_Checksum = checksum;
}

std::string & Bitstream::GetChecksum()
{
  return m_Checksum;
}

void Bitstream::SetLastModified(unsigned int stamp)
{
  m_LastModified = stamp;
}

unsigned int & Bitstream::GetLastModified()
{
  return m_LastModified;
}

void Bitstream::SetCreationDate(const std::string& date)
{
  m_CreationDate = date;
}

std::string & Bitstream::GetCreationDate()
{
  return m_CreationDate;
}

void Bitstream::Clear()
{
  this->m_Name = "";
  this->m_Checksum = "";
  this->m_Uuid = "";
  this->m_HasAgreement = "";
  this->m_Size = "";
}

bool Bitstream::Load()
{
  return m_Proxy->Load();
}

bool Bitstream::LoadTree()
{
  return m_Proxy->LoadTree();
}

} // end namespace
