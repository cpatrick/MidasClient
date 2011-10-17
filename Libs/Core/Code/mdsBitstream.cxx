/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mdsBitstream.h"
#include "mdoBitstream.h"
#include "midasStandardIncludes.h"
#include "mdsResourceUpdateHandler.h"
#include "midasUtils.h"

#include <QFile>

namespace mds{

/** Constructor */
Bitstream::Bitstream()
{
  m_Bitstream = 0;
}
  
/** Destructor */
Bitstream::~Bitstream()
{
}

/** Fetch the object */
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
  query << "SELECT size_bytes, name, internal_id FROM bitstream WHERE bitstream_id='"
    << m_Bitstream->GetId() << "'";
  db.Open();
  db.Database->ExecuteQuery(query.str().c_str());

  while(db.Database->GetNextRow())
    {
    std::stringstream val;
    val << db.Database->GetValueAsInt64(0);
    m_Bitstream->SetSize(val.str());
    m_Bitstream->SetName(db.Database->GetValueAsString(1));
    m_Bitstream->SetPath(db.Database->GetValueAsString(2));
    }
  m_Bitstream->SetFetched(true);
  db.Close();
  return true;
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
    db.Open();
    db.Database->ExecuteQuery("BEGIN"); //begin update transaction
    query << "UPDATE bitstream SET "
      << "size_bytes='" << m_Bitstream->GetSize() << "', "
      << "name='" << midasUtils::EscapeForSQL(m_Bitstream->GetName()) << "', "
      << "last_modified='" << m_Bitstream->GetLastModified()
      << "' WHERE bitstream_id='" << m_Bitstream->GetId() << "'";
    if(!db.Database->ExecuteQuery(query.str().c_str()))
      {
      db.GetLog()->Error("Failed to update bitstream table");
      db.Database->ExecuteQuery("ROLLBACK");
      db.Close();
      return false;
      }
    }
  else //insert new record
    {
    if(!midasUtils::ValidateBitstreamName(m_Bitstream->GetName()))
      {
      db.GetLog()->Error("Invalid bitstream name: " + m_Bitstream->GetName());
      return false;
      }
    if(m_Bitstream->GetUuid() == "")
      {
      db.GetLog()->Error("Uuid must be set when adding new bitstream");
      return false;
      }
    if(m_Bitstream->GetParentId() == 0)
      {
      db.GetLog()->Error("ParentId must be set when adding new bitstream");
      return false;
      }
    // Ensure no duplicate names under this item
    db.Open();
    query.str(std::string());
    query << "SELECT bitstream.name FROM bitstream, item2bitstream WHERE "
      "item2bitstream.bitstream_id=bitstream.bitstream_id AND "
      "item2bitstream.item_id=" << m_Bitstream->GetParentId();
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

    db.Database->ExecuteQuery("BEGIN"); //begin add transaction
    action = midasDirtyAction::ADDED;

    // Add bitstream record
    query.str(std::string());
    query << "INSERT INTO bitstream (location, internal_id, name, "
      "size_bytes, last_modified) VALUES ('1','"
      << m_Bitstream->GetPath() << "', '"
      << midasUtils::EscapeForSQL(m_Bitstream->GetName()) << "', '"
      << m_Bitstream->GetSize() << "', '"
      << m_Bitstream->GetLastModified() << "')";
    if(!db.Database->ExecuteQuery(query.str().c_str()))
      {
      db.GetLog()->Error("Failed to insert new record into bitstream table");
      db.Database->ExecuteQuery("ROLLBACK");
      db.Close();
      return false;
      }
    m_Bitstream->SetId(db.Database->GetLastInsertId());
    
    // Add resource_uuid record
    query.str(std::string());
    query << "INSERT INTO resource_uuid (resource_type_id, resource_id, path,"
      " uuid, server_parent) VALUES ('" <<
      static_cast<int>(midasResourceType::BITSTREAM) <<
      "', '" << m_Bitstream->GetId() << "', '" << m_Bitstream->GetPath() <<
      "', '" << m_Bitstream->GetUuid() << "', '0')";
    if(!db.Database->ExecuteQuery(query.str().c_str()))
      {
      db.GetLog()->Error("Failed to insert uuid record for "
        + m_Bitstream->GetName());
      db.Database->ExecuteQuery("ROLLBACK");
      db.Close();
      return false;
      }

    // Add parent relationship
    query.str(std::string());
    query << "INSERT INTO item2bitstream (item_id, bitstream_id) VALUES ('" <<
      m_Bitstream->GetParentId() << "', '" << m_Bitstream->GetId() << "')";
    if(!db.Database->ExecuteQuery(query.str().c_str()))
      {
      db.GetLog()->Error("Failed to create parent relationship for bitstream");
      db.Database->ExecuteQuery("ROLLBACK");
      db.Close();
      return false;
      }
    }

  // Bitstreams added/changed on the client side should be marked as dirty
  if(m_MarkDirty)
    {
    query.str(std::string());
    query << "DELETE FROM dirty_resource WHERE uuid='" <<
      m_Bitstream->GetUuid() << "'";
    if(!db.Database->ExecuteQuery(query.str().c_str()))
      {
      db.GetLog()->Error("Failed to delete duplicate dirty records");
      db.Database->ExecuteQuery("ROLLBACK");
      db.Close();
      return false;
      }
    query.str(std::string());
    query << "INSERT INTO dirty_resource (uuid, action) VALUES ('" <<
      m_Bitstream->GetUuid() << "', '" << action << "')";
    if(!db.Database->ExecuteQuery(query.str().c_str()))
      {
      db.GetLog()->Error("Failed to mark bitstream as dirty");
      db.Database->ExecuteQuery("ROLLBACK");
      db.Close();
      return false;
      }
    }
  db.Database->ExecuteQuery("COMMIT");
  db.Close();
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

  query.str(std::string());
  query << "DELETE FROM item2bitstream WHERE bitstream_id='" <<
    m_Bitstream->GetId() << "'";
  if(!db.Database->ExecuteQuery(query.str().c_str()))
    {
    db.Database->ExecuteQuery("ROLLBACK");
    db.Database->Close();
    return false;
    }

  query.str(std::string());
  query << "DELETE FROM partial_upload WHERE bitstream_id='" <<
    m_Bitstream->GetId() << "'";
  if(!db.Database->ExecuteQuery(query.str().c_str()))
    {
    db.Database->ExecuteQuery("ROLLBACK");
    db.Database->Close();
    return false;
    }

  query.str(std::string());
  query << "DELETE FROM dirty_resource WHERE uuid='" <<
    m_Bitstream->GetUuid() << "'";
  if(!db.Database->ExecuteQuery(query.str().c_str()))
    {
    db.Database->ExecuteQuery("ROLLBACK");
    db.Database->Close();
    return false;
    }

  query.str(std::string());
  query << "DELETE FROM resource_uuid WHERE uuid='" <<
    m_Bitstream->GetUuid() << "'";
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
  m_Bitstream = reinterpret_cast<mdo::Bitstream*>(object);
}

void Bitstream::ParentPathChanged(std::string parentPath)
{
  std::string newPath = parentPath + "/" + m_Bitstream->GetName();
  std::stringstream query;
  query << "UPDATE resource_uuid SET path='" << newPath << "' WHERE "
    "uuid='" << m_Bitstream->GetUuid() << "'";

  mds::DatabaseAPI db;
  db.Open();
  db.Database->ExecuteQuery(query.str().c_str());
  db.Close();
}

} // end namespace
