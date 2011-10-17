#include "mdoVersion.h"
#include "midasUtils.h"

namespace mdo
{

Version::Version()
: Major(0), Minor(0), Patch(0), Name("MIDASClient")
{
}

Version::Version(int major, int minor, int patch, std::string name)
: Major(major), Minor(minor), Patch(patch), Name(name)
{
}

Version::Version(const std::string& versionString)
{
  std::vector<std::string> tokens;
  midasUtils::Tokenize(versionString, tokens, " .");
  
  if(tokens.size() >= 3)
    {
    Major = atoi(tokens[0].c_str());
    Minor = atoi(tokens[1].c_str());
    Patch = atoi(tokens[2].c_str());
    }
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