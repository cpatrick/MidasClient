/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mdoProxy.h"
#include <sstream>
#include <iostream>
#include "mdoProxyInterface.h"
#include "mdoObject.h"

namespace mdo{

/** Constructor */
Proxy::Proxy()
{
  m_Object = 0;
}
  
/** Destructor */
Proxy::~Proxy()
{
}

/** Add a proxy interface.
 *  Interfaces should be added by order of priority in terms of access */
void Proxy::AddInterface(ProxyInterface* interf)
{
  interf->SetObject(m_Object);
  m_Interfaces.push_back(interf);
}

/** Return the number of attached interfaces */
unsigned int Proxy::GetNumberOfInterfaces()
{
  return m_Interfaces.size();
}
  
/** Set Object*/
void Proxy::SetObject(Object* object)
{
  m_Object = object;
  std::vector<ProxyInterface*>::iterator it =  m_Interfaces.begin();
  while(it != m_Interfaces.end())
    {
    (*it)->SetObject(m_Object);
    it++;
    }
}
  
/** Load function. This function dispatch the load based on the interfaces. */
bool Proxy::Load()
{
  if(m_Interfaces.size() == 0)
    {
    std::cerr << "Proxy::Load Interface not set" << std::endl;
    return false;
    }
  
  std::vector<ProxyInterface*>::iterator it = m_Interfaces.begin();
  while(it != m_Interfaces.end())
    {
    if((*it)->Fetch())
      {
      return true;
      }
    it++;   
    } 
  return false;
}

/** Load function. This function dispatch the load based on the interfaces. */
bool Proxy::LoadTree()
{
  if(m_Interfaces.size() == 0)
    {
    std::cerr << "Proxy::Load Interface not set" << std::endl;
    return false;
    }
    
  std::vector<ProxyInterface*>::iterator it = m_Interfaces.begin();
  while(it != m_Interfaces.end())
    {
    if((*it)->FetchTree())
      {
      return true;
      }
    it++;  
    } 
  return false;
}

} // end namespace
