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


#ifndef _mdoProxy_h_
#define _mdoProxy_h_

#include <string>
#include <vector>

namespace mdo
{

class ProxyInterface;
class Object;

/** This class represent a proxy for MIDAS Data Objects. */
class Proxy
{
public:

  Proxy();
  ~Proxy();

  /** Add a proxy interface */
  void AddInterface(ProxyInterface* interf);

  /** Set Object*/
  void SetObject(Object* object);

  /** Load function. This function dispatch the load based on the interfaces. */
  bool Load();

  /** LoadTree function. This function dispatch the load based on the
    interfaces. */
  bool LoadTree();

  /** Return the number of attached interfaces */
  unsigned int GetNumberOfInterfaces();

protected:

  std::vector<ProxyInterface *> m_Interfaces;
  Object*                       m_Object;
};

} // end namespace

#endif // _mdoProxy_h_
