#include "Midas3FolderTreeItem.h"
#include "Midas3ItemTreeItem.h"
#include "midasStandardIncludes.h"
#include "Midas3TreeModel.h"
#include "m3doFolder.h"
#include "m3doItem.h"

#include <iostream>
#include <QPixmap>
#include <QStyle>
#include <QModelIndex>

Midas3FolderTreeItem::Midas3FolderTreeItem(const QList<QVariant> &itemData,
                                           Midas3TreeModel* model,
                                           Midas3TreeItem* parent)
: Midas3TreeItem(itemData, model, parent)
{
  m_Folder = NULL;
}

Midas3FolderTreeItem::~Midas3FolderTreeItem()
{
}

void Midas3FolderTreeItem::SetFolder(m3do::Folder* folder)
{
  m_Folder = folder;
}

int Midas3FolderTreeItem::GetType() const
{
  if(m_Folder)
    {
    return m_Folder->GetResourceType();
    }
  return midas3ResourceType::FOLDER;
}

int Midas3FolderTreeItem::GetId() const
{
  return this->m_Folder->GetId();
}

std::string Midas3FolderTreeItem::GetUuid() const
{
  return m_Folder->GetUuid();
}

std::string Midas3FolderTreeItem::GetPath() const
{
  return m_Folder->GetPath();
}

void Midas3FolderTreeItem::Populate(QModelIndex parent)
{
  if(!m_Folder)
    {
    std::cerr << "Folder not set" << std::endl;
    return;
    }
  
  // Add the subfolders
  std::vector<m3do::Folder*>::const_iterator f = m_Folder->GetFolders().begin();
  int row = 0;
  while(f != m_Folder->GetFolders().end())
    {
    QList<QVariant> name;
    name << (*f)->GetName().c_str();
    Midas3FolderTreeItem* folder = new Midas3FolderTreeItem(name, m_Model, this);
    folder->SetClientResource(m_ClientResource);
    folder->SetFolder(*f);

    this->AppendChild(folder);
    QModelIndex index = m_Model->index(row, 0, parent);
    m_Model->registerResource((*f)->GetUuid(), index);

    if(this->IsDynamicFetch())
      {
      folder->SetFetchedChildren(false);
      folder->SetDynamicFetch(true);
      }
    else
      {
      folder->Populate(index);
      }

    if((*f)->IsDirty())
      {
      folder->SetDecorationRole(Midas3TreeItem::Dirty);
      }
    f++;
    row++;
    }

  // Add the items
  std::vector<m3do::Item*>::const_iterator i = m_Folder->GetItems().begin();
  while(i != m_Folder->GetItems().end())
    {
    QList<QVariant> name;
    name << (*i)->GetName().c_str();
    Midas3ItemTreeItem* item = new Midas3ItemTreeItem(name, m_Model, this);
    item->SetClientResource(m_ClientResource);
    item->SetItem(*i);

    this->AppendChild(item);
    QModelIndex index = m_Model->index(row, 0, parent);
    m_Model->registerResource((*f)->GetUuid(), index);

    if(this->IsDynamicFetch())
      {
      item->SetFetchedChildren(false);
      item->SetDynamicFetch(true);
      }
    else
      {
      item->Populate(index);
      }

    if((*i)->IsDirty())
      {
      item->SetDecorationRole(Midas3TreeItem::Dirty);
      }
    i++;
    row++;
    }

  if(!this->IsDynamicFetch())
    {
    this->SetFetchedChildren(true);
    }
}

void Midas3FolderTreeItem::UpdateDisplayName()
{
  QVariant name = this->GetFolder()->GetName().c_str();
  this->SetData(name,0);
}

void Midas3FolderTreeItem::RemoveFromTree()
{
}

m3do::Folder* Midas3FolderTreeItem::GetFolder() const
{
  return m_Folder;
}

mdo::Object* Midas3FolderTreeItem::GetObject() const
{
  return m_Folder;
}

bool Midas3FolderTreeItem::ResourceIsFetched() const
{
  return m_Folder->IsFetched();
}

QPixmap Midas3FolderTreeItem::GetDecoration()
{
  std::string role;
  if(m_Folder->GetResourceType() == midas3ResourceType::COMMUNITY)
    {
    role = ":icons/user_group";
    }
  else
    {
    role = ":icons/gpl_folder";

    if(m_DecorationRole & Expanded)
      {
      role += "_open";
      }
    if(m_DecorationRole & Dirty)
      {
      role += "_red";
      }
    }
  role += ".png";
  return QPixmap(role.c_str());
}
