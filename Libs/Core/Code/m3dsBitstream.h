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


#ifndef _m3dsBitstream_h_
#define _m3dsBitstream_h_

#include "mdsObject.h"

namespace m3do
{
class Bitstream;
}

namespace m3ds
{

class Bitstream : public mds::Object
{
public:

  Bitstream();
  ~Bitstream();

  bool Fetch();

  bool FetchTree();

  bool FetchParent();

  bool Commit();

  /**
   * If the bitstream exists locally already (by checksum), we copy
   * the existing file into the desired path, obviating the subsequent
   * call to Download().
   */
  bool CopyContentIfExists();

  /**
   * If any bitstream under the same parent item has the same name and
   * checksum as this bitstream, this will return true.
   */
  bool AlreadyExistsInItem();

  bool Delete(bool deleteOnDisk);

  void ParentPathChanged(std::string parentPath);

  void SetObject(mdo::Object* object);

protected:
  m3do::Bitstream* m_Bitstream;

};

} // end namespace

#endif // _mdsBitstream_h_
