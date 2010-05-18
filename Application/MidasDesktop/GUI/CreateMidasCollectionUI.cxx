#include "CreateMidasCollectionUI.h"

#include <QtGui> 
#include <Qt>

#include "MIDASDesktopUI.h"
#include "mwsSettings.h"
#include "midasSynchronizer.h"

#include "MidasTreeItem.h"
#include "MidasTreeModel.h"
#include "MidasTreeModelClient.h"
#include "MidasCommunityTreeItem.h"

#include <kwsys/SystemTools.hxx>

CreateMidasCollectionUI::CreateMidasCollectionUI(MIDASDesktopUI *parent, CreateMidasCollectionUI::Types type): 
  QDialog(parent), parent(parent)
{
  setupUi(this); // this sets up GUI
  this->setType(type);
}

void CreateMidasCollectionUI::setType(Types type)
{
  this->type = type;
  if (type == CreateMidasCollectionUI::ClientCollection)
    {
    QWidget::setWindowTitle(tr("Add collection")); 
    }
}

void CreateMidasCollectionUI::reset()
{
  this->nameEdit->clear(); 
  this->short_descriptionEdit->clear(); 
  this->linksEdit->clear(); 
  this->createDefaultGroupCheckBox->setChecked(false); 
}

int CreateMidasCollectionUI::exec()
{
  this->reset(); 
  return QDialog::exec(); 
}

void CreateMidasCollectionUI::accept()
{
  if(this->nameEdit->text() == "")
    {
    parent->getLog()->Error("You must supply a name for the collection.");
    QDialog::accept();
    return;
    }

  QModelIndex selected = QModelIndex();
  if(this->type == CreateMidasCollectionUI::ClientCollection)
    {
    std::string path = kwsys::SystemTools::GetCurrentWorkingDirectory();
    selected = this->parent->getTreeViewClient()->getSelectedModelIndex();
    MidasCommunityTreeItem* parentComm =
      reinterpret_cast<MidasCommunityTreeItem*>(
      const_cast<MidasTreeItem*>(
      reinterpret_cast<MidasTreeModelClient*>(
      this->parent->getTreeViewClient()->model())->midasTreeItem(selected)));

    parent->getDatabaseProxy()->Open();
    path = parent->getDatabaseProxy()->GetRecordByUuid(
      parent->getDatabaseProxy()->GetUuid(
      midasResourceType::COMMUNITY,
      parentComm->getCommunity()->GetId())).Path;
    parent->getDatabaseProxy()->Close();
    
    path += "/" + nameEdit->text().toStdString();
    kwsys::SystemTools::MakeDirectory(path.c_str());

    parent->getSynchronizer()->SetOperation(midasSynchronizer::OPERATION_ADD);
    parent->getSynchronizer()->SetResourceType(midasResourceType::COLLECTION);
    parent->getSynchronizer()->SetResourceHandle(path);
    
    std::stringstream text;
    if(parent->getSynchronizer()->Perform() == 0)
      {    
      text << "Successfully added collection " << path << ".";
      parent->getLog()->Message(text.str());
      parent->clientTreeViewUpdated();
      }
    else
      {
      text << "Failed to add collection at " << path << ".";
      parent->getLog()->Error(text.str());
      }
    }
  else //create collection on the server
    {
    selected = this->parent->getTreeView()->getSelectedModelIndex(); 

    reinterpret_cast<MidasTreeModel*>(
      this->parent->getTreeView()->model())->insertMidasCollection(
      selected, this->nameEdit->text().toStdString(), 
      this->short_descriptionEdit->text().toStdString(), 
      this->linksEdit->text().toStdString(), 
      this->createDefaultGroupCheckBox->isChecked());
    }
  QDialog::accept();
}
