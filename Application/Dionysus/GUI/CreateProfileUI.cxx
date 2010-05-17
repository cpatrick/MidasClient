#include "CreateProfileUI.h"
#include "MidasClientGlobal.h"
#include "mwsSettings.h"
#include "DionysusUI.h"
#include "mwsWebAPI.h"
#include "midasAuthenticator.h"
#include "midasDatabaseProxy.h"

CreateProfileUI::CreateProfileUI(DionysusUI *parent):
  QDialog(parent), parent(parent)
{
  setupUi(this);
}

void CreateProfileUI::init()
{
  this->emailEdit->setText("");
  this->profileNameEdit->setText("");
  this->apiKeyEdit->setText("");
  this->apiNameEdit->setText("");
  parent->getDatabaseProxy()->Open();
  this->serverURLEdit->setText(
    parent->getDatabaseProxy()->GetSetting(midasDatabaseProxy::LAST_URL).c_str());
  parent->getDatabaseProxy()->Close();
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
