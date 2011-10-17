/******************************************************************************
 * Copyright 2011 Kitware Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/


#ifndef _mwsCollection_h_
#define _mwsCollection_h_

#include <string>
#include <vector>

#include "mwsObject.h"

namespace mdo
{
class Collection;
}

namespace mws
{

/** This class represent an collection on the MIDAS server.
 *  An collection has many bitstreams. */
class Collection : public Object
{
public:

  Collection();
  ~Collection();

  // Fill the collection
  bool Fetch();

  bool FetchParent();

  bool Commit();

  bool Delete();

  void SetObject(mdo::Object* object);

protected:

  friend class CollectionXMLParser;
  mdo::Collection* m_Collection;
};

} // end namespace

#endif // _mwsCollection_h_
