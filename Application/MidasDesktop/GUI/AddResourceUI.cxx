#include "AddResourceUI.h"
#include "MIDASDesktopUI.h"
#include "midasSynchronizer.h"

#include <QFileDialog>
#include <QString>

AddResourceUI::AddResourceUI(DionysusUI *parent):
  QDialog(parent), parent(parent)
{
  setupUi(this);
  connect(chooseFile_Button, SIGNAL(released()), this, SLOT(chooseFile()));
  connect(localParentRadioButton, SIGNAL( released() ), 
    this, SLOT( handleRadioSelection()));
  connect(serverParentRadioButton, SIGNAL( released() ), 
    this, SLOT( handleRadioSelection()));
}

void AddResourceUI::init()
{
  this->pathEdit->setText("");
  this->resourceTypeComboBox->setCurrentIndex(0);
}

void AddResourceUI::setParentId(std::string id)
{
  this->parentIdEdit->setText(id.c_str());
}

int AddResourceUI::exec()
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

void AddResourceUI::accept()
{
  QDialog::accept();
  parent->displayStatus(tr("Adding resource..."));
  parent->getSynchronizer()->SetOperation(midasSynchronizer::OPERATION_ADD);
  parent->getSynchronizer()->SetResourceHandle(pathEdit->text().toStdString());

  if(serverParentRadioButton->isChecked())
    {
    parent->getSynchronizer()->SetParentId(
      atoi(parentIdEdit->text().toStdString().c_str()));
    }
  else
    {
    parent->getSynchronizer()->SetParentId(0);
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

  if(parent->getSynchronizer()->Perform())
    {
    parent->displayStatus(tr("Failed to add resource"));
    }
  else
    {
    parent->displayStatus(tr("Successfully added resource"));
    }
  emit addedResource();
}

void AddResourceUI::chooseFile()
{
  QString file;
  std::string text = resourceTypeComboBox->currentText().toStdString();
  if(resourceTypeComboBox->currentText().toStdString() == "Bitstream")
    {
    file = QFileDialog::getOpenFileName(
                                    this, tr("Choose file to add"),
                                    QDir::current().absolutePath(),
                                    tr("All Files (*)"));
    }
  else
    {
    file = QFileDialog::getExistingDirectory(
      this, tr("Choose directory to add"), QDir::current().absolutePath(),
      QFileDialog::ShowDirsOnly);
    }
  
  
  this->pathEdit->setText(file);
}

void AddResourceUI::handleRadioSelection()
{
  this->parentIdEdit->setEnabled(serverParentRadioButton->isChecked());
}
