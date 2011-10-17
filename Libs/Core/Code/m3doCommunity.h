/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef _m3doCommunity_h_
#define _m3doCommunity_h_

#include <string>
#include <vector>

#include "mdoObject.h"
#include "m3doFolder.h"

namespace m3do{

/**
 * This class represent a MIDAS 3 community, which is just a top
 * level folder with some additional metadata
 */
class Community : public Folder
{
public:

  Community();
  Community(Community* other);
  ~Community();

  std::string GetTypeName();
  int GetResourceType();

  void SetParentFolder(Folder* folder);

  // The id is the community id, so we store the folder id separately
  void SetFolderId(int id);
  int& GetFolderId();
  
protected:
  int m_FolderId;
  
};

} //end namespace

#endif
