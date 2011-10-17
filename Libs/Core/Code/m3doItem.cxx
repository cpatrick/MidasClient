/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
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
