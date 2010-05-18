#include "PullUI.h"
#include "MidasClientGlobal.h"
#include "MIDASDesktopUI.h"
#include "midasAuthenticator.h"
#include "midasDatabaseProxy.h"
#include "midasSynchronizer.h"

/** Constructor */
PullUI::PullUI(DionysusUI *parent):
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
  
  idEdit->setEnabled(val);
  idLabel->setEnabled(val);

  resourceTypeComboBox->setEnabled(val);
  resourceTypeLabel->setEnabled(val);
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
    std::stringstream str;
    str << this->pullId;
    idEdit->setText(str.str().c_str());
    }
  else
    {
    cloneRadioButton->setChecked(true);
    idEdit->setText("");
    }
  resourceTypeComboBox->setCurrentIndex(this->resourceTypeIndex);
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
    }
  else //pull
    {
    if(idEdit->text().toStdString() == "")
      {
      parent->getLog()->Error("You must specify the server-side ID of the resource to pull.");
      return;
      }

    switch(resourceTypeComboBox->currentIndex())
      {
      case 0: //community
        parent->getSynchronizer()->SetResourceType(midasResourceType::COMMUNITY);
        break;
      case 1: //collection
        parent->getSynchronizer()->SetResourceType(midasResourceType::COLLECTION);
        break;
      case 2: //item
        parent->getSynchronizer()->SetResourceType(midasResourceType::ITEM);
        break;
      case 3: //bitstream
        parent->getSynchronizer()->SetResourceType(midasResourceType::BITSTREAM);
        break;
      default:
        break;
      }
    parent->getSynchronizer()->SetResourceHandle(idEdit->text().toStdString());
    parent->getSynchronizer()->SetOperation(midasSynchronizer::OPERATION_PULL);
    parent->getSynchronizer()->SetRecursive(recursiveCheckBox->isChecked());
    }

  if(parent->getSynchronizer()->Perform())
    {
    parent->getLog()->Error("Operation failed");
    }
  else
    {
    emit pulledResources();
    parent->getLog()->Message("Operation complete");
    }
  parent->displayStatus(tr(""));
}
