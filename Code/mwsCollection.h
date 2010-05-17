/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef _mwsCollection_h_
#define _mwsCollection_h_

#include <string>
#include <vector>

#include "mwsObject.h"

namespace mdo{
  class Collection;
}

namespace mws{

/** This class represent an collection on the MIDAS server. 
 *  An collection has many bitstreams. */
class Collection : public Object
{
public:

  Collection();
  ~Collection();
  
  // Fill the collection
  bool Fetch();
  void SetObject(mdo::Object* object);
  bool Commit();
  void ResolveParents();
  
protected:

  friend class CollectionXMLParser;
  mdo::Collection* m_Collection;
};

} //end namespace

#endif //_mwsCollection_h_
