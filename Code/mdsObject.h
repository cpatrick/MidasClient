/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __mdsObject_h_
#define __mdsObject_h_

#include <string>
#include <vector>

#include "mdoProxyInterface.h"
#include "midasDatabaseProxy.h"

namespace mds{

/** This class represent an object in the local database */
class Object : public mdo::ProxyInterface
{
public:

  Object()
    {
    m_Database = NULL;
    m_MarkDirty = false;
    }
    
  ~Object(){};

  /** Set the Database */
  void SetDatabase(midasDatabaseProxy* database)
    {
    m_Database = database;
    }

  void MarkAsDirty()
    {
    m_MarkDirty = true;
    }

protected:
  
  midasDatabaseProxy* m_Database;
  bool                m_MarkDirty;
  
};

} //end namespace

#endif //_mdsObject_h_
