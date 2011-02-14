#include "PullUI.h"
#include "SynchronizerThread.h"
#include "MidasClientGlobal.h"
#include "MIDASDesktopUI.h"
#include "midasAuthenticator.h"
#include "midasDatabaseProxy.h"
#include "midasSynchronizer.h"
#include "PollFilesystemThread.h"

/** Constructor */
PullUI::PullUI(MIDASDesktopUI *parent):
  QDialog(parent), m_Parent(parent)
{
  setupUi(this);
  resetState();

  m_SynchronizerThread = NULL;
  m_TypeName = "Resource";

  connect( cloneRadioButton, SIGNAL( released() ), this, SLOT( radioButtonChanged() ) );
  connect( pullRadioButton, SIGNAL( released() ), this, SLOT( radioButtonChanged() ) );
}

PullUI::~PullUI()
{
  if(m_SynchronizerThread && m_SynchronizerThread->isRunning())
    {
    m_SynchronizerThread->Cancel();
    m_SynchronizerThread->wait();
    }
  delete m_SynchronizerThread;
}

SynchronizerThread* PullUI::getSynchronizerThread()
{
  return m_SynchronizerThread;
}

void PullUI::resetState()
{
  this->m_PullId = 0;
  this->m_ResourceType = 0;
  this->cloneRadioButton->setChecked(true);
  emit radioButtonChanged();
}

void PullUI::setClone()
{
  cloneRadioButton->setChecked(true);
  pullRadioButton->setChecked(false);
}

void PullUI::setPull()
{
  cloneRadioButton->setChecked(false);
  pullRadioButton->setChecked(true);
}

void PullUI::setRecursive(bool value)
{
  recursiveCheckBox->setChecked(value);
}

void PullUI::radioButtonChanged()
{
  bool val = pullRadioButton->isChecked();
  recursiveCheckBox->setEnabled(val);
}

void PullUI::setPullId(int id)
{
  m_PullId = id;
}

void PullUI::setResourceType(int type)
{
  m_ResourceType = type;
}

void PullUI::setResourceName(std::string name)
{
  m_Name = name;
}

/** */
void PullUI::init()
{
  if(m_PullId)
    {
    pullRadioButton->setChecked(true);
    }
  else
    {
    cloneRadioButton->setChecked(true);
    }
  emit radioButtonChanged();
}

/** */
int PullUI::exec()
{
  this->init();
  return QDialog::exec();
}

/** */
void PullUI::accept()
{
  if(m_SynchronizerThread)
    {
    disconnect(m_SynchronizerThread);
    }
  delete m_SynchronizerThread;

  m_SynchronizerThread = new SynchronizerThread;
  midasSynchronizer* synchronizer = new midasSynchronizer;
  synchronizer->SetDatabase(m_Parent->getSynchronizer()->GetDatabase()->GetDatabasePath());
  synchronizer->SetLog(m_Parent->getSynchronizer()->GetLog());
  synchronizer->SetProgressReporter(m_Parent->getProgress());
  synchronizer->SetServerURL(m_Parent->getSynchronizer()->GetServerURL());
  synchronizer->SetAgreementHandler(m_Parent->getAgreementHandler());
  synchronizer->SetResourceUpdateHandler(m_Parent->getResourceUpdateHandler());
  synchronizer->SetOverwriteHandler(m_Parent->getFileOverwriteHandler());
  //hack-ish: make sure to set the authenticator back to null so it doesn't delete the top level auth when it goes out of scope
  synchronizer->SetAuthenticator(m_Parent->getSynchronizer()->GetAuthenticator(), true);
  m_SynchronizerThread->SetSynchronizer(synchronizer);
  m_SynchronizerThread->SetDelete(true);

  if(cloneRadioButton->isChecked())
    {
    synchronizer->SetOperation(midasSynchronizer::OPERATION_CLONE);
    synchronizer->SetRecursive(true);
    m_Parent->GetLog()->Message("Cloning the server repository");
    m_Parent->displayStatus(tr("Cloning the server respository"));

    m_Parent->setProgressIndeterminate();
    m_Parent->displayStatus("Cloning MIDAS repository...");

    connect(m_SynchronizerThread, SIGNAL( performReturned(int) ), this, SLOT ( cloned(int) ) );
    }
  else //pull
    {
    switch(m_ResourceType)
      {
      case midasResourceType::COMMUNITY:
        synchronizer->SetResourceType(midasResourceType::COMMUNITY);
        m_TypeName = "Community";
        break;
      case midasResourceType::COLLECTION:
        synchronizer->SetResourceType(midasResourceType::COLLECTION);
        m_TypeName = "Collection";
        break;
      case midasResourceType::ITEM:
        synchronizer->SetResourceType(midasResourceType::ITEM);
        m_TypeName = "Item";
        break;
      case midasResourceType::BITSTREAM:
        synchronizer->SetResourceType(midasResourceType::BITSTREAM);
        m_TypeName = "Bitstream";
        break;
      default:
        break;
      }
    std::stringstream idStr;
    idStr << m_PullId;
    synchronizer->SetServerHandle(idStr.str());
    synchronizer->SetOperation(midasSynchronizer::OPERATION_PULL);
    synchronizer->SetRecursive(recursiveCheckBox->isChecked());

    m_Parent->setProgressIndeterminate();

    connect(m_SynchronizerThread, SIGNAL( performReturned(int) ), this, SLOT ( pulled(int) ) );
    }
  connect(m_SynchronizerThread, SIGNAL( threadComplete() ), m_Parent, SLOT( setProgressEmpty() ) );
  connect(m_SynchronizerThread, SIGNAL( enableActions(bool) ), m_Parent, SLOT( enableActions(bool) ) );

  m_Parent->getPollFilesystemThread()->Pause();
  m_SynchronizerThread->start();

  QDialog::accept();
}

void PullUI::pulled(int rc)
{
  std::stringstream text;
  if(rc == MIDAS_OK)
    {
    text << "Successfully pulled " << m_TypeName << ": " << m_Name;
    m_Parent->GetLog()->Message(text.str());
    }
  else if(rc == MIDAS_CANCELED)
    {
    text << "Pull canceled by user";
    m_Parent->GetLog()->Message(text.str());
    }
  else
    {
    text << "Failed to pull " << m_TypeName << ": " << m_Name;
    m_Parent->GetLog()->Error(text.str());
    }
  emit pulledResources();
  m_Parent->getPollFilesystemThread()->Resume();
  m_Parent->displayStatus(text.str().c_str());
  m_Parent->setProgressEmpty();
}

void PullUI::cloned(int rc)
{
  std::string text;
  if(rc == MIDAS_OK)
    {
    text = "Successfully cloned the MIDAS repository.";
    m_Parent->GetLog()->Message(text);
    }
  else if(rc == MIDAS_CANCELED)
    {
    text = "Clone canceled by user";
    m_Parent->GetLog()->Message(text);
    }
  else
    {
    text = "Failed to clone the MIDAS repository.";
    m_Parent->GetLog()->Error(text);
    }
  emit pulledResources();
  m_Parent->getPollFilesystemThread()->Resume();
  m_Parent->displayStatus(text.c_str());
  m_Parent->setProgressEmpty();
}
