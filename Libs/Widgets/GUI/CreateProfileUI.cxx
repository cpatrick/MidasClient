/******************************************************************************
 * Copyright 2011 Kitware Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/
#include "CreateProfileUI.h"
#include "mwsWebAPI.h"
#include "midasAuthenticator.h"
#include "mdsDatabaseAPI.h"
#include <QString>
#include <QCheckBox>
#include <QMessageBox>
#include <QFileDialog>

CreateProfileUI::CreateProfileUI(QWidget* parent)
  : QDialog(parent)
{
  setupUi(this);

  connect(profileComboBox, SIGNAL(currentIndexChanged(const QString &) ),
          this, SLOT(fillData(const QString &) ) );
  connect(anonymousCheckBox, SIGNAL(stateChanged(int) ),
          this, SLOT( anonymousChanged(int) ) );
  connect(rootDirCheckbox, SIGNAL(stateChanged(int) ),
          this, SLOT(rootDirChecked(int) ) );
  connect(deleteButton, SIGNAL(clicked() ),
          this, SLOT(deleteProfile() ) );
  connect(browseButton, SIGNAL(clicked() ),
          this, SLOT(browseRootDir() ) );
}

CreateProfileUI::~CreateProfileUI()
{
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

  mds::DatabaseAPI db;
  serverURLEdit->setText(db.GetSetting(mds::DatabaseAPI::LAST_URL).c_str() );

  std::vector<std::string> profiles = db.GetAuthProfiles();
  for( std::vector<std::string>::iterator i = profiles.begin(); i != profiles.end(); ++i )
    {
    profileComboBox->addItem(i->c_str() );
    }
}

void CreateProfileUI::fillData(const QString& name)
{
  passwordEdit->setText("");
  if( profileComboBox->currentIndex() == 0 )
    {
    profileNameEdit->setText("");
    emailEdit->setText("");
    serverURLEdit->setText("");
    deleteButton->setEnabled(false);
    }
  else
    {
    mds::DatabaseAPI db;
    midasAuthProfile profile = db.GetAuthProfile(
        name.toStdString() );

    profileNameEdit->setText(profile.Name.c_str() );
    emailEdit->setText(profile.User.c_str() );
    serverURLEdit->setText(profile.Url.c_str() );
    rootDirEdit->setText(profile.RootDir.c_str() );

    rootDirCheckbox->setCheckState(
      profile.HasRootDir() ? Qt::Unchecked : Qt::Checked);
    rootDirChecked(rootDirCheckbox->checkState() );

    anonymousCheckBox->setCheckState(
      profile.IsAnonymous() ? Qt::Checked : Qt::Unchecked);
    anonymousChanged(anonymousCheckBox->checkState() );
    deleteButton->setEnabled(true);
    }
}

void CreateProfileUI::anonymousChanged(int state)
{
  bool checked = state == Qt::Checked;

  if( checked )
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

  if( !checked )
    {
    rootDirEdit->setText("");
    }
  rootDirEdit->setEnabled(checked);
  browseButton->setEnabled(checked);
}

int CreateProfileUI::exec()
{
  if( mds::DatabaseInfo::Instance()->GetPath() != "" )
    {
    this->init();
    return QDialog::exec();
    }
  else
    {
    QMessageBox::critical(this, "Error", "You must select a local database.");
    return 0;
    }
}

void CreateProfileUI::accept()
{
  if( profileNameEdit->text().trimmed().isEmpty() )
    {
    QMessageBox::critical(this, "Error", "You must enter a profile name.");
    return;
    }
  if( !anonymousCheckBox->isChecked() && emailEdit->text().trimmed().isEmpty() )
    {
    QMessageBox::critical(this, "Error", "You must enter an email or choose anonymous access.");
    }
  if( !anonymousCheckBox->isChecked() && passwordEdit->text().isEmpty() )
    {
    QMessageBox::critical(this, "Error", "You must enter a password or choose anonymous access.");
    return;
    }
  std::string profileName, email, password, serverURL, rootDir;
  profileName = profileNameEdit->text().trimmed().toStdString();
  email = emailEdit->text().trimmed().toStdString();
  password = passwordEdit->text().toStdString();
  serverURL = serverURLEdit->text().trimmed().toStdString();
  rootDir = rootDirEdit->text() == "" ? "" :
    QDir(rootDirEdit->text().trimmed() ).path().toStdString();

  QDialog::accept();

  emit createdProfile(profileName, email, "Default", password, rootDir, serverURL);
}

void CreateProfileUI::deleteProfile()
{
  std::string profileName = profileComboBox->currentText().toStdString();

  profileComboBox->removeItem(profileComboBox->currentIndex() );

  mds::DatabaseAPI db;
  db.DeleteProfile(profileName);

  std::stringstream text;
  text << "Deleted profile " << profileName;
  db.GetLog()->Message(text.str() );

  init();

  QDialog::accept();
  emit deletedProfile(profileName);
}

void CreateProfileUI::browseRootDir()
{
  QString path = rootDirEdit->text().trimmed();

  if( path == "" )
    {
    path = QDir::currentPath();
    }

  QString dir = QFileDialog::getExistingDirectory(
      this, tr("Choose Root Directory"), path, QFileDialog::ShowDirsOnly);

  if( dir != "" )
    {
    rootDirEdit->setText(dir);
    }
}

