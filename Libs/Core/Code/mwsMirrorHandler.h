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


#ifndef _mwsMirrorHandler_h_
#define _mwsMirrorHandler_h_

#include "midasStandardIncludes.h"

namespace mdo
{
class Bitstream;
class Assetstore;
}

namespace mws
{
/**
 * Abstract class which will receive a callback anytime a
 * bitstream about to be pulled is mirrored in multiple locations
 */
class MirrorHandler
{
public:
  MirrorHandler()
  {
  }
  virtual ~MirrorHandler()
  {
  }

  /**
   * This function takes a bitstream with multiple locations as an argument.
   * It should do one of two things:
   * 1) Present the user with a list of the locations and let them choose one.
   * 2) Automatically choose one for them, either randomly or intelligently.
   * It should return a pointer to the chosen location object.
   */
  virtual mdo::Assetstore * HandleMirroredBitstream(mdo::Bitstream* resource) = 0;

};

} // end namespace

#endif
