/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "m3dsBitstream.h"
#include "m3doBitstream.h"
#include "m3doItem.h"
#include "midasStandardIncludes.h"
#include "mdsResourceUpdateHandler.h"
#include "midasUtils.h"

#include <QFile>

namespace m3ds{

Bitstream::Bitstream()
{
  m_Bitstream = 0;
}
  
Bitstream::~Bitstream()
{
}

//-------------------------------------------------------------------
bool Bitstream::Fetch()
{
  mds::DatabaseAPI db;
  if(!m_Bitstream)
    {
    db.GetLog()->Error("Bitstream::Fetch : Bitstream not set\n");
    return false;
    }
    
  if(m_Bitstream->GetId() == 0)
    {
    db.GetLog()->Error("Bitstream::Fetch : BitstreamId not set\n");
    return false;
    }

  if(m_Bitstream->IsFetched())
    {
    return true;
    }

  std::stringstream query;
  query << "SELECT sizebytes, name, uuid, checksum, last_modified, date_creation, path "
    "FROM bitstream WHERE bitstream_id='" << m_Bitstream->GetId() << "'";
  db.Open();
  db.Database->ExecuteQuery(query.str().c_str());

  while(db.Database->GetNextRow())
    {
    std::stringstream val;
    val << db.Database->GetValueAsInt64(0);
    m_Bitstream->SetSize(val.str());
    m_Bitstream->SetName(db.Database->GetValueAsString(1));
    m_Bitstream->SetUuid(db.Database->GetValueAsString(2));
    m_Bitstream->SetChecksum(db.Database->GetValueAsString(3));
    m_Bitstream->SetLastModified(static_cast<unsigned int>(db.Database->GetValueAsInt(4)));
    m_Bitstream->SetCreationDate(db.Database->GetValueAsString(5));
    m_Bitstream->SetPath(db.Database->GetValueAsString(6));
    }
  m_Bitstream->SetFetched(true);
  db.Close();
  return true;
}

bool Bitstream::FetchParent()
{
  if(!m_Bitstream || !m_Bitstream->GetId())
    {
    return false;
    }

  if(m_Bitstream->GetParentItem())
    {
    return true; //already fetched parent
    }

  mds::DatabaseAPI db;
  std::stringstream query;
  query << "SELECT item_id, uuid, name, path, description "
    "FROM item WHERE item_id IN (SELECT item_id FROM bitstream WHERE "
    "bitstream_id='" << m_Bitstream->GetId() << "')";
  db.Open();
  db.Database->ExecuteQuery(query.str().c_str());

  while(db.Database->GetNextRow())
    {
    m3do::Item* parent = new m3do::Item;
    parent->SetId(db.Database->GetValueAsInt(0));
    parent->SetUuid(db.Database->GetValueAsString(1));
    parent->SetName(db.Database->GetValueAsString(2));
    parent->SetPath(db.Database->GetValueAsString(3));
    parent->SetDescription(db.Database->GetValueAsString(4));
    m_Bitstream->SetParentItem(parent);
    break;
    }
  db.Close();
  return (m_Bitstream->GetParentItem() != NULL);
}

/** Commit the object */
bool Bitstream::Commit()
{
  std::stringstream query;
  mds::DatabaseAPI db;

  int action;
  if(m_Bitstream->GetId()) //update existing record
    {
    action = midasDirtyAction::MODIFIED;
    
    query << "UPDATE bitstream SET "
      << "sizebytes='" << m_Bitstream->GetSize() << "', "
      << "name='" << midasUtils::EscapeForSQL(m_Bitstream->GetName()) << "', "
      << "date_update=current_timestamp"
      << "checksum='" << m_Bitstream->GetChecksum() << "', "
      << "uuid='" << m_Bitstream->GetUuid() << "', "
      << "path='" << m_Bitstream->GetPath() << "', "
      << "last_modified='" << m_Bitstream->GetLastModified()
      << "' WHERE bitstream_id='" << m_Bitstream->GetId() << "'";
    db.Open();
    if(!db.Database->ExecuteQuery(query.str().c_str()))
      {
      db.GetLog()->Error("Failed to update bitstream table");
      db.Close();
      return false;
      }
    }
  else //insert new record
    {
    action = midasDirtyAction::ADDED;
    if(!midasUtils::ValidateBitstreamName(m_Bitstream->GetName()))
      {
      db.GetLog()->Error("Invalid bitstream name: " + m_Bitstream->GetName());
      return false;
      }
    if(!m_Bitstream->GetParentItem() || !m_Bitstream->GetParentItem()->GetId())
      {
      db.GetLog()->Error("Parent item must be set when adding new bitstream");
      return false;
      }
    // Ensure no duplicate names under this item
    db.Open();
    query.str(std::string());
    query << "SELECT name FROM bitstream WHERE item_id='" <<
      m_Bitstream->GetParentItem()->GetId() << "'";
    db.Database->ExecuteQuery(query.str().c_str());
    while(db.Database->GetNextRow())
      {
      std::string name = db.Database->GetValueAsString(0);
      if(name == m_Bitstream->GetName())
        {
        db.GetLog()->Error("Skipped adding " + m_Bitstream->GetName() +
          " due to duplicate name");
        db.Close();
        return false;
        }
      }

    // Add bitstream record
    query.str(std::string());
    query << "INSERT INTO bitstream (item_id, name, sizebytes, uuid, "
      "checksum, path, last_modified) VALUES ('"
      << m_Bitstream->GetParentItem()->GetId() << "', '"
      << midasUtils::EscapeForSQL(m_Bitstream->GetName()) << "', '"
      << m_Bitstream->GetSize() << "', '"
      << m_Bitstream->GetUuid() << "', '"
      << m_Bitstream->GetChecksum() << "', '"
      << m_Bitstream->GetPath() << "', '"
      << m_Bitstream->GetLastModified() << "')";
    if(!db.Database->ExecuteQuery(query.str().c_str()))
      {
      db.GetLog()->Error("Failed to insert new record into bitstream table");
      db.Close();
      return false;
      }
    m_Bitstream->SetId(db.Database->GetLastInsertId());
    }

  if(db.UpdateHandler && action == midasDirtyAction::ADDED)
    {
    db.UpdateHandler->AddedResource(m_Bitstream);
    }
  else if(db.UpdateHandler && action == midasDirtyAction::MODIFIED)
    {
    db.UpdateHandler->UpdatedResource(m_Bitstream);
    }
  return true;
}

bool Bitstream::FetchTree()
{
  return true;
}

bool Bitstream::Delete(bool deleteOnDisk)
{
  mds::DatabaseAPI db;
  db.Open();
  db.Database->ExecuteQuery("BEGIN");
  std::stringstream query;
  query << "DELETE FROM bitstream WHERE bitstream_id='" << m_Bitstream->GetId() << "'";
  if(!db.Database->ExecuteQuery(query.str().c_str()))
    {
    db.Database->ExecuteQuery("ROLLBACK");
    db.Database->Close();
    return false;
    }

  db.Database->ExecuteQuery("COMMIT");
  db.Database->Close();
  if(deleteOnDisk)
    {
    QFile::remove(m_Bitstream->GetPath().c_str());
    }
  return true;
}

// Add the object
void Bitstream::SetObject(mdo::Object* object)
{  
  m_Bitstream = reinterpret_cast<m3do::Bitstream*>(object);
}

void Bitstream::ParentPathChanged(std::string parentPath)
{
  std::string newPath = parentPath + "/" +
    midasUtils::EscapeForSQL(m_Bitstream->GetName());
  std::stringstream query;
  query << "UPDATE bitstream SET path='" << newPath << "' WHERE "
    "bitstream_id='" << m_Bitstream->GetId() << "'";

  mds::DatabaseAPI db;
  db.Open();
  db.Database->ExecuteQuery(query.str().c_str());
  db.Close();
}

bool Bitstream::CopyContentIfExists()
{
  if(!m_Bitstream || m_Bitstream->GetChecksum() == ""
     || m_Bitstream->GetPath() == "")
    {
    return false;
    }

  mds::DatabaseAPI db;
  std::stringstream query;
  query << "SELECT path FROM bitstream WHERE checksum='" <<
    m_Bitstream->GetChecksum() << "'";
  db.Open();
  db.Database->ExecuteQuery(query.str().c_str());
  while(db.Database->GetNextRow())
    {
    // TODO invoke overwrite handler if we already have a file here?
    QFile existing(db.Database->GetValueAsString(0));
    db.Close();
    return existing.copy(m_Bitstream->GetPath().c_str());
    }
  db.Close();
  return false;

}

bool Bitstream::AlreadyExistsInItem()
{
  if(!m_Bitstream || !m_Bitstream->GetParentItem() ||
     !m_Bitstream->GetParentItem()->GetId())
    {
    return false;
    }

  mds::DatabaseAPI db;
  std::stringstream query;
  query << "SELECT name, checksum FROM bitstream WHERE item_id='" <<
    m_Bitstream->GetParentItem()->GetId() << "'";

  db.Open();
  db.Database->ExecuteQuery(query.str().c_str());
  while(db.Database->GetNextRow())
    {
    std::string name = db.Database->GetValueAsString(0);
    std::string checksum = db.Database->GetValueAsString(1);
    if(name == m_Bitstream->GetName() &&
       checksum == m_Bitstream->GetChecksum())
      {
      db.Close();
      return true;
      }
    }
  db.Close();
  return false;
}

} // end namespace
