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


#ifndef _mdoProxyInterface_h_
#define _mdoProxyInterface_h_

#include <string>
#include <vector>

namespace mdo
{

class Object;

/** This class represent virtual class for interfaces such as WebAPI and
  database API. */
class ProxyInterface
{
public:

  ProxyInterface();
  ~ProxyInterface();

  /** Fetch data */
  virtual bool Fetch() = 0;

  /** Fetch tree */
  virtual bool FetchTree();

  /** Commit data */
  virtual bool Commit() = 0;

  /** Add an mdoObject */
  virtual void SetObject(Object* object) = 0;

  /** Set/Get if this interface is acting as a cache */
  void SetIsCache(bool iscache);

  bool GetIsCache();

  /** Set the name of the interface */
  void SetName(const char* name);

  /** Get the name of the interface */
  const char * GetName();

protected:

  bool        m_IsCache; // Does this interface acts as a default cache
  std::string m_Name;    // Name of the interface

};

} // end namespace

#endif // _mdoProxy_h_
