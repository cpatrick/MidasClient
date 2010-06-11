#include "PullUI.h"
#include "PullThread.h"
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

  m_PullThread = NULL;
  m_TypeName = "Resource";

  connect( cloneRadioButton, SIGNAL( released() ), this, SLOT( radioButtonChanged() ) );
  connect( pullRadioButton, SIGNAL( released() ), this, SLOT( radioButtonChanged() ) );
}

PullUI::~PullUI()
{
  delete m_PullThread;
}

void PullUI::resetState()
{
  this->m_PullId = 0;
  this->m_ResourceTypeIndex = 0;
  this->cloneRadioButton->setChecked(true);
  emit radioButtonChanged();
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

void PullUI::setResourceTypeIndex(int index)
{
  m_ResourceTypeIndex = index;
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
  if(m_PullThread)
    {
    disconnect(m_PullThread);
    }
  delete m_PullThread;

  m_PullThread = new PullThread;
  m_PullThread->SetParentUI(m_Parent);
  
  if(cloneRadioButton->isChecked())
    {
    m_Parent->getSynchronizer()->SetOperation(midasSynchronizer::OPERATION_CLONE);
    m_Parent->getSynchronizer()->SetRecursive(true);
    m_Parent->getLog()->Message("Cloning the server repository");
    m_Parent->displayStatus(tr("Cloning the server respository"));

    m_Parent->setProgressIndeterminate();
    m_Parent->displayStatus("Cloning MIDAS repository...");

    connect(m_PullThread, SIGNAL( performReturned(bool) ), this, SLOT ( cloned(bool) ) );
    }
  else //pull
    {
    switch(m_ResourceTypeIndex)
      {
      case 0: //community
        m_Parent->getSynchronizer()->SetResourceType(midasResourceType::COMMUNITY);
        m_TypeName = "Community";
        break;
      case 1: //collection
        m_Parent->getSynchronizer()->SetResourceType(midasResourceType::COLLECTION);
        m_TypeName = "Collection";
        break;
      case 2: //item
        m_Parent->getSynchronizer()->SetResourceType(midasResourceType::ITEM);
        m_TypeName = "Item";
        break;
      case 3: //bitstream
        m_Parent->getSynchronizer()->SetResourceType(midasResourceType::BITSTREAM);
        m_TypeName = "Bitstream";
        break;
      default:
        break;
      }
    std::stringstream idStr;
    idStr << m_PullId;
    m_Parent->getSynchronizer()->SetResourceHandle(idStr.str());
    m_Parent->getSynchronizer()->SetOperation(midasSynchronizer::OPERATION_PULL);
    m_Parent->getSynchronizer()->SetRecursive(recursiveCheckBox->isChecked());

    m_Parent->setProgressIndeterminate();

    connect(m_PullThread, SIGNAL( performReturned(bool) ), this, SLOT ( pulled(bool) ) );
    }
  connect(m_PullThread, SIGNAL( threadComplete() ), m_Parent, SLOT( setProgressEmpty() ) );
  connect(m_PullThread, SIGNAL( enablePull(bool) ), m_Parent, SLOT( enablePull(bool) ) );

  m_PullThread->start();

  QDialog::accept();
}

void PullUI::pulled(bool ok)
{
  std::stringstream text;
  if(!ok)
    {
    text << "Failed to pull " << m_TypeName << " with id=" << m_PullId;
    m_Parent->getLog()->Error(text.str());
    }
  else
    {
    emit pulledResources();

    text << "Successfully pulled " << m_TypeName << " with id=" << m_PullId;
    m_Parent->getLog()->Message(text.str());
    }
  m_Parent->displayStatus(text.str().c_str());
}

void PullUI::cloned(bool ok)
{
  std::string text;
  if(!ok)
    {
    text = "Failed to clone the MIDAS repository.";
    m_Parent->getLog()->Error(text);
    }
  else
    {
    emit pulledResources();
    text = "Successfully cloned the MIDAS repository.";
    m_Parent->getLog()->Message(text);
    }
  m_Parent->displayStatus(text.c_str());
}
