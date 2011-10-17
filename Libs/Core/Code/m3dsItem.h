/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __m3dsItem_h_
#define __m3dsItem_h_

#include "mdsObject.h"

namespace m3do
{
class Item;
}

namespace m3ds
{

class Item : public mds::Object
{
public:

  Item();
  ~Item();

  bool Fetch();

  bool Commit();

  bool FetchTree();

  bool FetchSize();

  bool FetchParent();

  bool Delete(bool deleteOnDisk);

  bool Create();

  void ParentPathChanged(const std::string& parentPath);

  void SetObject(mdo::Object* object);

protected:
  m3do::Item* m_Item;
};

} // end namespace

#endif // __mdsItem_h_
