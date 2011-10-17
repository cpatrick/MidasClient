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


#ifndef _mwsBitstream_h_
#define _mwsBitstream_h_

#include "mwsObject.h"
#include <vector>

namespace mdo
{
class Bitstream;
}

namespace mws
{

/** This class represent a bitstream on the MIDAS server. */
class Bitstream : public Object
{
public:

  Bitstream();
  ~Bitstream();

  // Fill the item given the id
  bool Fetch();

  bool FetchParent();

  // Get all of the assetstore locations of the bitstream
  bool FetchLocations();

  bool Commit();

  bool Delete();

  // Download this bitstream
  bool Download();

  // Upload this bitstream to the server
  bool Upload();

  // Resume an upload (set offset prior to calling this)
  bool ResumeUpload(const std::string& token, int userId);

  void SetObject(mdo::Object* object);

  // Set the offset for resuming a download
  void SetOffset(int64 offset);

protected:

  mdo::Bitstream* m_Bitstream;
  int64           m_Offset;
};

} // end namespace

#endif // _mwsBitstream_h_
