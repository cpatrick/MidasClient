/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __MDSUPGRADE_H
#define __MDSUPGRADE_H

#include "midasStandardIncludes.h"
#include "mdoVersion.h"

namespace mds
{

class Upgrade
{
public:
  static bool UpgradeDatabase(const std::string& path, mdo::Version dbVersion);

protected:
  static bool Upgrade1_8_0();

  static bool Upgrade1_8_2();

};

} // end namespace mds
#endif
