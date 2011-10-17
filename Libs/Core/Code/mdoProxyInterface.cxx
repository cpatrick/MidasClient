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

#include "mdoProxyInterface.h"
#include <sstream>
#include <iostream>

namespace mdo
{

/** Constructor */
ProxyInterface::ProxyInterface()
{
  m_IsCache = false;
  m_Name = "No Name";
}

/** Destructor */
ProxyInterface::~ProxyInterface()
{
}

/** Set the name of the interface */
void ProxyInterface::SetName(const char* name)
{
  m_Name = name;
}

/** Fetch a tree */
bool ProxyInterface::FetchTree()
{
  std::cerr << m_Name.c_str()
            << " ProxyInterface::FetchTree(): Not implemented" << std::endl;
  return false;
}

/** Set if this interface is acting as a cache */
void ProxyInterface::SetIsCache(bool iscache)
{
  m_IsCache = iscache;
}

/** Get if this interface is acting as a cache */
bool ProxyInterface::GetIsCache()
{
  return m_IsCache;
}

const char * ProxyInterface::GetName()
{
  return m_Name.c_str();
}

} // end namespace
