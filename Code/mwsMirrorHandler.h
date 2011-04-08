/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef _mwsMirrorHandler_h_
#define _mwsMirrorHandler_h_

#include "midasStandardIncludes.h"

namespace mdo {
  class Bitstream;
  class Assetstore;
}

namespace mws {
/**
 * Abstract class which will receive a callback anytime a
 * bitstream about to be pulled is mirrored in multiple locations
 */
class MirrorHandler
{
public:
  MirrorHandler() {}
  virtual ~MirrorHandler() {}

  /**
   * This function takes a bitstream with multiple locations as an argument.
   * It should do one of two things:
   * 1) Present the user with a list of the locations and let them choose one.
   * 2) Automatically choose one for them, either randomly or intelligently.
   * It should return a pointer to the chosen location object.
   */
  virtual mdo::Assetstore* HandleMirroredBitstream(
    mdo::Bitstream* resource) = 0;
};

} //end namespace

#endif
