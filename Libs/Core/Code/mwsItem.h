/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef _mwsItem_h_
#define _mwsItem_h_

#include <string>
#include <vector>

#include "mwsObject.h"
#include "mwsBitstream.h"

namespace mdo{
  class Item;
}

namespace mws{

/** This class represent an item on the MIDAS server. 
 *  An item has many bitstreams. */
class Item : public Object
{
public:

  Item();
  ~Item();

  bool Fetch();
  bool FetchParent();
  bool Commit();
  bool Delete();
  
  void SetObject(mdo::Object* object);
   
protected:

  friend class ItemXMLParser;
  mdo::Item* m_Item;

};

} //end namespace

#endif //_mwsItem_h_
