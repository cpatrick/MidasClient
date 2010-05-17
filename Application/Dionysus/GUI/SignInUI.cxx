#include "SignInUI.h"
#include "MidasClientGlobal.h"
#include "mwsSettings.h"
#include "DionysusUI.h"
#include "mwsWebAPI.h"
#include "midasAuthenticator.h"
#include "midasDatabaseProxy.h"
#include "midasSynchronizer.h"

/** Constructor */
SignInUI::SignInUI(DionysusUI *parent):
  QDialog(parent), parent(parent)
{
  setupUi(this);
  connect( editServerSettingsButton, SIGNAL( released() ), this, SLOT( editServerSettings() ) );
  connect( createProfileButton, SIGNAL( released() ), this, SLOT( showCreateProfileDialog() ) );
}

/** */
void SignInUI::init()
{
  while(profileComboBox->count())
    {
    //clear all old options before we add them
    profileComboBox->removeItem(0);
    }
  parent->getDatabaseProxy()->Open();
  serverEdit->setText( parent->getDatabaseProxy()->GetSetting(midasDatabaseProxy::LAST_URL).c_str() );

  std::vector<std::string> profiles = parent->getDatabaseProxy()->GetAuthProfiles();
  
  for(std::vector<std::string>::iterator i = profiles.begin(); i != profiles.end(); ++i)
    {
    profileComboBox->addItem(i->c_str());
    }
  parent->getDatabaseProxy()->Close();
}

/** */
int SignInUI::exec()
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

/** */
void SignInUI::accept()
{
  mws::WebAPI::Instance()->SetServerUrl(serverEdit->text().toStdString().c_str());
  if(mws::WebAPI::Instance()->CheckConnection())
    {
    parent->setServerURL(serverEdit->text().toStdString());
    parent->getDatabaseProxy()->Open();
    parent->getDatabaseProxy()->SetSetting(midasDatabaseProxy::LAST_URL, serverEdit->text().toStdString());
    parent->getAuthenticator()->SetProfile(profileComboBox->currentText().toStdString());
    parent->getDatabaseProxy()->Close();
    parent->getSynchronizer()->GetAuthenticator()->SetProfile(profileComboBox->currentText().toStdString());
    parent->getLog()->Message("Signed in successfully.");

    emit signedIn(); 
    }
  else
    {
    parent->getLog()->Error("The URL provided is not a valid MIDAS server Web API.");
    }
  QDialog::accept();
}

/** */
void SignInUI::editServerSettings()
{
  mws::Settings::GetInstance()->Write();
  this->parent->editServerSettings(); 
  serverEdit->setText(  mws::Settings::GetInstance()->GetServerURL().c_str() );
}

void SignInUI::profileCreated(std::string name, std::string url)
{
  profileComboBox->addItem(name.c_str());
  serverEdit->setText(url.c_str());
}

void SignInUI::showCreateProfileDialog()
{
  emit createProfileRequest();
}
