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

#ifndef _mwsCommunity_h_
#define _mwsCommunity_h_

#include <string>
#include <vector>

#include "mwsObject.h"
#include "mwsCollection.h"

namespace mdo
{
class Community;
}

namespace mws
{

/** This class represent an community on the MIDAS server.
 *  An community has many bitstreams. */
class Community : public Object
{
public:

  Community();
  ~Community();

  bool Fetch();

  bool FetchTree();

  bool FetchParent();

  bool Commit();

  bool Delete();

  void SetObject(mdo::Object* object);

  void ResolveParents();

protected:

  friend class CommunityXMLParser;

  mdo::Community* m_Community;
};

} // end namespace

#endif // _mwsCommunity_h_
