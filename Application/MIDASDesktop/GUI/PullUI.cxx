#include "PullUI.h"
#include "SynchronizerThread.h"
#include "MidasClientGlobal.h"
#include "MIDASDesktopUI.h"
#include "midasAuthenticator.h"
#include "midasDatabaseProxy.h"
#include "midasSynchronizer.h"

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
  delete m_SynchronizerThread;
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
  m_SynchronizerThread->SetSynchronizer(m_Parent->getSynchronizer());

  if(cloneRadioButton->isChecked())
    {
    m_Parent->getSynchronizer()->SetOperation(midasSynchronizer::OPERATION_CLONE);
    m_Parent->getSynchronizer()->SetRecursive(true);
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
        m_Parent->getSynchronizer()->SetResourceType(midasResourceType::COMMUNITY);
        m_TypeName = "Community";
        break;
      case midasResourceType::COLLECTION:
        m_Parent->getSynchronizer()->SetResourceType(midasResourceType::COLLECTION);
        m_TypeName = "Collection";
        break;
      case midasResourceType::ITEM:
        m_Parent->getSynchronizer()->SetResourceType(midasResourceType::ITEM);
        m_TypeName = "Item";
        break;
      case midasResourceType::BITSTREAM:
        m_Parent->getSynchronizer()->SetResourceType(midasResourceType::BITSTREAM);
        m_TypeName = "Bitstream";
        break;
      default:
        break;
      }
    std::stringstream idStr;
    idStr << m_PullId;
    m_Parent->getSynchronizer()->SetServerHandle(idStr.str());
    m_Parent->getSynchronizer()->SetOperation(midasSynchronizer::OPERATION_PULL);
    m_Parent->getSynchronizer()->SetRecursive(recursiveCheckBox->isChecked());

    m_Parent->setProgressIndeterminate();

    connect(m_SynchronizerThread, SIGNAL( performReturned(int) ), this, SLOT ( pulled(int) ) );
    }
  connect(m_SynchronizerThread, SIGNAL( threadComplete() ), m_Parent, SLOT( setProgressEmpty() ) );
  connect(m_SynchronizerThread, SIGNAL( enableActions(bool) ), m_Parent, SLOT( enableActions(bool) ) );

  m_SynchronizerThread->start();

  QDialog::accept();
}

void PullUI::pulled(int rc)
{
  std::stringstream text;
  if(rc == MIDAS_OK)
    {
    text << "Successfully pulled " << m_TypeName << " with id=" << m_PullId;
    m_Parent->GetLog()->Message(text.str());
    }
  else if(rc == MIDAS_CANCELED)
    {
    text << "Pull canceled by user";
    m_Parent->GetLog()->Message(text.str());
    }
  else
    {
    text << "Failed to pull " << m_TypeName << " with id=" << m_PullId;
    m_Parent->GetLog()->Error(text.str());
    }
  emit pulledResources();
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
  m_Parent->displayStatus(text.c_str());
  m_Parent->setProgressEmpty();
}
