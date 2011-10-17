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

#ifndef _m3wsBitstream_h_
#define _m3wsBitstream_h_

#include <string>
#include <vector>

#include "mwsObject.h"

namespace m3do
{
class Bitstream;
}

namespace m3ws
{

/** This class represent an Bitstream on a MIDAS 3 server. */
class Bitstream : public mws::Object
{
public:

  Bitstream();
  ~Bitstream();

  bool Fetch();

  bool FetchTree();

  bool FetchParent();

  bool Delete();

  bool Download();

  bool Upload();

  bool Commit();

  void SetObject(mdo::Object* object);

  void ResolveParents();

protected:

  m3do::Bitstream* m_Bitstream;
};

} // end namespace

#endif // _m3wsFolder_h_
