/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

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
