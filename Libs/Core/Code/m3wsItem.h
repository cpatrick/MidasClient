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

#ifndef _m3wsItem_h_
#define _m3wsItem_h_

#include <string>
#include <vector>

#include "mwsObject.h"

namespace m3do
{
class Item;
}

namespace m3ws
{

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

} // end namespace

#endif // _m3wsFolder_h_
