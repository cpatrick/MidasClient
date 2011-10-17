/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __mdsObject_h_
#define __mdsObject_h_

#include <string>
#include <vector>

#include "mdoProxyInterface.h"
#include "mdsDatabaseAPI.h"

namespace mds
{

/** This class represent an object in the local database */
class Object : public mdo::ProxyInterface
{
public:

  Object();
  ~Object();

  void MarkAsDirty();

protected:
  bool m_MarkDirty;

};

} // end namespace

#endif // _mdsObject_h_
