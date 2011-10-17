/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _m3wsItem_h_
#define _m3wsItem_h_

#include <string>
#include <vector>

#include "mwsObject.h"

namespace m3do{
  class Item;
}

namespace m3ws{

/** This class represent an Item on a MIDAS 3 server. */
class Item : public mws::Object
{
public:

  Item();
  ~Item();

  bool Fetch();
  bool FetchTree();
  bool FetchParent();
  bool Commit();
  bool Delete();

  void SetObject(mdo::Object* object);
  void ResolveParents();
     
protected:

  m3do::Item* m_Item;
};

} //end namespace

#endif //_m3wsFolder_h_
