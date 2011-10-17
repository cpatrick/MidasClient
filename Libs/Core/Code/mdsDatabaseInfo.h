/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

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
