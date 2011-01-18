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
#include <QFileDialog>

CreateProfileUI::CreateProfileUI(MIDASDesktopUI *parent):
  QDialog(parent), parent(parent)
{
  setupUi(this);

  connect(profileComboBox, SIGNAL( currentIndexChanged(const QString&) ),
    this, SLOT( fillData(const QString&) ) );
  connect(anonymousCheckBox, SIGNAL( stateChanged(int) ),
    this, SLOT( anonymousChanged(int) ) );
  connect(rootDirCheckbox, SIGNAL( stateChanged(int) ),
    this, SLOT( rootDirChecked(int) ) );
  connect(deleteButton, SIGNAL( clicked() ),
    this, SLOT( deleteProfile() ) );
  connect(browseButton, SIGNAL( clicked() ),
    this, SLOT( browseRootDir() ) );
}

void CreateProfileUI::init()
{
  emailEdit->setText("");
  profileNameEdit->setText("");
  passwordEdit->setText("");
  rootDirEdit->setText("");

  anonymousCheckBox->setChecked(false);
  rootDirCheckbox->setChecked(false);

  profileComboBox->clear();
  profileComboBox->addItem("New Profile");

  serverURLEdit->setText(
    parent->getDatabaseProxy()->GetSetting(midasDatabaseProxy::LAST_URL).c_str());

  std::vector<std::string> profiles = parent->getDatabaseProxy()->GetAuthProfiles();

  for(std::vector<std::string>::iterator i = profiles.begin(); i != profiles.end(); ++i)
    {
    profileComboBox->addItem(i->c_str());
    }
}

void CreateProfileUI::fillData(const QString& name)
{
  passwordEdit->setText("");
  if(profileComboBox->currentIndex() == 0)
    {
    profileNameEdit->setText("");
    emailEdit->setText("");
    serverURLEdit->setText("");
    deleteButton->setEnabled(false);
    }
  else
    {
    midasAuthProfile profile = parent->getDatabaseProxy()->GetAuthProfile(
      name.toStdString());

    profileNameEdit->setText(profile.Name.c_str());
    emailEdit->setText(profile.User.c_str());
    serverURLEdit->setText(profile.Url.c_str());
    rootDirEdit->setText(profile.RootDir.c_str());

    rootDirCheckbox->setCheckState(
      profile.HasRootDir() ? Qt::Unchecked : Qt::Checked);
    rootDirChecked(rootDirCheckbox->checkState());
    
    anonymousCheckBox->setCheckState(
      profile.IsAnonymous() ? Qt::Checked : Qt::Unchecked);
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
    passwordEdit->setText("");
    }
  emailEdit->setEnabled(!checked);
  passwordEdit->setEnabled(!checked);
}

void CreateProfileUI::rootDirChecked(int state)
{
  bool checked = state == Qt::Checked;
  if(!checked)
    {
    rootDirEdit->setText("");
    }
  rootDirEdit->setEnabled(checked);
  browseButton->setEnabled(checked);
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
    QMessageBox::critical(this, "Error", "You must enter a profile name.");
    return;
    }
  if(anonymousCheckBox->isChecked() && passwordEdit->text().toStdString() == "")
    {
    QMessageBox::critical(this, "Error", "You must enter a password or choose anonymous access.");
    return;
    }
  std::string profileName, email, password, serverURL, rootDir;
  profileName = profileNameEdit->text().trimmed().toStdString();
  email = emailEdit->text().trimmed().toStdString();
  password = passwordEdit->text().toStdString();
  serverURL = serverURLEdit->text().trimmed().toStdString();
  rootDir = rootDirEdit->text().trimmed().toStdString();
  kwsys::SystemTools::ConvertToUnixSlashes(rootDir);
  QDialog::accept();

  std::string apiKey = midasUtils::CreateDefaultAPIKey(email, password, "Default");

  emit serverURLSet(serverURL);
  emit createdProfile(profileName, email, "Default", apiKey, rootDir);
}

void CreateProfileUI::deleteProfile()
{
  std::string profileName = profileComboBox->currentText().toStdString();
  profileComboBox->removeItem(profileComboBox->currentIndex());

  parent->getDatabaseProxy()->DeleteProfile(profileName);

  std::stringstream text;
  text << "Deleted profile " << profileName;
  this->parent->GetLog()->Message(text.str());
  this->parent->displayStatus(text.str().c_str());

  init();

  QDialog::accept();
  emit deletedProfile(profileName);
}

void CreateProfileUI::browseRootDir()
{
  std::string path = rootDirEdit->text().trimmed().toStdString();
  if(path == "")
    {
    path = kwsys::SystemTools::GetCurrentWorkingDirectory();
    }

  QString dir = QFileDialog::getExistingDirectory(
    this, tr("Choose Root Directory"),
    path.c_str(),
    QFileDialog::ShowDirsOnly);

  if(dir != "")
    {
    rootDirEdit->setText(dir);
    }
}
