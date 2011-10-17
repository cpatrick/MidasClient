#include "CreateMidasResourceUI.h"

#include <QtGui> 
#include <Qt>
#include <QMessageBox>
#include <QDir>

#include "midasSynchronizer.h"
#include "mdsDatabaseAPI.h"
#include "midasAuthenticator.h"

#include "MidasTreeItem.h"
#include "Midas3TreeItem.h"
#include "MidasTreeModel.h"
#include "MidasTreeModelClient.h"
#include "MidasCommunityTreeItem.h"
#include "MidasCollectionTreeItem.h"
#include "Midas3FolderTreeItem.h"
#include "Midas3ItemTreeItem.h"

#include "mdoCommunity.h"
#include "mdoCollection.h"
#include "m3doFolder.h"

#include "midasStandardIncludes.h"

CreateMidasResourceUI::CreateMidasResourceUI(QWidget* parent,
                                             midasSynchronizer* synch)
: QDialog(parent), m_Synch(synch), m_ParentResource(NULL)
{
  setupUi(this); // this sets up GUI
}

CreateMidasResourceUI::~CreateMidasResourceUI()
{
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

void CreateMidasResourceUI::SetParentResource(const MidasTreeItem* parent)
{
  m_ParentResource = const_cast<MidasTreeItem*>(parent);
}

void CreateMidasResourceUI::SetParentResource3(const Midas3TreeItem* parent)
{
  m_ParentResource3 = const_cast<Midas3TreeItem*>(parent);
}

void CreateMidasResourceUI::reset()
{
  this->nameEdit->clear();
  this->nameEdit->setFocus();
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

  m_Synch->SetOperation(midasSynchronizer::OPERATION_ADD);

  mds::DatabaseAPI db;
  std::string type_str;
  std::string path = db.GetAuthProfile(
      m_Synch->GetAuthenticator()->GetProfile()).RootDir;

  if(path == "")
    {
    path = db.GetSetting(mds::DatabaseAPI::ROOT_DIR);
    }

  if(path == "")
    {
    path = QDir::currentPath().toStdString();
    }

  switch(this->m_Type)
    {
    case Community:
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
    case Community3:
      this->AddCommunity3(path);
      type_str = "community";
      break;
    case Folder:
      this->AddFolder(path);
      type_str = "top level folder";
      break;
    case SubFolder:
      this->AddSubFolder();
      type_str = "subfolder";
      break;
    case Item3:
      this->AddItem3();
      type_str = "item";
      break;
    default:
      break;
    }

  std::stringstream text;
  if(m_Synch->Perform() == 0)
    {
    text << "Successfully added " << type_str << ".";
    m_Synch->GetLog()->Message(text.str());
    emit resourceCreated();
    }
  else
    {
    text << "Failed to add " << type_str << ".";
    m_Synch->GetLog()->Error(text.str());
    }
  QDialog::accept();
}

bool CreateMidasResourceUI::ValidateName() const
{
  std::string name = this->nameEdit->text().toStdString();

  if(name == "")
    {
    QMessageBox::critical(NULL, "Error: Name required", "Name must not be empty");
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
    QMessageBox::critical(NULL, tr("Error: illegal characters"),
      tr("Name must not contain any of the following:<br /> <center>"
      "/ \\ \" : ; * ? | &lt; &gt;</center>"));
    return false;
    }
  return true;
}

void CreateMidasResourceUI::AddCommunity(std::string path)
{
  QDir dir(path.c_str());
  dir.mkpath(nameEdit->text());

  path += "/" + nameEdit->text().toStdString();

  m_Synch->SetResourceType(midasResourceType::COMMUNITY);
  m_Synch->SetClientHandle(path);
}

void CreateMidasResourceUI::AddSubCommunity()
{
  MidasCommunityTreeItem* parentComm =
    dynamic_cast<MidasCommunityTreeItem*>(m_ParentResource);

  if(!parentComm)
    {
    m_Synch->GetLog()->Error("CreateMidasResourceUI::AddSubCommunity - no parent resource");
    return;
    }

  mds::DatabaseAPI db;
  std::string path = db.GetRecordByUuid(parentComm->getUuid()).Path;

  this->AddCommunity(path);
}

void CreateMidasResourceUI::AddCollection()
{
  MidasCommunityTreeItem* parentComm =
    dynamic_cast<MidasCommunityTreeItem*>(m_ParentResource);

  if(!parentComm)
    {
    m_Synch->GetLog()->Error("CreateMidasResourceUI::AddCollection - no parent resource");
    return;
    }

  mds::DatabaseAPI db;
  std::string path = db.GetRecordByUuid(parentComm->getUuid()).Path;
  QDir dir(path.c_str());
  dir.mkpath(nameEdit->text());

  path += "/" + nameEdit->text().toStdString();
  
  m_Synch->SetResourceType(midasResourceType::COLLECTION);
  m_Synch->SetClientHandle(path);
}

void CreateMidasResourceUI::AddItem()
{
  MidasCollectionTreeItem* parentColl =
    dynamic_cast<MidasCollectionTreeItem*>(m_ParentResource);

  if(!parentColl)
    {
    m_Synch->GetLog()->Error("CreateMidasResourceUI::AddCollection - no parent resource");
    return;
    }

  mds::DatabaseAPI db;
  std::string path = db.GetRecordByUuid(parentColl->getUuid()).Path;
  
  QDir dir(path.c_str());
  dir.mkpath(nameEdit->text().trimmed());

  path += "/" + nameEdit->text().trimmed().toStdString();

  m_Synch->SetResourceType(midasResourceType::ITEM);
  m_Synch->SetClientHandle(path);
}

void CreateMidasResourceUI::AddCommunity3(std::string path)
{
  QDir dir(path.c_str());
  dir.mkpath(nameEdit->text());

  path += "/" + nameEdit->text().toStdString();

  m_Synch->SetObject(NULL);
  m_Synch->SetResourceType3(midas3ResourceType::COMMUNITY);
  m_Synch->SetClientHandle(path);
}

void CreateMidasResourceUI::AddFolder(std::string path)
{
  QDir dir(path.c_str());
  dir.mkpath(nameEdit->text());

  path += "/" + nameEdit->text().toStdString();

  m_Synch->SetObject(NULL);
  m_Synch->SetResourceType3(midas3ResourceType::FOLDER);
  m_Synch->SetClientHandle(path);
}

void CreateMidasResourceUI::AddSubFolder()
{
  Midas3FolderTreeItem* parentFolder =
    dynamic_cast<Midas3FolderTreeItem*>(m_ParentResource3);

  if(!parentFolder)
    {
    m_Synch->GetLog()->Error("CreateMidasResourceUI::AddSubFolder - no parent resource");
    return;
    }

  std::string path = parentFolder->getFolder()->GetPath();
  QDir dir(path.c_str());
  dir.mkpath(nameEdit->text());

  path += "/" + nameEdit->text().toStdString();

  m_Synch->SetObject(parentFolder->getFolder());
  m_Synch->SetResourceType3(midas3ResourceType::FOLDER);
  m_Synch->SetClientHandle(path);
}

void CreateMidasResourceUI::AddItem3()
{
  Midas3FolderTreeItem* parentFolder =
    dynamic_cast<Midas3FolderTreeItem*>(m_ParentResource3);

  if(!parentFolder)
    {
    m_Synch->GetLog()->Error("CreateMidasResourceUI::AddItem3 - no parent resource");
    return;
    }

  std::string path = parentFolder->getFolder()->GetPath();
  QDir dir(path.c_str());
  dir.mkpath(nameEdit->text().trimmed());

  path += "/" + nameEdit->text().trimmed().toStdString();

  m_Synch->SetObject(parentFolder->getFolder());
  m_Synch->SetResourceType3(midas3ResourceType::ITEM);
  m_Synch->SetClientHandle(path);
}
