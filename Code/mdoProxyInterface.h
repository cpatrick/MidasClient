/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef _mdoProxyInterface_h_
#define _mdoProxyInterface_h_

#include <string>
#include <vector>

namespace mdo{

class Object;

/** This class represent virtual class for interfaces such as WebAPI and database API. */
class ProxyInterface
{
public:

  ProxyInterface();
  ~ProxyInterface();

  /** Fetch data */
  virtual bool Fetch() = 0;
  
  /** Fetch tree */
  virtual bool FetchTree();
  
  /** Commit data */
  virtual bool Commit() = 0;

  /** Delete the object */
  virtual bool Delete();

  /** Add an mdoObject */
  virtual void SetObject(Object* object) = 0;
  
  /** Set/Get if this interface is acting as a cache */
  void SetIsCache(bool iscache);
  bool GetIsCache();
  
  /** Set the name of the interface */
  void SetName(const char* name);
  
  /** Get the name of the interface */
  const char* GetName() {return m_Name.c_str();}
  
protected:
   
  bool m_IsCache; // Does this interface acts as a default cache
  std::string m_Name; // Name of the interface
    
};

} //end namespace

#endif //_mdoProxy_h_
