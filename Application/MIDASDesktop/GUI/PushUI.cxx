#include "PushUI.h"
#include "SynchronizerThread.h"
#include "midasSynchronizer.h"
#include "mdoObject.h"

/** Constructor */
PushUI::PushUI(QWidget* parent, midasSynchronizer* synch)
: QDialog(parent), m_Synch(synch), m_Object(NULL), m_SynchThread(NULL)
{
  setupUi(this);
  resetState();

  connect( pushAllRadioButton, SIGNAL( released() ), this, SLOT( radioButtonChanged() ) );
  connect( selectedRadioButton, SIGNAL( released() ), this, SLOT( radioButtonChanged() ) );
}

PushUI::~PushUI()
{
  delete m_SynchThread;
}

void PushUI::setObject(mdo::Object* object)
{
  m_Object = object;
}

void PushUI::resetState()
{
  this->selectedRadioButton->setChecked(true);
  this->radioButtonChanged();
}

void PushUI::radioButtonChanged()
{
  bool val = selectedRadioButton->isChecked();
  recursiveCheckbox->setEnabled(val);
}

void PushUI::init()
{
  if(m_Object)
    {
    selectedRadioButton->setChecked(true);
    }
  else
    {
    pushAllRadioButton->setChecked(true);
    }
  this->radioButtonChanged();
}

int PushUI::exec()
{
  this->init();
  return QDialog::exec();
}

void PushUI::accept()
{
  delete m_SynchThread;
  m_SynchThread = new SynchronizerThread;
  m_SynchThread->SetSynchronizer(m_Synch);

  m_Synch->SetOperation(midasSynchronizer::OPERATION_PUSH);

  if(m_Object)
    {
    std::stringstream text;
    text << m_Object->GetId();

    m_Synch->SetClientHandle(text.str());
    m_Synch->SetServerHandle(m_Object->GetUuid());
    m_Synch->SetResourceType(m_Object->GetResourceType());
    m_Synch->SetRecursive(recursiveCheckbox->isChecked());
    }
  else
    {
    m_Synch->SetResourceType(midasResourceType::NONE);
    }

  connect(m_SynchThread, SIGNAL( performReturned(int) ),
          this, SIGNAL( pushedResources(int) ) );
  connect(m_SynchThread, SIGNAL( enableActions(bool) ),
          this, SIGNAL( enableActions(bool) ) );

  m_SynchThread->start();

  QDialog::accept();
}
