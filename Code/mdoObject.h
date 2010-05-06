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
    }
    
  ~Object()
    {
    delete m_Proxy;
    };
    
  /** Get the default proxy */
  Proxy* GetProxy() {return m_Proxy;}
  virtual bool IsDirty() { return m_Dirty; }
  virtual void SetDirty(bool dirty) { m_Dirty = dirty; }
  virtual void Clear() = 0;
    
protected:

  Proxy* m_Proxy;
  bool m_Dirty;
};

} //end namespace

#endif //_mdoObject_h_
