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

#include "m3doItem.h"
#include "m3doFolder.h"
#include "m3doBitstream.h"
#include "midasStandardIncludes.h"

#include <QString>

namespace m3do
{

Item::Item()
{
  m_Id = 0;
  m_BitstreamCount = 0;
  m_ParentFolder = NULL;
}

// copy constructor
Item::Item(Item* other)
{
  m_Id = other->GetId();
  m_Uuid = other->GetUuid();
  m_ParentFolder = other->GetParentFolder();
  m_Path = other->GetPath();
  m_Name = other->GetName();
  m_Description = other->GetDescription();
}

Item::~Item()
{
  std::vector<Bitstream *>::iterator i = m_Bitstreams.begin();
  while( i != m_Bitstreams.end() )
    {
    Bitstream* b = *i;
    ++i;
    delete b;
    }
}

void Item::Clear()
{
  this->m_Name = "";
  this->m_Description = "";
  this->m_Uuid = "";
  this->m_HasAgreement = "";
  this->m_Size = "";
}

/** Load */
bool Item::Load()
{
  return m_Proxy->Load();
}

/** Fill the full tree with Folder and collection */
bool Item::LoadTree()
{
  return m_Proxy->LoadTree();
}

bool Item::SetValue(std::string key, std::string value, bool append)
{
  QString keyStr = key.c_str();

  keyStr = keyStr.toUpper();
  key = keyStr.toStdString();

  if( key == "NAME" )
    {
    if( append )
      {
      m_Name += value;
      }
    else
      {
      m_Name = value;
      }
    return true;
    }
  if( key == "DESCRIPTION" )
    {
    if( append )
      {
      m_Description += value;
      }
    else
      {
      m_Description = value;
      }
    return true;
    }

  return false;
}

void Item::AddBitstream(Bitstream* bitstream)
{
  m_Bitstreams.push_back(bitstream);
}

} // end namespace
