/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "mdoObject.h"

namespace mdo
{

Object::Object()
{
  m_Proxy = new Proxy();
  m_Proxy->SetObject(this);
  m_Dirty = false;
  m_Fetched = false;
  m_HasAgreement = "";
  m_Size = "";
}

Object::~Object()
{
  delete m_Proxy;
}

void Object::SetId(int id)
{
  m_Id = id;
}

int Object::GetId()
{
  return m_Id;
}

// Set/Get the ID of the parent
void Object::SetParentId(std::string id)
{
  m_Parent = id;
}

void Object::SetParentId(int id)
{
  std::stringstream str;
  str << id;
  m_Parent = str.str();
}

std::string & Object::GetParentStr()
{
  return m_Parent;
}

int Object::GetParentId()
{
  return atoi(m_Parent.c_str() );
}

// Set/Get the uuid
void Object::SetUuid(const char* uuid)
{
  m_Uuid = uuid;
}

std::string & Object::GetUuid()
{
  return m_Uuid;
}

/** Get the default proxy */
Proxy * Object::GetProxy()
{
  return m_Proxy;
}

bool Object::IsDirty()
{
  return m_Dirty;
}

void Object::SetDirty(bool dirty)
{
  m_Dirty = dirty;
}

std::string & Object::RefAgreement()
{
  return m_HasAgreement;
}

bool Object::HasAgreement()
{
  return m_HasAgreement == "1";
}

bool Object::SetValue(std::string key,
                      std::string value,
                      bool append)
{
  (void)key;
  (void)value;
  (void)append;
  return false;
}

void Object::SetSize(std::string size)
{
  m_Size = size;
}

std::string & Object::GetSize()
{
  return m_Size;
}

bool Object::IsFetched()
{
  return m_Fetched;
}

void Object::SetFetched(bool val)
{
  m_Fetched = val;
}

} // end namespace
