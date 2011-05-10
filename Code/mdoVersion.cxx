#include "mdoVersion.h"

namespace mdo
{

Version::Version()
: Major(0), Minor(0), Patch(0)
{
}

Version::Version(int major, int minor, int patch)
: Major(major), Minor(minor), Patch(patch)
{
}

Version::~Version()
{
}

std::string Version::VersionString()
{
  std::stringstream str;
  str << Major << "." << Minor << "." << Patch;
  return str.str();
}

bool operator> (Version& v1, Version& v2)
{
  if(v1.Major > v2.Major)
    {
    return true;
    }
  if(v1.Major < v2.Major)
    {
    return false;
    }

  if(v1.Minor > v2.Minor)
    {
    return true;
    }
  if(v1.Minor < v2.Minor)
    {
    return false;
    }

  return v1.Patch > v2.Patch;
}

bool operator< (Version& v1, Version& v2)
{
  if(v1.Major < v2.Major)
    {
    return true;
    }
  if(v1.Major > v2.Major)
    {
    return false;
    }

  if(v1.Minor < v2.Minor)
    {
    return true;
    }
  if(v1.Minor > v2.Minor)
    {
    return false;
    }

  return v1.Patch < v2.Patch;
}

bool operator>=(Version& v1, Version& v2)
{
  return !(v1 < v2);
}

bool operator<=(Version& v1, Version& v2)
{
  return !(v1 > v2);
}

bool operator==(Version& v1, Version& v2)
{
  return v1.Major == v2.Major &&
         v1.Minor == v2.Minor &&
         v1.Patch == v2.Patch;
}

bool operator!=(Version& v1, Version& v2)
{
  return !(v1 == v2);
}

} // end namespace