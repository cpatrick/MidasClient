#include "CreateProfileUI.h"
#include "MidasClientGlobal.h"
#include "mwsSettings.h"
#include "MIDASDesktopUI.h"
#include "mwsWebAPI.h"
#include "midasAuthenticator.h"
#include "midasDatabaseProxy.h"
#include <QString>
#include <QCheckBox>
#include <QMessageBox>

CreateProfileUI::CreateProfileUI(MIDASDesktopUI *parent):
  QDialog(parent), parent(parent)
{
  setupUi(this);

  connect(profileComboBox, SIGNAL( currentIndexChanged(const QString&) ),
    this, SLOT( fillData(const QString&) ) );
  connect(anonymousCheckBox, SIGNAL( stateChanged(int) ),
    this, SLOT( anonymousChanged(int) ) );
  connect(deleteButton, SIGNAL( clicked() ),
    this, SLOT( deleteProfile() ) );
}

void CreateProfileUI::init()
{
  emailEdit->setText("");
  profileNameEdit->setText("");
  apiKeyEdit->setText("");
  apiNameEdit->setText("");

  profileComboBox->clear();
  profileComboBox->addItem("New Profile");

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
  if(profileComboBox->currentIndex() == 0)
    {
    profileNameEdit->setText("");
    emailEdit->setText("");
    apiKeyEdit->setText("");
    apiNameEdit->setText(this->anonymousCheckBox->isChecked() ? "" : "MIDASDesktop");
    serverURLEdit->setText("");
    deleteButton->setEnabled(false);
    }
  else
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
    
    anonymousCheckBox->setCheckState(
      profile.User == "" ? Qt::Checked : Qt::Unchecked);
    anonymousChanged(anonymousCheckBox->checkState());
    deleteButton->setEnabled(true);
    }
}

void CreateProfileUI::anonymousChanged(int state)
{
  bool checked = state == Qt::Checked;
  if(checked)
    {
    emailEdit->setText("");
    apiKeyEdit->setText("");
    apiNameEdit->setText("");
    }
  emailEdit->setEnabled(!checked);
  apiKeyEdit->setEnabled(!checked);
  apiNameEdit->setEnabled(!checked);
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
  if(profileNameEdit->text().trimmed().toStdString() == "")
    {
    QMessageBox::critical(this, "Error", "You must enter a profile name");
    return;
    }
  std::string profileName, email, apiName, apiKey, serverURL;
  profileName = profileNameEdit->text().trimmed().toStdString();
  email = emailEdit->text().trimmed().toStdString();
  apiName = apiNameEdit->text().trimmed().toStdString();
  apiKey = apiKeyEdit->text().trimmed().toStdString();
  serverURL = serverURLEdit->text().trimmed().toStdString();
  QDialog::accept();

  emit serverURLSet(serverURL);
  emit createdProfile(profileName, email, apiName, apiKey);
}

void CreateProfileUI::deleteProfile()
{
  std::string profileName = profileComboBox->currentText().toStdString();
  profileComboBox->removeItem(profileComboBox->currentIndex());

  parent->getDatabaseProxy()->Open();
  parent->getDatabaseProxy()->DeleteProfile(profileName);
  parent->getDatabaseProxy()->Close();

  std::stringstream text;
  text << "Deleted profile " << profileName;
  this->parent->GetLog()->Message(text.str());
  this->parent->displayStatus(text.str().c_str());

  init();

  QDialog::accept();
  emit deletedProfile(profileName);
}
