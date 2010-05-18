#include "SignInUI.h"
#include "MidasClientGlobal.h"
#include "mwsSettings.h"
#include "MIDASDesktopUI.h"
#include "mwsWebAPI.h"
#include "midasAuthenticator.h"
#include "midasDatabaseProxy.h"
#include "midasSynchronizer.h"

/** Constructor */
SignInUI::SignInUI(MIDASDesktopUI *parent):
  QDialog(parent), parent(parent)
{
  setupUi(this);
  connect( createProfileButton, SIGNAL( released() ), this, SLOT( showCreateProfileDialog() ) );
}

/** */
void SignInUI::init()
{
  profileComboBox->clear();
  parent->getDatabaseProxy()->Open();
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
  parent->getDatabaseProxy()->Open();
  std::string url = parent->getDatabaseProxy()->GetAuthProfile(
    profileComboBox->currentText().toStdString()).Url;
  parent->getDatabaseProxy()->Close();

  mws::WebAPI::Instance()->SetServerUrl(url.c_str());
  if(mws::WebAPI::Instance()->CheckConnection())
    {
    parent->setServerURL(url);
    parent->getDatabaseProxy()->Open();
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

void SignInUI::profileCreated(std::string name)
{
  init();
  profileComboBox->setCurrentIndex(profileComboBox->findText(name.c_str()));
}

void SignInUI::showCreateProfileDialog()
{
  emit createProfileRequest();
}
