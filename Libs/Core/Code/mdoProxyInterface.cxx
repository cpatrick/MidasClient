/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mdoProxyInterface.h"
#include <sstream>
#include <iostream>

namespace mdo
{

/** Constructor */
ProxyInterface::ProxyInterface()
{
  m_IsCache = false;
  m_Name = "No Name";
}

/** Destructor */
ProxyInterface::~ProxyInterface()
{
}

/** Set the name of the interface */
void ProxyInterface::SetName(const char* name)
{
  m_Name = name;
}

/** Fetch a tree */
bool ProxyInterface::FetchTree()
{
  std::cerr << m_Name.c_str()
            << " ProxyInterface::FetchTree(): Not implemented" << std::endl;
  return false;
}

/** Set if this interface is acting as a cache */
void ProxyInterface::SetIsCache(bool iscache)
{
  m_IsCache = iscache;
}

/** Get if this interface is acting as a cache */
bool ProxyInterface::GetIsCache()
{
  return m_IsCache;
}

const char * ProxyInterface::GetName()
{
  return m_Name.c_str();
}

} // end namespace
