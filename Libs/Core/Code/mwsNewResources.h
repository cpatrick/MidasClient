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

#ifndef _mwsNewResources_h_
#define _mwsNewResources_h_

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

/** This class represent a list of modified resources on the server */
class NewResources : public Object
{
public:

  NewResources();
  ~NewResources();

  // Fill the object
  bool Fetch();

  bool FetchTree();

  void SetObject(mdo::Object* object);

  bool Commit();

  void ResolveParents();

  std::vector<std::string> GetUuids();
  void AddUuid(std::string uuid);

  std::string GetSince();
  void SetSince(std::string since);

  std::string & GetTimestamp();

protected:

  friend class NewResourcesXMLParser;

  std::string              m_Timestamp;
  std::string              m_Since;
  std::vector<std::string> m_Uuids;
};

} // end namespace

#endif // _mwsNewResources_h_
