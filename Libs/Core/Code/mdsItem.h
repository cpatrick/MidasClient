/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __mdsItem_h_
#define __mdsItem_h_

#include "mdsObject.h"

namespace mdo
{
class Item;
}

namespace mds
{

class Item : public Object
{
public:

  Item();
  ~Item();

  bool Fetch();

  bool Commit();

  bool FetchTree();

  bool FetchSize();

  bool Delete(bool deleteOnDisk);

  void ParentPathChanged(std::string parentPath);

  // Add the object
  void SetObject(mdo::Object* object);

  void SetPath(std::string path);

protected:
  mdo::Item*  m_Item;
  std::string m_Path;
};

} // end namespace

#endif // __mdsItem_h_
