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


#ifndef __MDSUPGRADEHANDLER_H
#define __MDSUPGRADEHANDLER_H

#include "midasStandardIncludes.h"
#include "midasLogAware.h"
#include "mdoVersion.h"

/**
 * Abstract class for handling a database upgrade
 */
namespace mds
{

class UpgradeHandler : public midasLogAware
{
public:
  UpgradeHandler()
  {
  }
  virtual ~UpgradeHandler()
  {
  }

  /**
   * Returns true if the database at <path> was successfully upgraded
   * from <dbVersion> to <productVersion>
   */
  virtual bool Upgrade(const std::string& path, mdo::Version dbVersion, mdo::Version productVersion) = 0;

};

} // end namespace
#endif
