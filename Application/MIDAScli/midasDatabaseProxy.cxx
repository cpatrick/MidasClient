/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "midasDatabaseProxy.h"

midasDatabaseProxy::midasDatabaseProxy(std::string database)
{
  this->Database = new mds::SQLiteDatabase();
  this->DatabasePath = database;
}

midasDatabaseProxy::~midasDatabaseProxy()
{
  delete this->Database;
}

mds::SQLiteDatabase* midasDatabaseProxy::GetDatabase()
{
  return this->Database;
}

//-------------------------------------------------------------------------
int midasDatabaseProxy::AddResource(int type, std::string uuid,
  std::string path, std::string name, int parentType, int parentId)
{
  return this->AddResource(type, uuid, path, name, 
    this->GetUuid(parentType, parentId));
}

//-------------------------------------------------------------------------
int midasDatabaseProxy::AddResource(int type, std::string uuid,
  std::string path, std::string name, std::string parentUuid)
{
  if(!this->ResourceExists(uuid))
    {
    int id = -1;
    switch(type)
      {
      case midasResourceType::BITSTREAM:
        id = this->InsertBitstream(path, name);
        break;
      case midasResourceType::COLLECTION:
        id = this->InsertCollection(name);
        break;
      case midasResourceType::COMMUNITY:
        id = this->InsertCommunity(name);
        break;
      case midasResourceType::ITEM:
        id = this->InsertItem(name);
        break;
      default:
        break;
      }
    if(id > 0)
      {
      this->InsertResourceRecord(type, id, path, uuid);
      if(parentUuid != "")
        {
        this->AddChild(parentUuid, type, id);
        }
      }
    return id;
    }
  else
    {
    return this->GetIdForUuid(uuid);
    }
}

//-------------------------------------------------------------------------
bool midasDatabaseProxy::GetAuthProfile(std::string name, std::string& user,
                                        std::string& appName,
                                        std::string& apiKey)
{
  std::stringstream query;
  query << "SELECT eperson, apikey, app_name FROM auth_profile WHERE "
    "profile_name='" << name << "'";

  this->Database->ExecuteQuery(query.str().c_str());
  if(this->Database->GetNextRow())
    {
    user = this->Database->GetValueAsString(0);
    apiKey = this->Database->GetValueAsString(1);
    appName = this->Database->GetValueAsString(2);
    return true;
    }
  else
    {
    return false;
    }
}

//-------------------------------------------------------------------------
bool midasDatabaseProxy::AddAuthProfile(std::string user, std::string appName,
                                        std::string apiKey, std::string name)
{
  std::stringstream query;
  query << "INSERT INTO auth_profile (profile_name, eperson, apikey, "
    "app_name) VALUES ('" << name << "', '" << user << "', '" << apiKey <<
    "', '" << appName << "')";

  return this->Database->ExecuteQuery(query.str().c_str());
}

//-------------------------------------------------------------------------
void midasDatabaseProxy::MarkDirtyResource(std::string uuid, int dirtyAction)
{
  // Clear old dirty flags so that we don't have duplicates
  this->ClearDirtyResource(uuid);

  std::stringstream query;
  query << "INSERT INTO dirty_resource (uuid, action) VALUES ('" << uuid
    << "', '" << dirtyAction << "')";
  this->Database->ExecuteQuery(query.str().c_str());
}

//-------------------------------------------------------------------------
void midasDatabaseProxy::ClearDirtyResource(std::string uuid)
{
  std::stringstream query;
  query << "DELETE FROM dirty_resource WHERE uuid='" << uuid << "'";
  this->Database->ExecuteQuery(query.str().c_str());
}

//-------------------------------------------------------------------------
int midasDatabaseProxy::IsResourceDirty(std::string uuid)
{
  std::stringstream query;
  query << "SELECT action FROM dirty_resource WHERE uuid='"
    << uuid << "'";
  this->Database->ExecuteQuery(query.str().c_str());

  return this->Database->GetNextRow() ? this->Database->GetValueAsInt(0) : 0;
}

//-------------------------------------------------------------------------
std::string midasDatabaseProxy::GetName(int type, int id)
{
  std::stringstream query;
  
  switch(type)
    {
    case midasResourceType::BITSTREAM:
      query << "SELECT name FROM bitstream WHERE bitstream_id='" << id << "'";
      break;
    case midasResourceType::COMMUNITY:
      query << "SELECT name FROM community WHERE community_id='" << id << "'";
      break;
    case midasResourceType::COLLECTION:
      query << "SELECT name FROM collection WHERE collection_id='"
        << id << "'";
      break;
    case midasResourceType::ITEM:
      query << "SELECT title FROM item WHERE item_id='" << id << "'";
      break;
    default:
      return "";
    }
  this->Database->ExecuteQuery(query.str().c_str());
  return (this->Database->GetNextRow() ? 
    this->Database->GetValueAsString(0) : "");
}

