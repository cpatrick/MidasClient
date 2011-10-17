/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
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
