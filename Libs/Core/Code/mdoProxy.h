/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef _mdoProxy_h_
#define _mdoProxy_h_

#include <string>
#include <vector>

namespace mdo
{

class ProxyInterface;
class Object;

/** This class represent a proxy for MIDAS Data Objects. */
class Proxy
{
public:

  Proxy();
  ~Proxy();

  /** Add a proxy interface */
  void AddInterface(ProxyInterface* interf);

  /** Set Object*/
  void SetObject(Object* object);

  /** Load function. This function dispatch the load based on the interfaces. */
  bool Load();

  /** LoadTree function. This function dispatch the load based on the
    interfaces. */
  bool LoadTree();

  /** Return the number of attached interfaces */
  unsigned int GetNumberOfInterfaces();

protected:

  std::vector<ProxyInterface *> m_Interfaces;
  Object*                       m_Object;
};

} // end namespace

#endif // _mdoProxy_h_
