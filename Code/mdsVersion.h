#ifndef __MDSVERSION_H
#define __MDSVERSION_H

#include "midasStandardIncludes.h"
#include "mdoVersion.h"

namespace mds {

class Version
{
public:
  Version();
  ~Version();

  void SetObject(mdo::Version object);

  bool Commit();
  bool Fetch();

protected:
  mdo::Version m_Version;
};

} // end namespace

#endif