//-------------------------------------------------------------------------
int midasDatabaseProxy::GetParentId(int type, int id)
{
  std::stringstream query;
  
  switch(type)
    {
    case midasResourceType::BITSTREAM:
      query << "SELECT item_id FROM item2bitstream WHERE bitstream_id='"
        << id << "'";
      break;
    case midasResourceType::COMMUNITY:
      query << "SELECT parent_comm_id FROM community2community WHERE "
        "child_comm_id='" << id << "'";
      break;
    case midasResourceType::COLLECTION:
      query << "SELECT community_id FROM community2collection WHERE "
        "collection_id='" << id << "'";
      break;
    case midasResourceType::ITEM:
      query << "SELECT collection_id FROM collection2item WHERE item_id='"
        << id << "'";
      break;
    default:
      return 0;
    }
  this->Database->ExecuteQuery(query.str().c_str());
  return (this->Database->GetNextRow() ? 
    this->Database->GetValueAsInt(0) : 0);
}

//-------------------------------------------------------------------------
int midasDatabaseProxy::GetIdForUuid(std::string uuid)
{
  std::stringstream query;
  query << "SELECT resource_id FROM resource_uuid WHERE uuid='"
    << uuid << "'";
  this->Database->ExecuteQuery(query.str().c_str());

  return this->Database->GetNextRow() ? this->Database->GetValueAsInt(0) : -1;
}

//-------------------------------------------------------------------------
std::string midasDatabaseProxy::GetUuidFromPath(std::string path)
{
  std::stringstream query;
  query << "SELECT uuid FROM resource_uuid WHERE path='" << path << "'";
  this->Database->ExecuteQuery(query.str().c_str());

  return this->Database->GetNextRow() ? 
    this->Database->GetValueAsString(0) : "";
}

//-------------------------------------------------------------------------
std::string midasDatabaseProxy::GetUuid(int type, int id)
{
  std::stringstream query;
  query << "SELECT uuid FROM resource_uuid WHERE resource_type_id='" 
    << type << "' AND resource_id='" << id << "'";
  this->Database->ExecuteQuery(query.str().c_str());

  return this->Database->GetNextRow() ? 
    this->Database->GetValueAsString(0) : "";
}

//-------------------------------------------------------------------------
void midasDatabaseProxy::GetTypeAndIdForUuid(std::string uuid, 
                                             int& type, int& id)
{
  std::stringstream query;
  query << "SELECT resource_type_id, resource_id FROM resource_uuid "
    "WHERE uuid='" << uuid << "'";
  this->Database->ExecuteQuery(query.str().c_str());

  if(this->Database->GetNextRow())
    {
    type = this->Database->GetValueAsInt(0);
    id = this->Database->GetValueAsInt(1);
    }
  else
    {
    type = midasResourceType::TYPE_ERROR;
    }
}

//-------------------------------------------------------------------------
bool midasDatabaseProxy::AddChild(std::string parentUuid,
                                  int childType, int childId)
{
  int parentType, parentId;
  this->GetTypeAndIdForUuid(parentUuid, parentType, parentId);
  return this->AddChild(parentType, parentId, childType, childId);
}

//-------------------------------------------------------------------------
bool midasDatabaseProxy::AddChild(int parentType, int parentId,
                                  int childType, int childId)
{
  std::stringstream query;
  query << "INSERT INTO ";
  std::string parent, child, parentCol, childCol;

  switch(parentType)
    {
    case midasResourceType::COLLECTION:
      parent = "collection";
      break;
    case midasResourceType::COMMUNITY:
      parent = "community";
      break;
    case midasResourceType::ITEM:
      parent = "item";
      break;
    default:
      return false;
    }

  switch(childType)
    {
    case midasResourceType::BITSTREAM:
      child = "bitstream";
      break;
    case midasResourceType::COLLECTION:
      child = "collection";
      break;
    case midasResourceType::COMMUNITY:
      child = "community";
      break;
    case midasResourceType::ITEM:
      child = "item";
      break;
    default:
      return false;
    }

  //special case for community2community
  if(parent == "community" && child == "community")
    {
    parentCol = "parent_comm";
    childCol = "child_comm";
    }
  else
    {
    parentCol = parent;
    childCol = child;
    }
  query << parent << "2" << child << " (" << parentCol << "_id, " << childCol
    << "_id) VALUES ('" << parentId << "', '" << childId << "')";
  return this->Database->ExecuteQuery(query.str().c_str());
}

