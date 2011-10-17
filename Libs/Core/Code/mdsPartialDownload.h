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


#ifndef __MDSPARTIALDOWNLOAD_H
#define __MDSPARTIALDOWNLOAD_H

#include "midasStandardIncludes.h"

namespace mds
{

class PartialDownload
{
public:
  PartialDownload();
  ~PartialDownload();

  // Create a partial download record for a file
  bool Commit();

  // Remove a partial download record for a file
  bool Remove();

  // Return a list of all incomplete downloads
  static bool FetchAll(std::vector<mds::PartialDownload *>& list);

  // Clear all incomplete downloads
  static bool RemoveAll();

  void SetId(int id);

  int GetId();

  void SetOffset(int64 offset);

  int64 GetOffset();

  void SetPath(const std::string& path);

  std::string GetPath();

  void SetParentItem(int parentItem);

  int GetParentItem();

  void SetUuid(const std::string& uuid);

  std::string GetUuid();

protected:
  int         Id;
  int         ParentItem;
  std::string Path;
  std::string Uuid;
  int64       Offset;
};

} // end namespace mds
#endif
