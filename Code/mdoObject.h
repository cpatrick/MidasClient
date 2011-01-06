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

namespace mdo{

/** This class represent an object on the MIDAS server. */
class Object
{
public:

  Object()
    {
    m_Proxy = new Proxy();
    m_Proxy->SetObject(this);
    m_Dirty = false;
    m_HasAgreement = "0";
    }
    
  virtual ~Object()
    {
    delete m_Proxy;
    };

  // Set/Get ID
  void SetId(int id) { m_Id = id; }
  const int GetId() {return m_Id; }

    // Set/Get the uuid
  void SetUuid(const char* uuid) { m_Uuid = uuid; }
  std::string & GetUuid() { return m_Uuid; }
    
  /** Get the default proxy */
  Proxy* GetProxy() {return m_Proxy;}
  virtual bool IsDirty() { return m_Dirty; }
  virtual void SetDirty(bool dirty) { m_Dirty = dirty; }
  virtual void Clear() = 0;
  virtual std::string & GetName() = 0;
  virtual std::string GetTypeName() = 0;

  virtual std::string & RefAgreement() { return m_HasAgreement; }
  virtual bool HasAgreement() { return m_HasAgreement == "1"; }

  virtual bool SetValue(std::string key,
                        std::string value,
                        bool append = false)
    {
    return false;
    }
    
protected:

  Proxy* m_Proxy;
  bool m_Dirty;
  std::string m_HasAgreement;
  std::string m_Uuid;
  int m_Id;
};

} //end namespace

#endif //_mdoObject_h_
