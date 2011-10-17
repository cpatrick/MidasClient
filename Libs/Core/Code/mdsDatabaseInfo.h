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


#ifndef _MDSDATABASEINFO_H
#define _MDSDATABASEINFO_H

#include "midasStandardIncludes.h"
#include "midasLogAware.h"

namespace mds
{

class ResourceUpdateHandler;

/**
 * This singleton class stores persistent info about the database
 * Typically these are used as defaults -- they can be overriden by a
 * specific database connection if needed
 */
class DatabaseInfo : public midasLogAware
{
public:
  static DatabaseInfo * Instance(); // singleton

  ~DatabaseInfo();

  void SetPath(const std::string& path);

  std::string GetPath();

  void SetResourceUpdateHandler(mds::ResourceUpdateHandler* handler);

  ResourceUpdateHandler * GetResourceUpdateHandler();

protected:
  static DatabaseInfo* m_Instance;
  DatabaseInfo(); // singleton: use Instance() to construct

  std::string            m_Path;
  ResourceUpdateHandler* m_ResourceUpdateHandler;
};

} // end namespace

#endif