//-------------------------------------------------------------------------
std::string midasDatabaseProxy::GetResourceLocation(std::string uuid)
{
  std::stringstream query;
  query << "SELECT path FROM resource_uuid WHERE uuid='" << uuid << "'";

  this->Database->ExecuteQuery(query.str().c_str());
  std::string result;

  if(this->Database->GetNextRow())
    {
    result = this->Database->GetValueAsString(0);
    }
  return result;
}

//-------------------------------------------------------------------------
int midasDatabaseProxy::InsertBitstream(std::string path, std::string name)
{
  std::stringstream query;
  query << "INSERT INTO bitstream (location, internal_id, name) VALUES ('1','"
    << path << "', '" << name << "')";
  this->Database->ExecuteQuery(query.str().c_str());
  return this->Database->GetLastInsertId();
}

//-------------------------------------------------------------------------
int midasDatabaseProxy::InsertCollection(std::string name)
{
  std::stringstream query;
  query << "INSERT INTO collection (name) VALUES ('" << name << "')";
  this->Database->ExecuteQuery(query.str().c_str());
  return this->Database->GetLastInsertId();
}

//-------------------------------------------------------------------------
int midasDatabaseProxy::InsertCommunity(std::string name)
{
  std::stringstream query;
  query << "INSERT INTO community (name) VALUES ('" << name << "')";
  this->Database->ExecuteQuery(query.str().c_str());
  return this->Database->GetLastInsertId();
}

//-------------------------------------------------------------------------
int midasDatabaseProxy::InsertItem(std::string name)
{
  std::stringstream query;
  query << "INSERT INTO item (title) VALUES ('" << name << "')";
  this->Database->ExecuteQuery(query.str().c_str());
  return this->Database->GetLastInsertId();
}

//-------------------------------------------------------------------------
void midasDatabaseProxy::InsertResourceRecord(int type, int id,
                                              std::string path,
                                              std::string uuid)
{
  std::stringstream query;
  query << "INSERT INTO resource_uuid (resource_type_id, resource_id, path, "
    "uuid) VALUES ('" << type << "', '" << id << "', '" << path << "', '" 
    << uuid << "')";
  this->Database->ExecuteQuery(query.str().c_str());
}

//-------------------------------------------------------------------------
bool midasDatabaseProxy::ResourceExists(std::string uuid)
{
  std::stringstream query;
  query << "SELECT * FROM resource_uuid WHERE uuid='" << uuid << "'";
  this->Database->ExecuteQuery(query.str().c_str());
  
  return this->Database->GetNextRow();
}

//-------------------------------------------------------------------------
bool midasDatabaseProxy::Open()
{
  return this->Database->Open(this->DatabasePath.c_str());
}

//-------------------------------------------------------------------------
bool midasDatabaseProxy::Close()
{
  return this->Database->Close();
}

//-------------------------------------------------------------------------
void midasDatabaseProxy::Clean()
{
  std::stringstream selectQuery;
  selectQuery << "SELECT path FROM resource_uuid;";
  this->Database->ExecuteQuery(selectQuery.str().c_str());

  while(this->Database->GetNextRow())
    {
    const char* path = this->Database->GetValueAsString(0);

    if(kwsys::SystemTools::FileExists(path))
      {
      if(kwsys::SystemTools::FileIsDirectory(path))
        {
        kwsys::SystemTools::RemoveADirectory(path);
        }
      else
        {
        kwsys::SystemTools::RemoveFile(path);
        }
      }
    }

  this->Database->ExecuteQuery("DELETE FROM resource_uuid");
  this->Database->ExecuteQuery("DELETE FROM dirty_resource");
  this->Database->ExecuteQuery("DELETE FROM bitstream");
  this->Database->ExecuteQuery("DELETE FROM collection");
  this->Database->ExecuteQuery("DELETE FROM community");
  this->Database->ExecuteQuery("DELETE FROM item");
  this->Database->ExecuteQuery("DELETE FROM community2community");
  this->Database->ExecuteQuery("DELETE FROM community2collection");
  this->Database->ExecuteQuery("DELETE FROM collection2item");
  this->Database->ExecuteQuery("DELETE FROM item2bitstream");
}
