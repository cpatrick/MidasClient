/******************************************************************************
 * Copyright 2011 Kitware Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#include "m3dsFolder.h"
#include "m3doFolder.h"
#include "m3dsItem.h"
#include "m3doItem.h"
#include "mdsResourceUpdateHandler.h"
#include "midasStandardIncludes.h"
#include "midasUtils.h"

#include <QFileInfo>
#include <QDir>

namespace m3ds
{

Folder::Folder()
  : m_Recurse(true), m_Folder(NULL)
{
}

Folder::~Folder()
{
}

void Folder::SetRecursive(bool recurse)
{
  m_Recurse = recurse;
}

bool Folder::Fetch()
{
  mds::DatabaseAPI db;

  if( !m_Folder )
    {
    db.GetLog()->Error("Folder::Fetch : Folder not set\n");
    return false;
    }

  if( m_Folder->GetId() == 0 )
    {
    db.GetLog()->Error("Folder::Fetch : FolderId not set\n");
    return false;
    }

  if( m_Folder->IsFetched() )
    {
    return true;
    }

  std::stringstream query;
  query << "SELECT uuid, path, name, description FROM folder "
  "WHERE folder_id='" << m_Folder->GetId() << "'";

  db.Open();
  if( !db.Database->ExecuteQuery(query.str().c_str() ) )
    {
    std::stringstream text;
    text << "Folder::Fetch : Query failed: " << query.str() << std::endl;
    db.GetLog()->Error(text.str() );
    db.Close();
    return false;
    }

  while( db.Database->GetNextRow() )
    {
    m_Folder->SetUuid(db.Database->GetValueAsString(0) );
    m_Folder->SetPath(db.Database->GetValueAsString(1) );
    m_Folder->SetName(db.Database->GetValueAsString(2) );
    m_Folder->SetDescription(db.Database->GetValueAsString(3) );
    }

  m_Folder->SetFetched(true);
  db.Close();
  return true;
}

bool Folder::FetchSize()
{
  if( !m_Folder )
    {
    return false;
    }
  double       total = 0;
  unsigned int count = 0;
  for( std::vector<m3do::Folder *>::const_iterator i = m_Folder->GetFolders().begin();
       i != m_Folder->GetFolders().end(); ++i )
    {
    if( (*i)->GetSize() == "" )
      {
      m3ds::Folder mdsFolder;
      mdsFolder.SetObject(*i);
      mdsFolder.FetchSize();
      }
    total += midasUtils::StringToDouble( (*i)->GetSize() );
    count += (*i)->GetBitstreamCount();
    }
  for( std::vector<m3do::Item *>::const_iterator i = m_Folder->GetItems().begin();
       i != m_Folder->GetItems().end(); ++i )
    {
    if( (*i)->GetSize() == "" )
      {
      m3ds::Item mdsItem;
      mdsItem.SetObject(*i);
      mdsItem.FetchSize();
      }
    total += midasUtils::StringToDouble( (*i)->GetSize() );
    count += (*i)->GetBitstreamCount();
    }
  std::stringstream sizeStr;
  sizeStr << total;
  m_Folder->SetSize(sizeStr.str() );
  m_Folder->SetBitstreamCount(count);
  return true;
}

bool Folder::Commit()
{
  mds::DatabaseAPI db;

  if( !m_Folder )
    {
    db.GetLog()->Error("Folder::Commit : Folder not set\n");
    return false;
    }

  if( m_Folder->GetId() == 0 )
    {
    db.GetLog()->Error("Folder::Commit : FolderId not set\n");
    return false;
    }

  if( m_Folder->GetUuid() == "" )
    {
    db.GetLog()->Error("Folder::Commit : Uuid not set\n");
    return false;
    }

  QFileInfo   fileInfo(m_Folder->GetPath().c_str() );
  std::string parentDir = fileInfo.dir().path().toStdString();
  std::string oldName = fileInfo.fileName().toStdString();

  if( oldName != m_Folder->GetName() )
    {
    std::string newPath = parentDir + "/" + m_Folder->GetName();
    if( rename(m_Folder->GetPath().c_str(), newPath.c_str() ) == 0 )
      {
      m_Folder->SetPath(newPath);
      this->FetchTree();
      for( std::vector<m3do::Folder *>::const_iterator i =
             m_Folder->GetFolders().begin();
           i != m_Folder->GetFolders().end(); ++i )
        {
        m3ds::Folder mdsFolder;
        mdsFolder.SetObject(*i);
        mdsFolder.ParentPathChanged(newPath);
        }
      for( std::vector<m3do::Item *>::const_iterator i =
             m_Folder->GetItems().begin();
           i != m_Folder->GetItems().end(); ++i )
        {
        m3ds::Item mdsItem;
        mdsItem.SetObject(*i);
        mdsItem.ParentPathChanged(newPath);
        }
      }
    else
      {
      db.GetLog()->Error("Folder::Commit : could not rename "
                         "directory on disk. It may be locked.\n");
      return false;
      }
    }

  std::stringstream query;
  query << "UPDATE folder SET "
        << "name='"
        << midasUtils::EscapeForSQL(m_Folder->GetName() ) << "', "
  "description='"
        << midasUtils::EscapeForSQL(m_Folder->GetDescription() ) << "', "
  "path='" << midasUtils::EscapeForSQL(m_Folder->GetPath() ) << "', "
  "uuid='" << m_Folder->GetUuid() << "' WHERE "
  "folder_id='" << m_Folder->GetId() << "'";

  db.Open();
  if( db.Database->ExecuteQuery(query.str().c_str() ) )
    {
    db.Close();
    /*if(m_MarkDirty)
      {
      db.MarkDirtyResource(m_Folder->GetUuid(),
        midasDirtyAction::MODIFIED);
      }*/
    return true;
    }
  std::stringstream text;
  text << "Folder::Commit : Query failed: " << query.str() << std::endl;
  db.GetLog()->Error(text.str() );
  db.Close();
  return false;
}

