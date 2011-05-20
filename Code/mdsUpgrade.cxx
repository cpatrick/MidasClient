#include "mdsUpgrade.h"
#include "mdsDatabaseAPI.h"
#include "mdsVersion.h"
#include "mdoVersion.h"

namespace mds {

bool Upgrade::UpgradeDatabase(const std::string& path, mdo::Version dbVersion)
{
  mds::DatabaseInfo::Instance()->SetPath(path);

  bool status = true;

  mdo::Version v1_8_0(1, 8, 0);

  if(dbVersion < v1_8_0)
    {
    status &= Upgrade::Upgrade1_8_0();
    }
  // Add more version upgrades here when needed

  if(!status)
    {
    return false;
    }
  
  // Set db version to current version
  mdo::Version currentVersion(MIDAS_CLIENT_VERSION_MAJOR,
                              MIDAS_CLIENT_VERSION_MINOR,
                              MIDAS_CLIENT_VERSION_PATCH);

  mds::Version mdsVersion;
  mdsVersion.SetObject(currentVersion);
  status &= mdsVersion.Commit();
  return status;
}

bool Upgrade::Upgrade1_8_0()
{
  bool status = true;

  // Empty set of changes (first version where we have this upgrade mechanism)
  mds::Version version;
  version.SetObject(mdo::Version(1, 8, 0));
  status &= version.Commit();

  return status;
}

}