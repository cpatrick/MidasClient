/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef _mdsObject_h_
#define _mdsObject_h_

#include <string>
#include <vector>

#include "mdoProxyInterface.h"
#include "mdsDatabase.h"

namespace mds{

/** This class represent an object on the MIDAS server. */
class Object : public mdo::ProxyInterface
{
public:

  Object()
    {
    m_Database = NULL;
    }
    
  ~Object(){};

  /** Set the Database */
  void SetDatabase(Database* database)
    {
    m_Database = database;
    }

protected:
  
  Database* m_Database;
  
};

} //end namespace

#endif //_mdsObject_h_
