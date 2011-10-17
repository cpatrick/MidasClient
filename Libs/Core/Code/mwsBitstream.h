/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

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
