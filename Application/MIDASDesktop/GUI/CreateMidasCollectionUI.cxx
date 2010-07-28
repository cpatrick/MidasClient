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
    parent->GetLog()->Error("You must supply a name for the collection.");
    QDialog::accept();
    return;
    }

  if(this->type == CreateMidasCollectionUI::ClientCollection)
    {
    QModelIndex selected = this->parent->getTreeViewClient()->getSelectedModelIndex();
    MidasCommunityTreeItem* parentComm =
      reinterpret_cast<MidasCommunityTreeItem*>(
      const_cast<MidasTreeItem*>(
      reinterpret_cast<MidasTreeModelClient*>(
      this->parent->getTreeViewClient()->model())->midasTreeItem(selected)));

    parent->getDatabaseProxy()->Open();
    std::string path = parent->getDatabaseProxy()->GetRecordByUuid(
      parent->getDatabaseProxy()->GetUuid(
      midasResourceType::COMMUNITY,
      parentComm->getCommunity()->GetId())).Path;
    parent->getDatabaseProxy()->Close();
    
    path += "/" + nameEdit->text().toStdString();
    kwsys::SystemTools::MakeDirectory(path.c_str());

    parent->getSynchronizer()->SetOperation(midasSynchronizer::OPERATION_ADD);
    parent->getSynchronizer()->SetResourceType(midasResourceType::COLLECTION);
    parent->getSynchronizer()->SetClientHandle(path);
    
    std::stringstream text;
    if(parent->getSynchronizer()->Perform() == 0)
      {    
      text << "Successfully added collection " << path << ".";
      parent->GetLog()->Message(text.str());
      parent->updateClientTreeView();
      }
    else
      {
      text << "Failed to add collection at " << path << ".";
      parent->GetLog()->Error(text.str());
      }
    }
  else //create collection on the server
    {
    //disabled for now
    }
  QDialog::accept();
}