bool Folder::FetchTree()
{
  mds::DatabaseAPI db;

  if( !m_Folder )
    {
    db.GetLog()->Error("Folder::FetchTree : Folder not set\n");
    return false;
    }

  if( m_Folder->GetId() == 0 )
    {
    db.GetLog()->Error("Folder::FetchTree : FolderId not set\n");
    return false;
    }

  if( m_Folder->GetUuid() == "" )
    {
    db.GetLog()->Error("Folder::FetchTree : Uuid not set\n");
    return false;
    }

  // m_Folder->SetDirty(db.IsResourceDirty(m_Folder->GetUuid()));

  std::stringstream query;
  query << "SELECT folder_id, path, name, uuid, description FROM folder "
  "WHERE parent_id='" << m_Folder->GetId()
        << "' ORDER BY name COLLATE NOCASE ASC";
  db.Open();
  db.Database->ExecuteQuery(query.str().c_str() );

  std::vector<m3do::Folder *> childFolders;
  while( db.Database->GetNextRow() )
    {
    m3do::Folder* folder = new m3do::Folder;
    folder->SetId(db.Database->GetValueAsInt(0) );
    folder->SetPath(db.Database->GetValueAsString(1) );
    folder->SetName(db.Database->GetValueAsString(2) );
    folder->SetUuid(db.Database->GetValueAsString(3) );
    folder->SetDescription(db.Database->GetValueAsString(4) );
    childFolders.push_back(folder);
    m_Folder->AddFolder(folder);
    }

  db.Close();
  /*for(std::vector<m3do::Folder*>::const_iterator i =
      m_Folder->GetFolders().begin();
      i != m_Folder->GetFolders().end(); ++i)
    {
    (*i)->SetDirty(db.IsResourceDirty((*i)->GetUuid()));
    }*/

  if( m_Recurse )
    {
    for( std::vector<m3do::Folder *>::iterator i = childFolders.begin();
         i != childFolders.end(); ++i )
      {
      m3ds::Folder mdsFolder;
      mdsFolder.SetObject(*i);
      if( !mdsFolder.FetchTree() )
        {
        return false;
        }
      }
    }

  query.str(std::string() );
  query << "SELECT item_id, path, name, uuid, description FROM item "
  "WHERE item_id IN "
  "(SELECT item_id FROM item2folder WHERE folder_id="
  << m_Folder->GetId() << ") ORDER BY name COLLATE NOCASE ASC";
  db.Open();
  db.Database->ExecuteQuery(query.str().c_str() );

  std::vector<m3do::Item *> items;
  while( db.Database->GetNextRow() )
    {
    m3do::Item* item = new m3do::Item;
    item->SetId(db.Database->GetValueAsInt(0) );
    item->SetPath(db.Database->GetValueAsString(1) );
    item->SetName(db.Database->GetValueAsString(2) );
    item->SetUuid(db.Database->GetValueAsString(3) );
    item->SetDescription(db.Database->GetValueAsString(4) );
    items.push_back(item);
    m_Folder->AddItem(item);
    }

  db.Close();
  /*for(std::vector<m3do::Item*>::const_iterator i =
      m_Folder->GetItems().begin();
      i != m_Folder->GetItems().end(); ++i)
    {
    (*i)->SetDirty(db.IsResourceDirty((*i)->GetUuid()));
    }*/

  if( m_Recurse )
    {
    for( std::vector<m3do::Item *>::iterator i = items.begin();
         i != items.end(); ++i )
      {
      m3ds::Item mdsItem;
      mdsItem.SetObject(*i);
      if( !mdsItem.FetchTree() )
        {
        return false;
        }
      }
    }
  return true;
}

bool Folder::Delete(bool deleteOnDisk)
{
  this->FetchTree(); // populate child vectors
  for( std::vector<m3do::Folder *>::iterator f = m_Folder->GetFolders().begin();
       f != m_Folder->GetFolders().end(); ++f )
    {
    Folder mdsFolder;
    mdsFolder.SetObject(*f);
    if( !mdsFolder.Delete(deleteOnDisk) )
      {
      return false;
      }
    }
  for( std::vector<m3do::Item *>::iterator i = m_Folder->GetItems().begin();
       i != m_Folder->GetItems().end(); ++i )
    {
    Item mdsItem;
    mdsItem.SetObject(*i);
    if( !mdsItem.Delete(deleteOnDisk) )
      {
      return false;
      }
    }

  mds::DatabaseAPI db;
  db.Open();
  std::stringstream query;
  query << "DELETE FROM folder WHERE folder_id='"
        << m_Folder->GetId() << "'";
  if( !db.Database->ExecuteQuery(query.str().c_str() ) )
    {
    db.Database->Close();
    return false;
    }
  db.Database->Close();

  if( deleteOnDisk )
    {
    midasUtils::RemoveDir(m_Folder->GetPath() );
    }
  return true;
}

