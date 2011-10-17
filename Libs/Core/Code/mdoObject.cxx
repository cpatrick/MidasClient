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
