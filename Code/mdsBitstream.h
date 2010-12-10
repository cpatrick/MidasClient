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

namespace mdo{
  class Bitstream;
}

namespace mds{

class Bitstream : public Object
{
public:

  Bitstream();
  ~Bitstream();

  bool Fetch();
  bool Commit();
  bool FetchTree();
  bool Delete(bool deleteOnDisk);

  void ParentPathChanged(std::string parentPath);

  void SetObject(mdo::Object* object);
  void SetPath(std::string path);

protected:

  mdo::Bitstream* m_Bitstream;
  std::string     m_Path;
};

} //end namespace

#endif //_mdsBitstream_h_
