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
#include "mdsDatabaseInfo.h"

namespace mds
{

/** Singleton */
DatabaseInfo * DatabaseInfo::m_Instance = NULL;

/** Return the instance as a singleton */
DatabaseInfo * DatabaseInfo::Instance()
{
  if( m_Instance != NULL )
    {
    return m_Instance;
    }
  else
    {
    m_Instance = new DatabaseInfo();
    return m_Instance;
    }
}

/** Constructor */
DatabaseInfo::DatabaseInfo()
{
  m_Path = "";
  m_ResourceUpdateHandler = NULL;
}

/** Destructor */
DatabaseInfo::~DatabaseInfo()
{
}

void DatabaseInfo::SetPath(const std::string& path)
{
  m_Path = path;
}

std::string DatabaseInfo::GetPath()
{
  return m_Path;
}

void DatabaseInfo::SetResourceUpdateHandler(ResourceUpdateHandler* handler)
{
  this->m_ResourceUpdateHandler = handler;
}

ResourceUpdateHandler * DatabaseInfo::GetResourceUpdateHandler()
{
  return this->m_ResourceUpdateHandler;
}

} // end namespace
