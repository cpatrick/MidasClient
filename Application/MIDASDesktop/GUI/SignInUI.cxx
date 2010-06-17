#include "SignInUI.h"
#include "MIDASDesktopUI.h"
#include "MidasClientGlobal.h"
#include "SignInThread.h"

#include "mwsSettings.h"
#include "mwsWebAPI.h"
#include "midasAuthenticator.h"
#include "midasDatabaseProxy.h"
#include "midasSynchronizer.h"

/** Constructor */
SignInUI::SignInUI(MIDASDesktopUI* parent):
  QDialog(parent), parent(parent)
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
  if(m_SignInThread)
    {
    disconnect(m_SignInThread);
    }
  delete m_SignInThread;
  m_SignInThread = new SignInThread;
  m_SignInThread->SetProfile(profileComboBox->currentText());
  m_SignInThread->SetParentUI(parent);

  connect(m_SignInThread, SIGNAL( initialized(bool) ), parent, SLOT( signIn(bool) ) );

  parent->displayStatus(tr("Connecting to server..."));
  parent->setProgressIndeterminate();

  m_SignInThread->start();
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