bool Folder::FetchParent()
{
  if( !m_Folder || !m_Folder->GetId() )
    {
    return false;
    }

  if( m_Folder->GetParentFolder() )
    {
    return true; // already fetched parent
    }

  mds::DatabaseAPI  db;
  std::stringstream query;
  query << "SELECT folder_id, uuid, name, path, description "
  "FROM folder WHERE folder_id IN "
  "(SELECT parent_id FROM folder WHERE folder_id='"
  << m_Folder->GetId() << "')";
  db.Open();
  db.Database->ExecuteQuery(query.str().c_str() );

  while( db.Database->GetNextRow() )
    {
    m3do::Folder* parent = new m3do::Folder;
    parent->SetId(db.Database->GetValueAsInt(0) );
    parent->SetUuid(db.Database->GetValueAsString(1) );
    parent->SetName(db.Database->GetValueAsString(2) );
    parent->SetPath(db.Database->GetValueAsString(3) );
    parent->SetDescription(db.Database->GetValueAsString(4) );
    m_Folder->SetParentFolder(parent);
    break;
    }

  db.Close();
  return true;
}

void Folder::SetObject(mdo::Object* object)
{
  m_Folder = reinterpret_cast<m3do::Folder *>(object);
}

void Folder::ParentPathChanged(const std::string& parentPath)
{
  mds::DatabaseAPI  db;
  std::string       newPath = parentPath + "/" + m_Folder->GetName();
  std::stringstream query;

  query << "UPDATE folder SET path='" << newPath << "' WHERE "
  "folder_id='" << m_Folder->GetId() << "'";

  db.Open();
  db.Database->ExecuteQuery(query.str().c_str() );
  db.Close();
  for( std::vector<m3do::Folder *>::const_iterator i =
         m_Folder->GetFolders().begin();
       i != m_Folder->GetFolders().end(); ++i )
    {
    m3ds::Folder mdsFolder;
    mdsFolder.SetObject(*i);
    mdsFolder.ParentPathChanged(newPath);
    }
  for( std::vector<m3do::Item *>::const_iterator i =
         m_Folder->GetItems().begin();
       i != m_Folder->GetItems().end(); ++i )
    {
    m3ds::Item mdsItem;
    mdsItem.SetObject(*i);
    mdsItem.ParentPathChanged(newPath);
    }
}

bool Folder::Create()
{
  if( !this->m_Folder )
    {
    return false;
    }

  mds::DatabaseAPI db;
  std::string      path;
  int              parentId;
  if( this->m_Folder->GetParentFolder() )
    {
    path = this->m_Folder->GetParentFolder()->GetPath()
      + "/" + m_Folder->GetName();
    parentId = this->m_Folder->GetParentFolder()->GetId();
    }
  else
    {
    path = db.GetSetting(mds::DatabaseAPI::ROOT_DIR);
    if( path == "" )
      {
      path = QDir::currentPath().toStdString() + "/" + m_Folder->GetName();
      }
    parentId = this->m_Folder->GetResourceType() ==
      midas3ResourceType::COMMUNITY ? -2 : -1;
    }

  db.Open();
  std::stringstream query;
  query << "SELECT folder_id FROM folder WHERE uuid='"
        << m_Folder->GetUuid() << "' LIMIT 1";
  db.Database->ExecuteQuery(query.str().c_str() );

  // we already have a folder with this uuid, so just update the record
  if( db.Database->GetNextRow() )
    {
    m_Folder->SetId(db.Database->GetValueAsInt(0) );
    m_Folder->SetPath(path);
    m_Folder->SetParentId(parentId);
    db.Database->Close();
    return this->Commit();
    }

  query.str(std::string() );
  query << "INSERT INTO folder (name, description, uuid, path, parent_id) "
  "VALUES ('" << midasUtils::EscapeForSQL(m_Folder->GetName() ) << "', '"
        << midasUtils::EscapeForSQL(m_Folder->GetDescription() ) << "', '"
        << m_Folder->GetUuid() << "', '" << path << "', '" << parentId << "')";
  if( !db.Database->ExecuteQuery(query.str().c_str() ) )
    {
    db.GetLog()->Error("Folder::Create : Insert folder record failed");
    db.Database->Close();
    return false;
    }
  m_Folder->SetId(db.Database->GetLastInsertId() );
  m_Folder->SetPath(path);
  db.Database->Close();

  mds::DatabaseInfo::Instance()->GetResourceUpdateHandler()
  ->AddedResource(this->m_Folder);
  return true;
}

} // end namespace
