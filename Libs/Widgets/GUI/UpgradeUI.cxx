#include "UpgradeUI.h"

UpgradeUI::UpgradeUI(QWidget* parent)
: QDialog(parent)
{
  this->setupUi(this);
}

UpgradeUI::~UpgradeUI()
{
}

void UpgradeUI::SetDbVersion(const std::string& version)
{
  m_DbVersion = version;
}

void UpgradeUI::SetProductVersion(const std::string& version)
{
  m_ProductVersion = version;
}

int UpgradeUI::exec()
{
  std::stringstream text;
  text << "You are using an outdated database version ("
    << m_DbVersion << "). You must upgrade your database schema "
    " to the current version (" << m_ProductVersion << ") before "
    " you can use it.  Your existing data will be preserved.<br><br>"
    "Would you like to upgrade the database now?";
  this->upgradeLabel->setText(text.str().c_str());
  return QDialog::exec();
}
