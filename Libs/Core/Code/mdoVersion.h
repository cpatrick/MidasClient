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
#ifndef __MDOVERSION_H
#define __MDOVERSION_H

#include "midasStandardIncludes.h"

namespace mdo
{

/**
 * Data structure for a version (major, minor, patch)
 */
class Version
{
public:
  Version();
  Version(int major, int minor, int patch, std::string name = "MIDASClient");
  Version(const std::string & versionString);
  ~Version();

  std::string VersionString();

  friend bool operator>(Version & v1, Version & v2);

  friend bool operator<(Version & v1, Version & v2);

  friend bool operator>=(Version & v1, Version & v2);

  friend bool operator<=(Version & v1, Version & v2);

  friend bool operator==(Version & v1, Version & v2);

  friend bool operator!=(Version& v1, Version& v2);

  int         Major;
  int         Minor;
  int         Patch;
  std::string Name;
};

} // end namespace

#endif
