#include "SignInUI.h"
#include "MIDASDesktopUI.h"
#include "MidasClientGlobal.h"
#include "SignInThread.h"

#include "mwsWebAPI.h"
#include "midasAuthenticator.h"
#include "mdsDatabaseAPI.h"
#include "midasSynchronizer.h"

#include <QMessageBox>

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

  mds::DatabaseAPI db;
  std::vector<std::string> profiles = db.GetAuthProfiles();
  
  for(std::vector<std::string>::iterator i = profiles.begin(); i != profiles.end(); ++i)
    {
    profileComboBox->addItem(i->c_str());
    }
}

/** */
int SignInUI::exec()
{
  if(mds::DatabaseInfo::Instance()->GetPath() != "")
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
  if(this->profileComboBox->currentText().toStdString() == "")
    {
    QMessageBox::critical(this, "Error", "Please select a profile or create a new one");
    return;
    }

  if(m_SignInThread)
    {
    disconnect(m_SignInThread);
    }
  delete m_SignInThread;
  m_SignInThread = new SignInThread(parent);
  m_SignInThread->SetProfile(profileComboBox->currentText());

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

void SignInUI::removeProfile(std::string name)
{
  init();
}
