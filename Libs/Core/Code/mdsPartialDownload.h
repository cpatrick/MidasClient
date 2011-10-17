/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

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
