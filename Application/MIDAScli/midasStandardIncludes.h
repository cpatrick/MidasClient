#ifndef MIDASSTANDARDINCLUDES_H
#define MIDASSTANDARDINCLUDES_H

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

#include <kwsys/SystemTools.hxx>

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
