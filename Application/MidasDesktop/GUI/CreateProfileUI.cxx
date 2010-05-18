#include "CreateProfileUI.h"
#include "MidasClientGlobal.h"
#include "mwsSettings.h"
#include "MIDASDesktopUI.h"
#include "mwsWebAPI.h"
#include "midasAuthenticator.h"
#include "midasDatabaseProxy.h"
#include <QString>

CreateProfileUI::CreateProfileUI(DionysusUI *parent):
  QDialog(parent), parent(parent)
{
  setupUi(this);

  connect(profileComboBox, SIGNAL( currentIndexChanged(const QString&) ),
    this, SLOT( fillData(const QString&) ) );
}

void CreateProfileUI::init()
{
  emailEdit->setText("");
  profileNameEdit->setText("");
  apiKeyEdit->setText("");
  apiNameEdit->setText("");

  profileComboBox->clear();
  profileComboBox->addItem("");

  parent->getDatabaseProxy()->Open();
  serverURLEdit->setText(
    parent->getDatabaseProxy()->GetSetting(midasDatabaseProxy::LAST_URL).c_str());

  std::vector<std::string> profiles = parent->getDatabaseProxy()->GetAuthProfiles();

  for(std::vector<std::string>::iterator i = profiles.begin(); i != profiles.end(); ++i)
    {
    profileComboBox->addItem(i->c_str());
    }
  parent->getDatabaseProxy()->Close();
}

void CreateProfileUI::fillData(const QString& name)
{
  parent->getDatabaseProxy()->Open();
  midasAuthProfile profile = parent->getDatabaseProxy()->GetAuthProfile(
    name.toStdString());
  parent->getDatabaseProxy()->Close();

  profileNameEdit->setText(profile.Name.c_str());
  emailEdit->setText(profile.User.c_str());
  apiKeyEdit->setText(profile.ApiKey.c_str());
  apiNameEdit->setText(profile.AppName.c_str());
  serverURLEdit->setText(profile.Url.c_str());
}

int CreateProfileUI::exec()
{
  if(this->parent->getDatabaseProxy())
    {
    this->init();
    return QDialog::exec();
    }
  else
    {
    this->parent->displayStatus(tr("You must select a local database first."));
    return 0;
    }
}

void CreateProfileUI::accept()
{
  std::string profileName, email, apiName, apiKey, serverURL;
  profileName = profileNameEdit->text().toStdString();
  email = emailEdit->text().toStdString();
  apiName = apiNameEdit->text().toStdString();
  apiKey = apiKeyEdit->text().toStdString();
  serverURL = serverURLEdit->text().toStdString();
  QDialog::accept();

  emit serverURLSet(serverURL);
  emit createdProfile(profileName, email, apiName, apiKey);
}
