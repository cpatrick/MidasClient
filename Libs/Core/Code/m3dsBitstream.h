/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef _m3dsBitstream_h_
#define _m3dsBitstream_h_

#include "mdsObject.h"

namespace m3do{
  class Bitstream;
}

namespace m3ds{

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

} //end namespace

#endif //_mdsBitstream_h_
