/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

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

  Object()
  {
    m_Proxy = new Proxy();
    m_Proxy->SetObject(this);
    m_Dirty = false;
    m_Fetched = false;
    m_HasAgreement = "";
    m_Size = "";
  }

  virtual ~Object()
  {
    delete m_Proxy;
  };

  // Set/Get ID
  void SetId(int id)
  {
    m_Id = id;
  }
  int GetId()
  {
    return m_Id;
  }

  // Set/Get the ID of the parent
  void SetParentId(std::string id)
  {
    m_Parent = id;
  }
  void SetParentId(int id)
  {
    std::stringstream str;

    str << id;
    m_Parent = str.str();
  }

  std::string & GetParentStr()
  {
    return m_Parent;
  }
  int GetParentId()
  {
    return atoi(m_Parent.c_str() );
  }

  // Set/Get the uuid
  void SetUuid(const char* uuid)
  {
    m_Uuid = uuid;
  }
  std::string & GetUuid()
  {
    return m_Uuid;
  }

  /** Get the default proxy */
  Proxy * GetProxy()
  {
    return m_Proxy;
  }
  virtual bool IsDirty()
  {
    return m_Dirty;
  }
  virtual void SetDirty(bool dirty)
  {
    m_Dirty = dirty;
  }
  virtual void Clear() = 0;

  virtual std::string & GetName() = 0;

  virtual int GetResourceType() = 0;

  virtual std::string GetTypeName() = 0;

  virtual std::string & RefAgreement()
  {
    return m_HasAgreement;
  }
  virtual bool HasAgreement()
  {
    return m_HasAgreement == "1";
  }

  virtual bool SetValue(std::string key,
                        std::string value,
                        bool append = false)
  {
    (void)key;
    (void)value;
    (void)append;
    return false;
  }

  virtual void SetSize(std::string size)
  {
    m_Size = size;
  }
  virtual std::string & GetSize()
  {
    return m_Size;
  }

  virtual bool IsFetched()
  {
    return m_Fetched;
  }
  virtual void SetFetched(bool val)
  {
    m_Fetched = val;
  }
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
