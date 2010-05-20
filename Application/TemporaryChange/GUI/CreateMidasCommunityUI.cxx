#include "CreateMidasCommunityUI.h"

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

CreateMidasCommunityUI::CreateMidasCommunityUI(MIDASDesktopUI *parent, CreateMidasCommunityUI::Types type):
  QDialog(parent), parent(parent)
  {
  setupUi(this); // this sets up GUI
  this->setType(type); 
  }

void CreateMidasCommunityUI::setType(Types type)
  {
  this->type = type;
  if (type == CreateMidasCommunityUI::SubCommunity)
    {
    QWidget::setWindowTitle(tr("Create Sub-community")); 
    }
  else if(type == CreateMidasCommunityUI::ClientCommunity)
    {
    QWidget::setWindowTitle(tr("Add Community"));
    }
  else if(type == CreateMidasCommunityUI::ClientSubCommunity)
    {
    QWidget::setWindowTitle(tr("Add Sub-community"));
    }
  }

void CreateMidasCommunityUI::reset()
  {
  this->nameEdit->clear(); 
  this->short_descriptionEdit->clear(); 
  this->linksEdit->clear(); 
  }

int CreateMidasCommunityUI::exec()
  {
  this->reset(); 
  return QDialog::exec(); 
  }

void CreateMidasCommunityUI::accept()
  {
  if(this->nameEdit->text() == "")
    {
    parent->getLog()->Error("You must supply a name for the community.");
    QDialog::accept();
    return;
    }

  QModelIndex selected = QModelIndex();
  if (this->type >= CreateMidasCommunityUI::ClientCommunity)
    {
    std::string path = kwsys::SystemTools::GetCurrentWorkingDirectory();
    if (this->type == CreateMidasCommunityUI::ClientSubCommunity)
      {
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
      }
    
    path += "/" + nameEdit->text().toStdString();
    kwsys::SystemTools::MakeDirectory(path.c_str());

    parent->getSynchronizer()->SetOperation(midasSynchronizer::OPERATION_ADD);
    parent->getSynchronizer()->SetResourceType(midasResourceType::COMMUNITY);
    parent->getSynchronizer()->SetResourceHandle(path);
    
    std::stringstream text;
    if(parent->getSynchronizer()->Perform() == 0)
      {    
      text << "Successfully added community " << path << ".";
      parent->getLog()->Message(text.str());
      parent->clientTreeViewUpdated();
      }
    else
      {
      text << "Failed to add community at " << path << ".";
      parent->getLog()->Error(text.str());
      }
    }
  else //creating a server-side community
    {
    if (this->type == CreateMidasCommunityUI::SubCommunity)
      {
      selected = this->parent->getTreeView()->getSelectedModelIndex(); 
      }

    reinterpret_cast<MidasTreeModel*>(
      this->parent->getTreeView()->model())->insertMidasCommunity(selected, 
      this->nameEdit->text().toStdString(),
      this->short_descriptionEdit->text().toStdString(),
      this->linksEdit->text().toStdString());
    }
  QDialog::accept();
  }
