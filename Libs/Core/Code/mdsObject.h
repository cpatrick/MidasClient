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
