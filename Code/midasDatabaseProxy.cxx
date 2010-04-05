/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "midasDatabaseProxy.h"

#include "mdoCommunity.h"

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
  std::string path, std::string name, int parentType, int parentId,
  int serverParent)
{
  return this->AddResource(type, uuid, path, name, 
    this->GetUuid(parentType, parentId), serverParent);
}

//-------------------------------------------------------------------------
int midasDatabaseProxy::AddResource(int type, std::string uuid,
  std::string path, std::string name, std::string parentUuid, int parentId)
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
      this->InsertResourceRecord(type, id, path, uuid, parentId);
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
    while(this->Database->GetNextRow());
    return true;
    }
  else
    {
    return false;
    }
}

//-------------------------------------------------------------------------
std::vector<std::string> midasDatabaseProxy::GetAuthProfiles()
{
  this->Database->ExecuteQuery("SELECT profile_name FROM auth_profile");
  std::vector<std::string> profileNames;
  while(this->Database->GetNextRow())
    {
    profileNames.push_back(this->Database->GetValueAsString(0));
    }
  return profileNames;
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
  this->Database->Close();
  this->Database->Open(this->DatabasePath.c_str());
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
midasResourceRecord midasDatabaseProxy::GetRecordByUuid(std::string uuid)
{
  std::stringstream query;
  query << "SELECT resource_type_id, resource_id, server_parent, path FROM "
    "resource_uuid WHERE uuid='" << uuid << "'";
  bool ok = this->Database->ExecuteQuery(query.str().c_str());
  midasResourceRecord record;

  if(this->Database->GetNextRow())
    {
    record.Type = this->Database->GetValueAsInt(0);
    record.Id = this->Database->GetValueAsInt(1);
    record.Parent = this->Database->GetValueAsInt(2);
    record.Path = this->Database->GetValueAsString(3);
    }
  return record;
}

//-------------------------------------------------------------------------
bool midasDatabaseProxy::AddChild(std::string parentUuid,
                                  int childType, int childId)
{
  midasResourceRecord record = this->GetRecordByUuid(parentUuid);
  return this->AddChild(record.Type, record.Id, childType, childId);
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
                                              std::string uuid, int parentId)
{
  std::stringstream query;
  query << "INSERT INTO resource_uuid (resource_type_id, resource_id, path, "
    "uuid, server_parent) VALUES ('" << type << "', '" << id << "', '"
    << path << "', '" << uuid << "', '" << parentId << "')";
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

//-------------------------------------------------------------------------
std::string midasDatabaseProxy::GetLastUsedURL()
{
  std::stringstream query;
  query << "SELECT url FROM last_url LIMIT 1";
  this->Database->ExecuteQuery(query.str().c_str());
  
  std::string url;
  while(this->Database->GetNextRow())
    {
    url = this->Database->GetValueAsString(0);
    }
  return url;
}

//-------------------------------------------------------------------------
void midasDatabaseProxy::SetLastUsedURL(std::string url)
{
  std::stringstream query;
  query << "DELETE FROM last_url";
  this->Database->ExecuteQuery(query.str().c_str());
  query.str(std::string());

  query << "INSERT INTO last_url (url) VALUES ('" << url << "')";
  this->Database->ExecuteQuery(query.str().c_str());
}

//--------------------------------------------------------------------------
std::vector<midasStatus> midasDatabaseProxy::GetStatusEntries()
{
  std::vector<midasStatus> statlist;
  this->Database->ExecuteQuery("SELECT uuid, action FROM dirty_resource");

  std::map<std::string, midasDirtyAction::Action> dirties;
  while(this->Database->GetNextRow())
    {
    dirties[this->Database->GetValueAsString(0)] =
      midasDirtyAction::Action(this->Database->GetValueAsInt(1));
    }

  for(std::map<std::string, midasDirtyAction::Action>::iterator i =
      dirties.begin(); i != dirties.end(); ++i)
    {
    midasResourceRecord record = this->GetRecordByUuid(i->first);

    midasResourceType::ResourceType rt =
      midasResourceType::ResourceType(record.Type);
    std::string name = this->GetName(record.Type, record.Id);

    midasStatus status(i->first, name, i->second, rt, record.Path);
    statlist.push_back(status);
    }

  return statlist;
}

//--------------------------------------------------------------------------
std::vector<mdo::Community*> midasDatabaseProxy::GetTopLevelCommunities(bool buildTree)
{
  std::vector<mdo::Community*> communities;
  this->Database->ExecuteQuery("SELECT community_id, name FROM community "
    "community WHERE community.community_id NOT IN (SELECT child_comm_id "
    "FROM community2community)");

  while(this->Database->GetNextRow())
    {
    mdo::Community* community = new mdo::Community;
    community->SetId(this->Database->GetValueAsInt(0));
    community->SetName(this->Database->GetValueAsString(1));
    communities.push_back(community);
    }

  for(std::vector<mdo::Community*>::iterator i = communities.begin();
      i != communities.end(); ++i)
    {

    }
  return communities;
}
