/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __mdsCommunity_h_
#define __mdsCommunity_h_

#include "mdsObject.h"

namespace mdo{
  class Community;
}

namespace mds{

class Community : public Object
{
public:

  Community();
  ~Community();

  bool Fetch();
  bool Commit();
  bool FetchTree();
  bool Delete(bool deleteOnDisk);

  void ParentPathChanged(std::string parentPath);

  void SetObject(mdo::Object* object);
  void SetRecursive(bool recurse);
  void SetPath(std::string path);

protected:
  bool            m_Recurse;
  mdo::Community* m_Community;
  std::string     m_Path;

};

} //end namespace

#endif //__mdsCommunity_h_
