/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef _mdsResourceUpdateHandler_h_
#define _mdsResourceUpdateHandler_h_

#include "midasStandardIncludes.h"

namespace mdo{
  class Object;
}

namespace mds{
/**
 * Abstract class which will receive a callback anytime a
 * is inserted into the client database
 */
class ResourceUpdateHandler
{
public:
  ResourceUpdateHandler() {}
  virtual ~ResourceUpdateHandler() {}

  virtual void AddedResource(mdo::Object* resource) = 0;
  virtual void DeletedResource(mdo::Object* resource) = 0;
  virtual void UpdatedResource(mdo::Object* resource) = 0;

};

} //end namespace

#endif
