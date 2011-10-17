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

class midasAuthenticator;

namespace mws
{

/** This class represent an object on the MIDAS server. */
class Object : public mdo::ProxyInterface
{
public:

  Object();
  ~Object();

  void SetAuthenticator(midasAuthenticator* auth);

protected:
  midasAuthenticator* m_Auth;

};

} // end namespace

#endif // _mwsObject_h_
