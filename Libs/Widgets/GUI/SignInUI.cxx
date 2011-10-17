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
#include "SignInUI.h"
#include "SignInThread.h"

#include "mwsWebAPI.h"
#include "midasAuthenticator.h"
#include "mdsDatabaseAPI.h"
#include "midasSynchronizer.h"

#include <QMessageBox>

/** Constructor */
SignInUI::SignInUI(QWidget* parent, midasSynchronizer* synch)
  : QDialog(parent), m_Synch(synch)
{
  setupUi(this);
  m_SignInThread = NULL;
  connect( createProfileButton, SIGNAL( released() ), this, SLOT( showCreateProfileDialog() ) );
}

SignInUI::~SignInUI()
{
  delete m_SignInThread;
}

/** */
void SignInUI::init()
{
  profileComboBox->clear();

  mds::DatabaseAPI         db;
  std::vector<std::string> profiles = db.GetAuthProfiles();
  for( std::vector<std::string>::iterator i = profiles.begin(); i != profiles.end(); ++i )
    {
    profileComboBox->addItem(i->c_str() );
    }
}

/** */
int SignInUI::exec()
{
  if( mds::DatabaseInfo::Instance()->GetPath() != "" )
    {
    this->init();
    return QDialog::exec();
    }
  else
    {
    QMessageBox::critical(this, "Error", "You must select a local database first.");
    return 0;
    }
}

/** */
void SignInUI::accept()
{
  if( this->profileComboBox->currentText().toStdString() == "" )
    {
    QMessageBox::critical(this, "Error", "Please select a profile or create a new one");
    return;
    }

  if( m_SignInThread )
    {
    disconnect(m_SignInThread);
    }
  delete m_SignInThread;
  m_SignInThread = new SignInThread(m_Synch);
  m_SignInThread->SetProfile(profileComboBox->currentText() );

  connect(m_SignInThread, SIGNAL( initialized(bool) ), this, SLOT( signIn(bool) ) );

  emit signingIn();

  m_SignInThread->start();
  QDialog::accept();
}

void SignInUI::signIn(bool ok)
{
  emit signedIn(ok);
}

void SignInUI::profileCreated(std::string name)
{
  init();
  profileComboBox->setCurrentIndex(profileComboBox->findText(name.c_str() ) );
}

void SignInUI::showCreateProfileDialog()
{
  emit createProfileRequest();
}

void SignInUI::removeProfile(std::string name)
{
  (void)name;
  init();
}

