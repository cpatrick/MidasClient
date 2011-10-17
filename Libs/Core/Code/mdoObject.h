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


#ifndef _mdoObject_h_
#define _mdoObject_h_

#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

#include "mdoProxy.h"
#include "midasStandardIncludes.h"

namespace mdo
{

/** This class represent an object on the MIDAS server. */
class Object
{
public:

  Object();
  virtual ~Object();

  // Set/Get ID
  void SetId(int id);
  int GetId();

  // Set/Get the ID of the parent
  void SetParentId(std::string id);
  void SetParentId(int id);

  std::string & GetParentStr();
  int GetParentId();

  // Set/Get the uuid
  void SetUuid(const char* uuid);
  std::string & GetUuid();

  /** Get the default proxy */
  Proxy * GetProxy();
  virtual bool IsDirty();
  virtual void SetDirty(bool dirty);

  virtual void Clear() = 0;
  virtual std::string & GetName() = 0;

  virtual int GetResourceType() = 0;

  virtual std::string GetTypeName() = 0;

  virtual std::string & RefAgreement();
  virtual bool HasAgreement();

  virtual bool SetValue(std::string key,
                        std::string value,
                        bool append = false);

  virtual void SetSize(std::string size);
  virtual std::string & GetSize();

  virtual bool IsFetched();
  virtual void SetFetched(bool val);

protected:

  Proxy*      m_Proxy;
  bool        m_Dirty;
  bool        m_Fetched;
  std::string m_HasAgreement;
  std::string m_Uuid;
  std::string m_Size;
  std::string m_Parent;
  int         m_Id;
};

} // end namespace

#endif // _mdoObject_h_
