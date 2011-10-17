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


#ifndef __m3dsFolder_h_
#define __m3dsFolder_h_

#include "mdsObject.h"

namespace m3do
{
class Folder;
}

namespace m3ds
{

class Folder : public mds::Object
{
public:

  Folder();
  ~Folder();

  bool Fetch();

  bool Commit();

  bool FetchTree();

  bool FetchSize();

  bool FetchParent();

  bool Delete(bool deleteOnDisk);

  bool Create();

  void ParentPathChanged(const std::string& parentPath);

  void SetObject(mdo::Object* object);

  void SetRecursive(bool recurse);

protected:

  bool          m_Recurse;
  m3do::Folder* m_Folder;
};

} // end namespace

#endif // __m3dsFolder_h_
