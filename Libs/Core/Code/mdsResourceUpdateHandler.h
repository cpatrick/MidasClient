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


#ifndef _mdsResourceUpdateHandler_h_
#define _mdsResourceUpdateHandler_h_

#include "midasStandardIncludes.h"

namespace mdo
{
class Object;
}

namespace mds
{
/**
 * Abstract class which will receive a callback anytime a
 * is inserted into the client database
 */
class ResourceUpdateHandler
{
public:
  ResourceUpdateHandler()
  {
  }
  virtual ~ResourceUpdateHandler()
  {
  }

  virtual void AddedResource(mdo::Object* resource) = 0;

  virtual void DeletedResource(mdo::Object* resource) = 0;

  virtual void UpdatedResource(mdo::Object* resource) = 0;

};

} // end namespace

#endif
