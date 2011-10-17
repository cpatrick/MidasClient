/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __m3dsFolder_h_
#define __m3dsFolder_h_

#include "mdsObject.h"

namespace m3do{
  class Folder;
}

namespace m3ds{

class Folder : public mds::Object
{
public:

  Folder();
  ~Folder();

  bool Fetch();
  bool Commit();
  bool FetchTree();
  bool FetchSize();
  bool FetchParent();
  bool Delete(bool deleteOnDisk);
  bool Create();

  void ParentPathChanged(const std::string& parentPath);

  void SetObject(mdo::Object* object);
  void SetRecursive(bool recurse);

protected:

  bool          m_Recurse;
  m3do::Folder* m_Folder;
};

} //end namespace

#endif //__m3dsFolder_h_
