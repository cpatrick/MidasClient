/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _mwsCommunity_h_
#define _mwsCommunity_h_

#include <string>
#include <vector>

#include "mwsObject.h"
#include "mwsCollection.h"

namespace mdo{
  class Community;
}

namespace mws{

/** This class represent an community on the MIDAS server. 
 *  An community has many bitstreams. */
class Community : public Object
{
public:

  Community();
  ~Community();

  // Fill the community
  bool Fetch();
  bool FetchTree();
  bool FetchParent();
  void SetObject(mdo::Object* object);
  bool Commit();
  void ResolveParents();
     
protected:

  friend class CommunityXMLParser;

  mdo::Community*  m_Community;
};

} //end namespace

#endif //_mwsCommunity_h_
