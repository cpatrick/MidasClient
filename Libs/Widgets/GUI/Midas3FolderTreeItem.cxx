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

void Midas3FolderTreeItem::setFolder(m3do::Folder* folder)
{
  m_Folder = folder;
}

int Midas3FolderTreeItem::getType() const
{
  if(m_Folder)
    {
    return m_Folder->GetResourceType();
    }
  return midas3ResourceType::FOLDER;
}

int Midas3FolderTreeItem::getId() const
{
  return this->m_Folder->GetId();
}

std::string Midas3FolderTreeItem::getUuid() const
{
  return m_Folder->GetUuid();
}

std::string Midas3FolderTreeItem::getPath() const
{
  return m_Folder->GetPath();
}

void Midas3FolderTreeItem::populate(QModelIndex parent)
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
    folder->setClientResource(m_ClientResource);
    folder->setFolder(*f);

    this->appendChild(folder);
    QModelIndex index = m_Model->index(row, 0, parent);
    m_Model->registerResource((*f)->GetUuid(), index);

    if(this->isDynamicFetch())
      {
      folder->setFetchedChildren(false);
      folder->setDynamicFetch(true);
      }
    else
      {
      folder->populate(index);
      }

    if((*f)->IsDirty())
      {
      folder->setDecorationRole(Midas3TreeItem::Dirty);
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
    item->setClientResource(m_ClientResource);
    item->setItem(*i);

    this->appendChild(item);
    QModelIndex index = m_Model->index(row, 0, parent);
    m_Model->registerResource((*f)->GetUuid(), index);

    if(this->isDynamicFetch())
      {
      item->setFetchedChildren(false);
      item->setDynamicFetch(true);
      }
    else
      {
      item->populate(index);
      }

    if((*i)->IsDirty())
      {
      item->setDecorationRole(Midas3TreeItem::Dirty);
      }
    i++;
    row++;
    }

  if(!this->isDynamicFetch())
    {
    this->setFetchedChildren(true);
    }
}

void Midas3FolderTreeItem::updateDisplayName()
{
  QVariant name = this->getFolder()->GetName().c_str();
  this->setData(name,0);
}

void Midas3FolderTreeItem::removeFromTree()
{
}

m3do::Folder* Midas3FolderTreeItem::getFolder() const
{
  return m_Folder;
}

mdo::Object* Midas3FolderTreeItem::getObject() const
{
  return m_Folder;
}

bool Midas3FolderTreeItem::resourceIsFetched() const
{
  return m_Folder->IsFetched();
}

QPixmap Midas3FolderTreeItem::getDecoration()
{
  std::string role;
  if(m_Folder->GetResourceType() == midas3ResourceType::COMMUNITY)
    {
    role = ":icons/user_group";
    }
  else
    {
    role = ":icons/gpl_folder";

    if(this->decorationRole & Expanded)
      {
      role += "_open";
      }
    if(this->decorationRole & Dirty)
      {
      role += "_red";
      }
    }
  role += ".png";
  return QPixmap(role.c_str());
}
