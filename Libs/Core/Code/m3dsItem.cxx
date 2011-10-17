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

#include "m3dsItem.h"

#include "m3doItem.h"
#include "m3doFolder.h"
#include "m3dsBitstream.h"
#include "m3doBitstream.h"
#include "mdsResourceUpdateHandler.h"
#include "midasStandardIncludes.h"

#include <QFileInfo>
#include <QDir>

namespace m3ds
{

Item::Item()
  : m_Item(NULL)
{
}

Item::~Item()
{
}

/** Fetch */
bool Item::Fetch()
{
  mds::DatabaseAPI db;

  if( !m_Item )
    {
    db.GetLog()->Error("Item::Fetch : Item not set\n");
    return false;
    }

  if( m_Item->GetId() == 0 )
    {
    db.GetLog()->Error("Item::Fetch : ItemId not set\n");
    return false;
    }

  if( m_Item->IsFetched() )
    {
    return true;
    }

  std::stringstream query;
  query << "SELECT uuid, path, name, description FROM item WHERE item_id='"
        << m_Item->GetId() << "'";

  db.Open();
  db.Database->ExecuteQuery(query.str().c_str() );

  while( db.Database->GetNextRow() )
    {
    m_Item->SetUuid(db.Database->GetValueAsString(0) );
    m_Item->SetPath(db.Database->GetValueAsString(1) );
    m_Item->SetName(db.Database->GetValueAsString(2) );
    m_Item->SetDescription(db.Database->GetValueAsString(3) );
    }

  db.Close();
  m_Item->SetFetched(true);
  return true;
}

/** Commit */
bool Item::Commit()
{
  bool ok = true;

  mds::DatabaseAPI db;
  QFileInfo        fileInfo(m_Item->GetPath().c_str() );
  std::string      parentDir = fileInfo.dir().path().toStdString();
  std::string      oldName = fileInfo.fileName().toStdString();

  if( oldName != m_Item->GetName() )
    {
    std::string newPath = parentDir + "/" + m_Item->GetName();
    if( rename(m_Item->GetPath().c_str(), newPath.c_str() ) == 0 )
      {
      m_Item->SetPath(newPath);
      this->FetchTree();
      for( std::vector<m3do::Bitstream *>::const_iterator i =
             m_Item->GetBitstreams().begin();
           i != m_Item->GetBitstreams().end(); ++i )
        {
        m3ds::Bitstream mdsBitstream;
        mdsBitstream.SetObject(*i);
        mdsBitstream.ParentPathChanged(newPath);
        }
      }
    else
      {
      db.GetLog()->Error("Item::Commit : could not rename directory "
                         "on disk. It may be locked.\n");
      return false;
      }
    }

  db.Open();
  std::stringstream query;
  query << "UPDATE item SET name='" << midasUtils::EscapeForSQL(m_Item->GetName() )
        << "', path='" << m_Item->GetPath()
        << "', uuid='" << m_Item->GetUuid()
        << "', description='" << midasUtils::EscapeForSQL(m_Item->GetDescription() )
        << "' WHERE item_id='" << m_Item->GetId() << "'";
  ok &= db.Database->ExecuteQuery(query.str().c_str() );
  db.Close();

  if( ok )
    {
    /* TODO dirty resources?
    if(m_MarkDirty)
      {
      db.MarkDirtyResource(m_Item->GetUuid(), midasDirtyAction::MODIFIED);
      }*/
    return true;
    }
  db.GetLog()->Error("Item::Commit : database update failed");
  return false;
}

bool Item::FetchTree()
{
  mds::DatabaseAPI db;

  if( !m_Item )
    {
    db.GetLog()->Error("Item::FetchTree : Item not set\n");
    return false;
    }

  if( m_Item->GetId() == 0 )
    {
    db.GetLog()->Error("Item::FetchTree : ItemId not set\n");
    return false;
    }

  std::stringstream query;
  query << "SELECT bitstream_id, name, sizebytes, checksum, uuid, last_modified, path "
  "FROM bitstream WHERE item_id='" << m_Item->GetId()
        << "' ORDER BY name COLLATE NOCASE ASC";
  db.Open();
  db.Database->ExecuteQuery(query.str().c_str() );

  while( db.Database->GetNextRow() )
    {
    m3do::Bitstream* bitstream = new m3do::Bitstream;
    bitstream->SetId(db.Database->GetValueAsInt(0) );
    bitstream->SetName(db.Database->GetValueAsString(1) );
    std::stringstream val;
    val << db.Database->GetValueAsInt64(2);
    bitstream->SetSize(val.str() );
    bitstream->SetChecksum(db.Database->GetValueAsString(3) );
    bitstream->SetUuid(db.Database->GetValueAsString(4) );
    bitstream->SetLastModified(db.Database->GetValueAsInt(5) );
    bitstream->SetPath(db.Database->GetValueAsString(6) );
    bitstream->SetFetched(true);
    m_Item->AddBitstream(bitstream);
    }

  db.Close();

  return true;
}

bool Item::FetchSize()
{
  if( !m_Item )
    {
    return false;
    }
  double total = 0;
  for( std::vector<m3do::Bitstream *>::const_iterator i = m_Item->GetBitstreams().begin();
       i != m_Item->GetBitstreams().end(); ++i )
    {
    total += midasUtils::StringToDouble( (*i)->GetSize() );
    }
  std::stringstream sizeStr;
  sizeStr << total;
  m_Item->SetSize(sizeStr.str() );
  m_Item->SetBitstreamCount(m_Item->GetBitstreams().size() );
  return true;
}

bool Item::Delete(bool deleteOnDisk)
{
  mds::DatabaseAPI db;

  std::vector<m3do::Bitstream *> children;
  std::stringstream              query;
  query << "SELECT bitstream_id, uuid, path FROM bitstream WHERE "
  "item_id='" << m_Item->GetId() << "'";
  db.Open();
  if( !db.Database->ExecuteQuery(query.str().c_str() ) )
    {
    db.Database->Close();
    return false;
    }

  while( db.Database->GetNextRow() )
    {
    m3do::Bitstream* bitstream = new m3do::Bitstream();
    bitstream->SetId(db.Database->GetValueAsInt(0) );
    bitstream->SetUuid(db.Database->GetValueAsString(1) );
    bitstream->SetPath(db.Database->GetValueAsString(2) );
    children.push_back(bitstream);
    }

  db.Database->Close();

  bool ok = true;
  for( std::vector<m3do::Bitstream *>::iterator i = children.begin();
       i != children.end(); ++i )
    {
    m3ds::Bitstream mdsBitstream;
    mdsBitstream.SetObject(*i);
    ok &= mdsBitstream.Delete(deleteOnDisk);
    delete *i;

    if( !ok )
      {
      return false;
      }
    }

  db.Open();
  db.Database->ExecuteQuery("BEGIN");
  query.str(std::string() );
  query << "DELETE FROM item WHERE item_id='"
        << m_Item->GetId() << "'";
  if( !db.Database->ExecuteQuery(query.str().c_str() ) )
    {
    db.Database->ExecuteQuery("ROLLBACK");
    db.Database->Close();
    return false;
    }
  query.str(std::string() );
  query << "DELETE FROM item2folder WHERE item_id='"
        << m_Item->GetId() << "'";
  if( !db.Database->ExecuteQuery(query.str().c_str() ) )
    {
    db.Database->ExecuteQuery("ROLLBACK");
    db.Database->Close();
    return false;
    }
  db.Database->ExecuteQuery("COMMIT");

  db.Database->Close();
  if( deleteOnDisk )
    {
    midasUtils::RemoveDir(m_Item->GetPath() );
    }
  return true;
}

bool Item::FetchParent()
{
  if( !m_Item || !m_Item->GetId() )
    {
    return false;
    }

  if( m_Item->GetParentFolder() )
    {
    return true; // already fetched parent
    }

  mds::DatabaseAPI  db;
  std::stringstream query;
  query << "SELECT folder_id, uuid, name, path, description "
  "FROM folder WHERE folder_id IN "
  "(SELECT folder_id FROM item2folder WHERE item_id='"
  << m_Item->GetId() << "')";
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
    m_Item->SetParentFolder(parent);
    break;
    }

