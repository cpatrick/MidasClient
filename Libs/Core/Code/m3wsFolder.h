/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _m3wsFolder_h_
#define _m3wsFolder_h_

#include <string>
#include <vector>

#include "mwsObject.h"

namespace m3do{
  class Folder;
}

namespace m3ws{

/** This class represent a Folder on a MIDAS 3 server. */
class Folder : public mws::Object
{
public:

  Folder();
  ~Folder();

  bool Fetch();
  bool FetchTree();
  bool FetchParent();
  bool Commit();
  bool Delete();

  void SetObject(mdo::Object* object);
  void ResolveParents();
     
protected:

  m3do::Folder* m_Folder;
};

} //end namespace

#endif //_m3wsFolder_h_
