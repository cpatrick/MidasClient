#include "CreateMidasResourceUI.h"

#include <QtGui> 
#include <Qt>
#include <QMessageBox>

#include "MIDASDesktopUI.h"
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

  m_Synch->SetOperation(midasSynchronizer::OPERATION_ADD);

  std::string type_str, path;
  mds::DatabaseAPI db;
  switch(this->m_Type)
    {
    case Community:
      path = db.GetAuthProfile(
        m_Synch->GetAuthenticator()->GetProfile()).RootDir;

      if(path == "")
        {
        path = db.GetSetting(mds::DatabaseAPI::ROOT_DIR);
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
  path += "/" + nameEdit->text().toStdString();
  kwsys::SystemTools::MakeDirectory(path.c_str());

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

  path += "/" + nameEdit->text().toStdString();
  kwsys::SystemTools::MakeDirectory(path.c_str());

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

  path += "/" + nameEdit->text().toStdString();
  kwsys::SystemTools::MakeDirectory(path.c_str());

  m_Synch->SetResourceType(midasResourceType::ITEM);
  m_Synch->SetClientHandle(path);
}
