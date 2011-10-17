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


#ifndef __MDSPARTIALUPLOAD_H
#define __MDSPARTIALUPLOAD_H

#include "midasStandardIncludes.h"

namespace mds
{

class PartialUpload
{
public:
  PartialUpload();
  ~PartialUpload();

  // Create a partial Upload record for a file
  bool Commit();

  // Remove a partial Upload record for a file
  bool Remove();

  // Return a list of all incomplete Uploads
  static bool FetchAll(std::vector<mds::PartialUpload *>& list);

  // Clear all incomplete Uploads
  static bool RemoveAll();

  void SetId(int id);

  int GetId();

  void SetBitstreamId(int id);

  int GetBitstreamId();

  void SetToken(const std::string& token);

  std::string GetToken();

  void SetUserId(int id);

  int GetUserId();

  void SetParentItem(int id);

  int GetParentItem();

protected:
  int         Id;
  int         BitstreamId;
  int         ParentItem;
  int         UserId;
  std::string Token;
};

} // end namespace mds
#endif
