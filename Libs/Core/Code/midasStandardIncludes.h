/******************************************************************************
 * Copyright 2011 Kitware Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/


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

#include <QObject>

typedef qint64 int64;

#include "MIDASConfig.h"
#include "midasUtils.h"

#define INVALID_POLICY -151

// For resource_uuid.resource_type_id
struct midasResourceType
  {
  enum ResourceType
    {
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

struct midas3ResourceType
  {
  enum ResourceType
    {
    BITSTREAM = 0,
    ITEM = 1,
    USER = 2,
    REVISION = 3,
    FOLDER = 4,
    ASSETSTORE = 5,
    COMMUNITY = 6,
    TYPE_ERROR = -1,
    NONE = -2
    };
  };

// For marking local resources dirty
struct midasDirtyAction
  {
  enum Action
    {
    MODIFIED = 1,
    ADDED = 2,
    REMOVED = 3
    };
  };

#endif
