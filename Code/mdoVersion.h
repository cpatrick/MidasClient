#ifndef __MDOVERSION_H
#define __MDOVERSION_H

#include "midasStandardIncludes.h"

namespace mdo {

/**
 * Data structure for a version (major, minor, patch)
 */
class Version
{
public:
  Version();
  Version(int major, int minor, int patch);
  ~Version();

  std::string VersionString();

  friend bool operator> (Version& v1, Version& v2);
  friend bool operator< (Version& v1, Version& v2);
  friend bool operator>=(Version& v1, Version& v2);
  friend bool operator<=(Version& v1, Version& v2);
  friend bool operator==(Version& v1, Version& v2);
  friend bool operator!=(Version& v1, Version& v2);

  int Major;
  int Minor;
  int Patch;
  std::string Name;
};

} // end namespace

#endif