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


#ifndef _mdsBitstream_h_
#define _mdsBitstream_h_

#include "mdsObject.h"

namespace mdo
{
class Bitstream;
}

namespace mds
{

class Bitstream : public Object
{
public:

  Bitstream();
  ~Bitstream();

  bool Fetch();

  /**
   * Commits this bitstream object.
   * If the bitstream id is set to 0, it will commit a new record
   */
  bool Commit();

  bool FetchTree();

  bool Delete(bool deleteOnDisk);

  void ParentPathChanged(std::string parentPath);

  void SetObject(mdo::Object* object);

protected:
  mdo::Bitstream* m_Bitstream;

};

} // end namespace

#endif // _mdsBitstream_h_
