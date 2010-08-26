/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __MIDASSTANDARDINCLUDES_H
#define __MIDASSTANDARDINCLUDES_H

#include <vector>
#include <string>
#include <iterator>
#include <algorithm>
#include <functional>
#include <map>
#include <list>
#include <set>
#include <deque>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "MIDASConfig.h"
#include "midasUtils.h"

#define INVALID_POLICY -151

// For resource_uuid.resource_type_id
struct midasResourceType
{
  enum ResourceType {
    BITSTREAM = 0,
    BUNDLE = 1,
    ITEM = 2,
    COLLECTION = 3,
    COMMUNITY = 4,
    PROJECT = 5,
    TYPE_ERROR = -1,
    NONE = -2
  };
};

// For marking local resources dirty
struct midasDirtyAction
{
  enum Action {
    MODIFIED = 1,
    ADDED = 2,
    REMOVED = 3
  };
};

#endif
