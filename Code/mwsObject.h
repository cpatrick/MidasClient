/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef _mwsObject_h_
#define _mwsObject_h_

#include <string>
#include <vector>

#include "mwsWebAPI.h"
#include "mdoProxyInterface.h"

namespace mws{

/** This class represent an object on the MIDAS server. */
class Object : public mdo::ProxyInterface
{
public:

  Object()
    {
    m_WebAPI = NULL;
    }
    
  ~Object(){};

  /** Set the WebAPI */
  void SetWebAPI(WebAPI* api)
    {
    m_WebAPI = api;
    }

  virtual void ResolveParents() = 0;

protected:

  WebAPI* m_WebAPI;
  
};

} //end namespace

#endif //_mwsObject_h_
