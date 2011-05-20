/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __MDSUPGRADEHANDLER_H
#define __MDSUPGRADEHANDLER_H

#include "midasStandardIncludes.h"
#include "midasLogAware.h"
#include "mdoVersion.h"

/**
 * Abstract class for handling a database upgrade
 */
namespace mds
{

class UpgradeHandler : public midasLogAware
{
public:
  UpgradeHandler() {}
  virtual ~UpgradeHandler() {}

  /**
   * Returns true if the database at <path> was successfully upgraded
   * from <dbVersion> to <productVersion>
   */
  virtual bool Upgrade(const std::string& path,
                       mdo::Version dbVersion,
                       mdo::Version productVersion) = 0;
};

} // end namespace
#endif
