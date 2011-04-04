#include "CreateMidasResourceUI.h"

#include <QtGui> 
#include <Qt>
#include <QMessageBox>

#include "MIDASDesktopUI.h"
#include "mwsSettings.h"
#include "midasSynchronizer.h"
#include "mdsDatabaseAPI.h"
#include "midasAuthenticator.h"

#include "MidasTreeItem.h"
#include "MidasTreeModel.h"
#include "MidasTreeModelClient.h"
#include "MidasCommunityTreeItem.h"
#include "MidasCollectionTreeItem.h"

#include "mdoCommunity.h"
#include "mdoCollection.h"

#include "midasStandardIncludes.h"

CreateMidasResourceUI::CreateMidasResourceUI(MIDASDesktopUI *m_Parent):
  QDialog(m_Parent), m_Parent(m_Parent)
{
  setupUi(this); // this sets up GUI
}

void CreateMidasResourceUI::SetType(Types type)
{
  this->m_Type = type;

  switch(type)
    {
    case Community:
      QWidget::setWindowTitle(tr("Add Community"));
      break;
    case SubCommunity:
      QWidget::setWindowTitle(tr("Add Subcommunity"));
      break;
    case Collection:
      QWidget::setWindowTitle(tr("Add Collection"));
      break;
    case Item:
      QWidget::setWindowTitle(tr("Add Item"));
      break;
    }
}

void CreateMidasResourceUI::reset()
{
  this->nameEdit->clear();
}

int CreateMidasResourceUI::exec()
{
  this->reset();
  return QDialog::exec();
}

void CreateMidasResourceUI::accept()
{
  this->nameEdit->setText(this->nameEdit->text().trimmed());

  if(!this->ValidateName())
    {
    return;
    }

  m_Parent->getSynchronizer()->SetOperation(midasSynchronizer::OPERATION_ADD);

  std::string type_str, path;
  switch(this->m_Type)
    {
    case Community:
      path = mds::DatabaseAPI::Instance()->GetAuthProfile(
        this->m_Parent->getSynchronizer()->GetAuthenticator()->GetProfile()).RootDir;

      if(path == "")
        {
        path = mds::DatabaseAPI::Instance()->GetSetting(
          mds::DatabaseAPI::ROOT_DIR);
        }

      if(path == "")
        {
        path = kwsys::SystemTools::GetCurrentWorkingDirectory();
        }
      this->AddCommunity(path);
      type_str = "community";
      break;
    case SubCommunity:
      this->AddSubCommunity();
      type_str = "subcommunity";
      break;
    case Collection:
      this->AddCollection();
      type_str = "collection";
      break;
    case Item:
      this->AddItem();
      type_str = "item";
      break;
    }

  std::stringstream text;
  if(m_Parent->getSynchronizer()->Perform() == 0)
    {
    text << "Successfully added " << type_str << ".";
    m_Parent->GetLog()->Message(text.str());
    m_Parent->updateClientTreeView();
    }
  else
    {
    text << "Failed to add " << type_str << ".";
    m_Parent->GetLog()->Error(text.str());
    }

  QDialog::accept();
}

bool CreateMidasResourceUI::ValidateName() const
{
  std::string name = this->nameEdit->text().toStdString();

  if(name == "")
    {
    QMessageBox::critical(m_Parent, "Error: Name required", "Name must not be empty");
    return false;
    }

  if(name.find(":") != name.npos ||
     name.find(";") != name.npos ||
     name.find("/") != name.npos ||
     name.find("*") != name.npos ||
     name.find("?") != name.npos ||
     name.find("|") != name.npos ||
     name.find("<") != name.npos ||
     name.find(">") != name.npos ||
     name.find("\"") != name.npos ||
     name.find("\\") != name.npos)
    {
    QMessageBox::critical(m_Parent, tr("Error: illegal characters"),
      tr("Name must not contain any of the following:<br /> <center>"
      "/ \\ \" : ; * ? | &lt; &gt;</center>"));
    return false;
    }
  return true;
}

void CreateMidasResourceUI::AddCommunity(std::string path)
{
  path += "/" + nameEdit->text().toStdString();
  kwsys::SystemTools::MakeDirectory(path.c_str());

  m_Parent->getSynchronizer()->SetResourceType(midasResourceType::COMMUNITY);
  m_Parent->getSynchronizer()->SetClientHandle(path);
}

void CreateMidasResourceUI::AddSubCommunity()
{
  QModelIndex selected =
    this->m_Parent->getTreeViewClient()->getSelectedModelIndex();

  MidasCommunityTreeItem* parentComm =
    reinterpret_cast<MidasCommunityTreeItem*>(
    const_cast<MidasTreeItem*>(
    reinterpret_cast<MidasTreeModelClient*>(
    this->m_Parent->getTreeViewClient()->model())->midasTreeItem(selected)));

  std::string path = mds::DatabaseAPI::Instance()->GetRecordByUuid(
    mds::DatabaseAPI::Instance()->GetUuid(
    midasResourceType::COMMUNITY,
    parentComm->getCommunity()->GetId())).Path;

  this->AddCommunity(path);
}

void CreateMidasResourceUI::AddCollection()
{
  QModelIndex selected = this->m_Parent->getTreeViewClient()->getSelectedModelIndex();
  MidasCommunityTreeItem* parentComm =
    reinterpret_cast<MidasCommunityTreeItem*>(
    const_cast<MidasTreeItem*>(
    reinterpret_cast<MidasTreeModelClient*>(
    this->m_Parent->getTreeViewClient()->model())->midasTreeItem(selected)));

  std::string path = mds::DatabaseAPI::Instance()->GetRecordByUuid(
    mds::DatabaseAPI::Instance()->GetUuid(
    midasResourceType::COMMUNITY,
    parentComm->getCommunity()->GetId())).Path;

  path += "/" + nameEdit->text().toStdString();
  kwsys::SystemTools::MakeDirectory(path.c_str());

  m_Parent->getSynchronizer()->SetResourceType(midasResourceType::COLLECTION);
  m_Parent->getSynchronizer()->SetClientHandle(path);
}

void CreateMidasResourceUI::AddItem()
{
  QModelIndex selected = this->m_Parent->getTreeViewClient()->getSelectedModelIndex();
  MidasCollectionTreeItem* parentColl =
    reinterpret_cast<MidasCollectionTreeItem*>(
    const_cast<MidasTreeItem*>(
    reinterpret_cast<MidasTreeModelClient*>(
    this->m_Parent->getTreeViewClient()->model())->midasTreeItem(selected)));

  std::string path = mds::DatabaseAPI::Instance()->GetRecordByUuid(
    mds::DatabaseAPI::Instance()->GetUuid(
    midasResourceType::COLLECTION,
    parentColl->getCollection()->GetId())).Path;

  path += "/" + nameEdit->text().toStdString();
  kwsys::SystemTools::MakeDirectory(path.c_str());

  m_Parent->getSynchronizer()->SetResourceType(midasResourceType::ITEM);
  m_Parent->getSynchronizer()->SetClientHandle(path);
}
