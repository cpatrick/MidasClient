#include "PullUI.h"
#include "MidasClientGlobal.h"
#include "MIDASDesktopUI.h"
#include "midasAuthenticator.h"
#include "midasDatabaseProxy.h"
#include "midasSynchronizer.h"

/** Constructor */
PullUI::PullUI(MIDASDesktopUI *parent):
  QDialog(parent), parent(parent)
{
  setupUi(this);
  resetState();

  connect( cloneRadioButton, SIGNAL( released() ), this, SLOT( radioButtonChanged() ) );
  connect( pullRadioButton, SIGNAL( released() ), this, SLOT( radioButtonChanged() ) );
}

void PullUI::resetState()
{
  this->pullId = 0;
  this->resourceTypeIndex = 0;
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
  this->pullId = id;
}

void PullUI::setResourceTypeIndex(int index)
{
  this->resourceTypeIndex = index;
}

/** */
void PullUI::init()
{
  if(this->pullId)
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
  QDialog::accept();
  if(cloneRadioButton->isChecked())
    {
    parent->getSynchronizer()->SetOperation(midasSynchronizer::OPERATION_CLONE);
    parent->getSynchronizer()->SetRecursive(true);
    parent->getLog()->Message("Cloning the server repository");
    parent->displayStatus(tr("Cloning the server respository"));

    parent->setProgressIndeterminate();
    parent->displayStatus("Cloning MIDAS repository...");
    if(parent->getSynchronizer()->Perform())
      {
      std::string text = "Failed to clone the MIDAS repository.";
      parent->getLog()->Error(text);
      parent->displayStatus(text.c_str());
      }
    else
      {
      emit pulledResources();
      std::string text = "Successfully cloned the MIDAS repository.";
      parent->getLog()->Message(text);
      parent->displayStatus(text.c_str());
      }
    parent->setProgressEmpty();
    }
  else //pull
    {
    std::string typeName;
    switch(this->resourceTypeIndex)
      {
      case 0: //community
        parent->getSynchronizer()->SetResourceType(midasResourceType::COMMUNITY);
        typeName = "Community";
        break;
      case 1: //collection
        parent->getSynchronizer()->SetResourceType(midasResourceType::COLLECTION);
        typeName = "Collection";
        break;
      case 2: //item
        parent->getSynchronizer()->SetResourceType(midasResourceType::ITEM);
        typeName = "Item";
        break;
      case 3: //bitstream
        parent->getSynchronizer()->SetResourceType(midasResourceType::BITSTREAM);
        typeName = "Bitstream";
        break;
      default:
        break;
      }
    std::stringstream idStr;
    idStr << pullId;
    parent->getSynchronizer()->SetResourceHandle(idStr.str());
    parent->getSynchronizer()->SetOperation(midasSynchronizer::OPERATION_PULL);
    parent->getSynchronizer()->SetRecursive(recursiveCheckBox->isChecked());

    parent->setProgressIndeterminate();
    std::stringstream text;
    if(parent->getSynchronizer()->Perform())
      {
      text << "Failed to pull " << typeName << " with id=" << this->pullId;
      parent->getLog()->Error(text.str());
      }
    else
      {
      emit pulledResources();
      
      text << "Successfully pulled " << typeName << " with id=" << this->pullId;
      parent->getLog()->Message(text.str());
      }
    parent->setProgressEmpty();
    parent->displayStatus(text.str().c_str());
    }
}
