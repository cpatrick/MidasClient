/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "mwsObject.h"

namespace mws
{

Object::Object()
{
  m_Auth = NULL;
}

Object::~Object()
{
}

void Object::SetAuthenticator(midasAuthenticator* auth)
{
  m_Auth = auth;
}

}
