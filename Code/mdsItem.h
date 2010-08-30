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

namespace mdo{
  class Item;
}

namespace mds{

class Item : public Object
{
public:

  Item();
  ~Item();

  bool Fetch();
  bool Commit();
  bool FetchTree();
  bool Delete();
  
  // Add the object
  void SetObject(mdo::Object* object);
  void SetRecursive(bool recurse);

protected:
  bool       m_Recurse;
  mdo::Item* m_Item;

};

} //end namespace

#endif //__mdsItem_h_