  db.Close();
  return m_Item->GetParentFolder() != NULL;
}

void Item::SetObject(mdo::Object* object)
{
  m_Item = reinterpret_cast<m3do::Item *>(object);
}

void Item::ParentPathChanged(const std::string& parentPath)
{
  std::string       newPath = parentPath + "/" + m_Item->GetName();
  std::stringstream query;

  query << "UPDATE item SET path='" << newPath << "' WHERE "
  "item_id='" << m_Item->GetId() << "'";

  mds::DatabaseAPI db;
  db.Open();
  db.Database->ExecuteQuery(query.str().c_str() );
  db.Close();

  /*TODO propagate path down
  for(std::vector<mdo::Bitstream*>::const_iterator i =
      m_Item->GetBitstreams().begin();
      i != m_Item->GetBitstreams().end(); ++i)
    {
    mds::Bitstream mdsBitstream;
    mdsBitstream.SetObject(*i);
    mdsBitstream.ParentPathChanged(newPath);
    }*/
}

bool Item::Create()
{
  if( !this->m_Item )
    {
    return false;
    }
  if( !this->m_Item->GetParentFolder() )
    {
    return false;
    }
  std::string path = this->m_Item->GetParentFolder()->GetPath()
    + "/" + m_Item->GetName();

  mds::DatabaseAPI db;
  db.Open();
  std::stringstream query;
  query << "SELECT item_id FROM item WHERE uuid='"
        << m_Item->GetUuid() << "' LIMIT 1";
  db.Database->ExecuteQuery(query.str().c_str() );

  // we already have an item with this uuid, so just update the record
  if( db.Database->GetNextRow() )
    {
    m_Item->SetId(db.Database->GetValueAsInt(0) );
    m_Item->SetPath(path);
    db.Database->Close();
    return this->Commit();
    }

  if( !db.Database->ExecuteQuery("BEGIN") )
    {
    db.Close();
    return false;
    }
  query.str(std::string() );
  query << "INSERT INTO item (name, description, uuid, path) VALUES ('"
        << midasUtils::EscapeForSQL(m_Item->GetName() ) << "', '"
        << midasUtils::EscapeForSQL(m_Item->GetDescription() ) << "', '"
        << m_Item->GetUuid() << "', '" << path << "')";
  if( !db.Database->ExecuteQuery(query.str().c_str() ) )
    {
    db.GetLog()->Error("Item::Create : Insert item record failed");
    db.Database->ExecuteQuery("ROLLBACK");
    db.Database->Close();
    return false;
    }

  m_Item->SetPath(path);
  m_Item->SetId(db.Database->GetLastInsertId() );
  query.str(std::string() );
  query << "INSERT INTO item2folder (item_id, folder_id) VALUES ('"
        << m_Item->GetId() << "', '"
        << m_Item->GetParentFolder()->GetId() << "')";
  if( !db.Database->ExecuteQuery(query.str().c_str() ) )
    {
    db.GetLog()->Error("Item::Create : Insert item2folder record failed");
    db.Database->ExecuteQuery("ROLLBACK");
    db.Database->Close();
    return false;
    }
  db.Database->ExecuteQuery("COMMIT");
  db.Database->Close();

  mds::DatabaseInfo::Instance()->GetResourceUpdateHandler()
  ->AddedResource(this->m_Item);
  return true;
}

} // end namespace
