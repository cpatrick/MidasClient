#include "GUIUpgradeHandler.h"
#include "UpgradeUI.h"
#include "mdoVersion.h"

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
    //TODO upgrade the database
    return true;
    }
  return false;
}
