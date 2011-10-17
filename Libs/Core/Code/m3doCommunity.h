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


#ifndef _m3doCommunity_h_
#define _m3doCommunity_h_

#include <string>
#include <vector>

#include "mdoObject.h"
#include "m3doFolder.h"

namespace m3do
{

/**
 * This class represent a MIDAS 3 community, which is just a top
 * level folder with some additional metadata
 */
class Community : public Folder
{
public:

  Community();
  Community(Community* other);
  ~Community();

  std::string GetTypeName();

  int GetResourceType();

  void SetParentFolder(Folder* folder);

  // The id is the community id, so we store the folder id separately
  void SetFolderId(int id);

  int & GetFolderId();

protected:
  int m_FolderId;

};

} // end namespace

#endif
