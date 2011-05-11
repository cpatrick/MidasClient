#include "GUIUpgradeHandler.h"
#include "UpgradeUI.h"
#include "mdoVersion.h"
#include "mdsUpgrade.h"

GUIUpgradeHandler::GUIUpgradeHandler(UpgradeUI* ui)
{
  m_Dialog = ui;
}

GUIUpgradeHandler::~GUIUpgradeHandler()
{
}

bool GUIUpgradeHandler::Upgrade(const std::string& path,
                                mdo::Version dbVersion,
                                mdo::Version productVersion)
{
  m_Dialog->SetDbVersion(dbVersion.VersionString());
  m_Dialog->SetProductVersion(productVersion.VersionString());

  if(m_Dialog->exec() == QDialog::Accepted)
    {
    Log->Message("Upgrading database schema to version " + productVersion.VersionString());
    if(mds::Upgrade::UpgradeDatabase(path, dbVersion))
      {
      Log->Message("Database upgrade successful");
      return true;
      }
    else
      {
      Log->Error("Database upgrade failed");
      return false;
      }
    }
  return false;
}
