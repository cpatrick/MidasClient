/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
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

  void ResolveParents()
  {
  }                        // do nothing (no parents)

  std::vector<std::string> GetUuids()
  {
    return m_Uuids;
  }
  void AddUuid(std::string uuid)
  {
    m_Uuids.push_back(uuid);
  }

  std::string GetSince()
  {
    return m_Since;
  }
  void SetSince(std::string since)
  {
    m_Since = since;
  }

  std::string & GetTimestamp()
  {
    return m_Timestamp;
  }
protected:

  friend class NewResourcesXMLParser;

  std::string              m_Timestamp;
  std::string              m_Since;
  std::vector<std::string> m_Uuids;
};

} // end namespace

#endif // _mwsNewResources_h_
