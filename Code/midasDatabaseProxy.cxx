/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "midasDatabaseProxy.h"

#include "mdoCommunity.h"
#include "mdoCollection.h"
#include "mdoItem.h"
#include "mdoBitstream.h"
#include "mdsCommunity.h"

midasDatabaseProxy::midasDatabaseProxy(std::string database)
{
  this->Database = new mds::SQLiteDatabase();
  this->DatabasePath = database;
}

midasDatabaseProxy::~midasDatabaseProxy()
{
  delete this->Database;
}

//-------------------------------------------------------------------------
std::string midasDatabaseProxy::GetKeyName(MidasAppSetting setting)
{
  switch(setting)
    {
    case LAST_URL:
      return "last_url";
    case LAST_FETCH_TIME:
      return "last_fetch";
    case AUTO_REFRESH_INTERVAL:
      return "refresh_interval";
    case AUTO_REFRESH_SETTING:
      return "refresh_setting";
    case ROOT_DIR:
      return "root_dir";
    case UNIFIED_TREE:
      return "unified_tree";
    default:
      return "";
    }
}

//-------------------------------------------------------------------------
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
  name = midasUtils::EscapeForSQL(name);

  if(parentUuid == "" && type != midasResourceType::COMMUNITY)
    {
    return -1;
    }
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
    return this->GetRecordByUuid(uuid).Id;
    }
}

//-------------------------------------------------------------------------
midasAuthProfile midasDatabaseProxy::GetAuthProfile(std::string name)
{
  std::stringstream query;
  query << "SELECT eperson, apikey, app_name, url, root_dir FROM auth_profile"
    " WHERE profile_name='" << name << "'";

  midasAuthProfile profile;
  this->Database->Open(this->DatabasePath.c_str());
  this->Database->ExecuteQuery(query.str().c_str());
  if(this->Database->GetNextRow())
    {
    profile.Name = name;
    profile.User = this->Database->GetValueAsString(0);
    profile.ApiKey = this->Database->GetValueAsString(1);
    profile.AppName = this->Database->GetValueAsString(2);
    profile.Url = this->Database->GetValueAsString(3);
    profile.RootDir = this->Database->GetValueAsString(4);
    while(this->Database->GetNextRow());
    }
  this->Database->Close();
  return profile;
}

//-------------------------------------------------------------------------
std::vector<std::string> midasDatabaseProxy::GetAuthProfiles()
{
  this->Database->Open(this->DatabasePath.c_str());
  this->Database->ExecuteQuery("SELECT profile_name FROM auth_profile");
  std::vector<std::string> profileNames;
  while(this->Database->GetNextRow())
    {
    profileNames.push_back(this->Database->GetValueAsString(0));
    }
  this->Database->Close();
  return profileNames;
}

//-------------------------------------------------------------------------
bool midasDatabaseProxy::AddAuthProfile(std::string user, std::string appName,
                                        std::string apiKey, std::string name,
                                        std::string rootDir, std::string url)
{
  this->DeleteProfile(name);
  this->Database->Open(this->DatabasePath.c_str());
  std::stringstream query;
  query << "INSERT INTO auth_profile (profile_name, eperson, apikey, app_name,"
    " root_dir, url) VALUES ('" << name << "', '" << user << "', '" << apiKey
    << "', '" << appName << "', '" << rootDir << "', '" << url << "')";

  bool ok = this->Database->ExecuteQuery(query.str().c_str());
  this->Database->Close();
  return ok;
}

//-------------------------------------------------------------------------
void midasDatabaseProxy::DeleteProfile(std::string name)
{
  this->Database->Open(this->DatabasePath.c_str());
  std::stringstream query;
  query << "DELETE FROM auth_profile WHERE profile_name = '" << name << "'";
  this->Database->ExecuteQuery(query.str().c_str());
  this->Database->Close();
}

//-------------------------------------------------------------------------
void midasDatabaseProxy::MarkDirtyResource(std::string uuid, int dirtyAction)
{
  // Clear old dirty flags so that we don't have duplicates
  this->ClearDirtyResource(uuid);
  this->Database->Open(this->DatabasePath.c_str());
  std::stringstream query;
  query << "INSERT INTO dirty_resource (uuid, action) VALUES ('" << uuid
    << "', '" << dirtyAction << "')";
  this->Database->ExecuteQuery(query.str().c_str());
  this->Database->Close();
}

//-------------------------------------------------------------------------
void midasDatabaseProxy::ClearDirtyResource(std::string uuid)
{
  this->Database->Open(this->DatabasePath.c_str());
  std::stringstream query;
  query << "DELETE FROM dirty_resource WHERE uuid='" << uuid << "'";
  this->Database->ExecuteQuery(query.str().c_str());
  this->Database->Close();
}

//-------------------------------------------------------------------------
bool midasDatabaseProxy::IsResourceDirty(std::string uuid)
{
  std::stringstream query;
  query << "SELECT uuid FROM dirty_resource WHERE uuid='"
    << uuid << "'";
  this->Database->Open(this->DatabasePath.c_str());
  this->Database->ExecuteQuery(query.str().c_str());

  bool dirty = this->Database->GetNextRow();
  while(this->Database->GetNextRow());
  this->Database->Close();
  return dirty;
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
  this->Database->Open(this->DatabasePath.c_str());
  this->Database->ExecuteQuery(query.str().c_str());
  std::string name = (this->Database->GetNextRow() ? 
    this->Database->GetValueAsString(0) : "");
  while(this->Database->GetNextRow());
  this->Database->Close();
  return name;
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
  this->Database->Open(this->DatabasePath.c_str());
  this->Database->ExecuteQuery(query.str().c_str());
  int parentId = (this->Database->GetNextRow() ? 
    this->Database->GetValueAsInt(0) : 0);
  while(this->Database->GetNextRow());
  this->Database->Close();
  return parentId;
}

//-------------------------------------------------------------------------
std::string midasDatabaseProxy::GetUuid(int type, int id)
{
  std::stringstream query;
  query << "SELECT uuid FROM resource_uuid WHERE resource_type_id='" 
    << type << "' AND resource_id='" << id << "'";
  this->Database->Open(this->DatabasePath.c_str());
  this->Database->ExecuteQuery(query.str().c_str());

  std::string uuid = this->Database->GetNextRow() ? 
    this->Database->GetValueAsString(0) : "";
  while(this->Database->GetNextRow());
  this->Database->Close();
  return uuid;
}

//-------------------------------------------------------------------------
midasResourceRecord midasDatabaseProxy::GetRecordByUuid(std::string uuid)
{
  std::stringstream query;
  query << "SELECT resource_type_id, resource_id, server_parent, path FROM "
    "resource_uuid WHERE uuid='" << uuid << "'";
  this->Database->Open(this->DatabasePath.c_str());
  bool ok = this->Database->ExecuteQuery(query.str().c_str());
  midasResourceRecord record;
  record.Uuid = uuid;

  if(this->Database->GetNextRow())
    {
    record.Type = this->Database->GetValueAsInt(0);
    record.Id = this->Database->GetValueAsInt(1);
    record.Parent = this->Database->GetValueAsInt(2);
    record.Path = this->Database->GetValueAsString(3);
    }
  while(this->Database->GetNextRow());
  this->Database->Close();
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
  this->Database->Open(this->DatabasePath.c_str());
  bool ok = this->Database->ExecuteQuery(query.str().c_str());
  this->Database->Close();
  return ok;
}

//-------------------------------------------------------------------------
int midasDatabaseProxy::InsertBitstream(std::string path, std::string name)
{
  std::stringstream query;
  query << "INSERT INTO bitstream (location, internal_id, name) VALUES ('1','"
    << path << "', '" << name << "')";
  this->Database->Open(this->DatabasePath.c_str());
  this->Database->ExecuteQuery(query.str().c_str());
  int id = this->Database->GetLastInsertId();
  this->Database->Close();
  return id;
}

//-------------------------------------------------------------------------
int midasDatabaseProxy::InsertCollection(std::string name)
{
  std::stringstream query;
  query << "INSERT INTO collection (name) VALUES ('" << name << "')";
  this->Database->Open(this->DatabasePath.c_str());
  this->Database->ExecuteQuery(query.str().c_str());
  int id = this->Database->GetLastInsertId();
  this->Database->Close();
  return id;
}

//-------------------------------------------------------------------------
int midasDatabaseProxy::InsertCommunity(std::string name)
{
  std::stringstream query;
  query << "INSERT INTO community (name) VALUES ('" << name << "')";
  this->Database->Open(this->DatabasePath.c_str());
  this->Database->ExecuteQuery(query.str().c_str());
  return this->Database->GetLastInsertId();
}

//-------------------------------------------------------------------------
int midasDatabaseProxy::InsertItem(std::string name)
{
  std::stringstream query;
  query << "INSERT INTO item (title) VALUES ('" << name << "')";
  this->Database->Open(this->DatabasePath.c_str());
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
  this->Database->Open(this->DatabasePath.c_str());
  this->Database->ExecuteQuery(query.str().c_str());
  this->Database->Close();
}

//-------------------------------------------------------------------------
bool midasDatabaseProxy::ResourceExists(std::string uuid)
{
  std::stringstream query;
  query << "SELECT * FROM resource_uuid WHERE uuid='" << uuid << "'";
  this->Database->Open(this->DatabasePath.c_str());
  this->Database->ExecuteQuery(query.str().c_str());
  
  bool exists = this->Database->GetNextRow();
  while(this->Database->GetNextRow());
  this->Database->Close();
  return exists;
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
  this->Database->Open(this->DatabasePath.c_str());
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
  this->Database->ExecuteQuery("DELETE FROM metadatavalue");
  //this->Database->ExecuteQuery("DELETE FROM app_settings");
  this->Database->Close();
}

//-------------------------------------------------------------------------
std::string midasDatabaseProxy::GetSetting(MidasAppSetting setting)
{
  std::string key = this->GetKeyName(setting);

  std::stringstream query;
  query << "SELECT value FROM app_settings WHERE key='" << key << "' LIMIT 1";
  this->Database->Open(this->DatabasePath.c_str());
  this->Database->ExecuteQuery(query.str().c_str());
  
  std::string value;
  while(this->Database->GetNextRow())
    {
    value = this->Database->GetValueAsString(0);
    }
  this->Database->Close();
  return value;
}

int midasDatabaseProxy::GetSettingInt(MidasAppSetting setting)
{
  std::string val = this->GetSetting(setting);
  return atoi(val.c_str());
}

bool midasDatabaseProxy::GetSettingBool(MidasAppSetting setting)
{
  std::string val = this->GetSetting(setting);
  return atoi(val.c_str()) != 0;
}

//-------------------------------------------------------------------------
void midasDatabaseProxy::SetSetting(MidasAppSetting setting, std::string value)
{
  std::string key = this->GetKeyName(setting);

  std::stringstream query;
  query << "DELETE FROM app_settings WHERE key='" << key << "'";
  this->Database->Open(this->DatabasePath.c_str());
  this->Database->ExecuteQuery(query.str().c_str());
  query.str(std::string());

  query << "INSERT INTO app_settings (key, value) VALUES ('" << key << "', '"
    << value << "')";
  this->Database->ExecuteQuery(query.str().c_str());
  this->Database->Close();
}

void midasDatabaseProxy::SetSetting(MidasAppSetting setting, int value)
{
  std::stringstream s;
  s << value;
  this->SetSetting(setting, s.str());
}

void midasDatabaseProxy::SetSetting(MidasAppSetting setting, bool value)
{
  this->SetSetting(setting, value ? 1 : 0);
}

//--------------------------------------------------------------------------
std::vector<midasStatus> midasDatabaseProxy::GetStatusEntries()
{
  std::vector<midasStatus> statlist;
  this->Database->Open(this->DatabasePath.c_str());
  this->Database->ExecuteQuery("SELECT uuid, action FROM dirty_resource");

  std::vector<std::string> uuids;
  std::vector<midasDirtyAction::Action> actions;
  
  while(this->Database->GetNextRow())
    {
    uuids.push_back(this->Database->GetValueAsString(0));
    actions.push_back(midasDirtyAction::Action(
      this->Database->GetValueAsInt(1)));
    }
  this->Database->Close();

  for(size_t i = 0; i < uuids.size(); i++)
    {
    midasResourceRecord record = this->GetRecordByUuid(uuids[i]);
    std::string name = this->GetName(record.Type, record.Id);

    midasStatus status(uuids[i], name, actions[i],
      midasResourceType::ResourceType(record.Type), record.Path);

    statlist.push_back(status);
    }
  return statlist;
}

//--------------------------------------------------------------------------
std::vector<mdo::Community*> midasDatabaseProxy::GetTopLevelCommunities(
                                                            bool buildTree)
{
  std::vector<mdo::Community*> communities;
  std::stringstream query;
  query << "SELECT resource_uuid.uuid, community.community_id, community.name "
    "FROM community, resource_uuid WHERE resource_uuid.resource_id=community.community_id AND "
    "resource_uuid.resource_type_id='" << midasResourceType::COMMUNITY << "' AND community.community_id "
    "NOT IN (SELECT child_comm_id FROM community2community)";

  this->Database->Open(this->DatabasePath.c_str());
  this->Database->ExecuteQuery(query.str().c_str());

  while(this->Database->GetNextRow())
    {
    mdo::Community* community = new mdo::Community;
    community->SetUuid(this->Database->GetValueAsString(0));
    community->SetId(this->Database->GetValueAsInt(1));
    community->SetName(this->Database->GetValueAsString(2));
    communities.push_back(community);
    }
  this->Database->Close();

  if(buildTree)
    {
    for(std::vector<mdo::Community*>::iterator i = communities.begin();
        i != communities.end(); ++i)
      {
      mds::Community mdsComm;
      mdsComm.SetObject(*i);
      mdsComm.SetDatabase(this);
      mdsComm.FetchTree();
      }
    }
  return communities;
}

//--------------------------------------------------------------------------
bool midasDatabaseProxy::DeleteResource(std::string uuid, bool deleteFiles)
{
  midasResourceRecord record = this->GetRecordByUuid(uuid);

  if(!record.Id) return false;

  std::stringstream query;
  std::vector<int> children;

  switch(record.Type)
    {
    case midasResourceType::COMMUNITY:
      query << "SELECT child_comm_id FROM community2community WHERE "
        "parent_comm_id='" << record.Id << "'";
      this->Database->Open(this->DatabasePath.c_str());
      this->Database->ExecuteQuery(query.str().c_str());

      while(this->Database->GetNextRow())
        {
        children.push_back(this->Database->GetValueAsInt(0));
        }
      this->Database->Close();
      for(std::vector<int>::iterator i = children.begin();
          i != children.end(); ++i)
        {
        this->DeleteResource(this->GetUuid(midasResourceType::COMMUNITY, *i));
        }

      query.str(std::string());
      query << "SELECT collection_id FROM community2collection WHERE "
        "community_id='" << record.Id << "'";
      this->Database->Open(this->DatabasePath.c_str());
      this->Database->ExecuteQuery(query.str().c_str());

      children.clear();
      while(this->Database->GetNextRow())
        {
        children.push_back(this->Database->GetValueAsInt(0)); 
        }
      this->Database->Close();
      for(std::vector<int>::iterator i = children.begin();
          i != children.end(); ++i)
        {
        this->DeleteResource(this->GetUuid(midasResourceType::COLLECTION, *i));
        }

      query.str(std::string());
      query << "DELETE FROM community2community WHERE parent_comm_id='" <<
        record.Id << "'";
      this->Database->Open(this->DatabasePath.c_str());
      this->Database->ExecuteQuery(query.str().c_str());

      query.str(std::string());
      query << "DELETE FROM community2collection WHERE community_id='" <<
        record.Id << "'";
      this->Database->ExecuteQuery(query.str().c_str());

      query.str(std::string());
      query << "DELETE FROM community WHERE community_id='" <<
        record.Id << "'";
      this->Database->ExecuteQuery(query.str().c_str());
      this->Database->Close();
      break;
    case midasResourceType::COLLECTION:
      query << "SELECT item_id FROM collection2item WHERE "
        "collection_id='" << record.Id << "'";
      this->Database->Open(this->DatabasePath.c_str());
      this->Database->ExecuteQuery(query.str().c_str());

      while(this->Database->GetNextRow())
        {
        children.push_back(this->Database->GetValueAsInt(0));
        }
      this->Database->Close();
      for(std::vector<int>::iterator i = children.begin();
          i != children.end(); ++i)
        {
        this->DeleteResource(this->GetUuid(midasResourceType::ITEM, *i));
        }

      query.str(std::string());
      query << "DELETE FROM collection2item WHERE collection_id='" <<
        record.Id << "'";
      this->Database->Open(this->DatabasePath.c_str());
      this->Database->ExecuteQuery(query.str().c_str());

      query.str(std::string());
      query << "DELETE FROM collection WHERE collection_id='" <<
        record.Id << "'";
      this->Database->ExecuteQuery(query.str().c_str());
      this->Database->Close();
      break;
    case midasResourceType::ITEM:
      query << "SELECT bitstream_id FROM item2bitstream WHERE "
        "item_id='" << record.Id << "'";
      this->Database->Open(this->DatabasePath.c_str());
      this->Database->ExecuteQuery(query.str().c_str());

      while(this->Database->GetNextRow())
        {
        children.push_back(this->Database->GetValueAsInt(0));
        }
      this->Database->Close();
      for(std::vector<int>::iterator i = children.begin();
          i != children.end(); ++i)
        {
        this->DeleteResource(this->GetUuid(midasResourceType::BITSTREAM, *i));
        }

      query.str(std::string());
      query << "DELETE FROM item2bitstream WHERE item_id='" <<
        record.Id << "'";
      this->Database->Open(this->DatabasePath.c_str());
      this->Database->ExecuteQuery(query.str().c_str());

      query.str(std::string());
      query << "DELETE FROM item WHERE item_id='" << record.Id << "'";
      this->Database->ExecuteQuery(query.str().c_str());
      this->Database->Close();
      break;
    case midasResourceType::BITSTREAM:
      query << "DELETE FROM bitstream WHERE bitstream_id='" << record.Id << "'";
      this->Database->Open(this->DatabasePath.c_str());
      this->Database->ExecuteQuery(query.str().c_str());
      this->Database->Close();
      break;
    default:
      return false;
    }

  query.str(std::string());
  query << "DELETE FROM dirty_resource WHERE uuid='" << uuid << "'";
  this->Database->Open(this->DatabasePath.c_str());
  this->Database->ExecuteQuery(query.str().c_str());

  query.str(std::string());
  query << "DELETE FROM resource_uuid WHERE uuid='" << uuid << "'";
  this->Database->Open(this->DatabasePath.c_str());
  this->Database->ExecuteQuery(query.str().c_str());
  this->Database->Close();

  if(deleteFiles)
    {
    if(record.Type == midasResourceType::BITSTREAM)
      {
      kwsys::SystemTools::RemoveFile(record.Path.c_str());
      }
    else
      {
      kwsys::SystemTools::RemoveADirectory(record.Path.c_str());
      }
    }
  return true;
}

//--------------------------------------------------------------------------
void midasDatabaseProxy::UnifyTree()
{
  std::vector<mdo::Community*> topLevel = this->GetTopLevelCommunities(true);

  for(std::vector<mdo::Community*>::iterator i = topLevel.begin();
      i != topLevel.end(); ++i)
    {
    this->MergeOnDisk(*i);
    }
}

//--------------------------------------------------------------------------
void midasDatabaseProxy::MergeOnDisk(mdo::Community* comm)
{
  std::vector<mdo::Community*>::const_iterator i =
    comm->GetCommunities().begin();
  for(; i != comm->GetCommunities().end(); ++i)
    {
    //TODO copy community directory under parent
    MergeOnDisk(*i);
    }

  std::vector<mdo::Collection*>::const_iterator j =
    comm->GetCollections().begin();
  for(; j != comm->GetCollections().end(); ++j)
    {
    //TODO copy collection directory under parent
    MergeOnDisk(*j);
    }
}

//--------------------------------------------------------------------------
void midasDatabaseProxy::MergeOnDisk(mdo::Collection* coll)
{
  std::vector<mdo::Item*>::const_iterator i =
    coll->GetItems().begin();
  for(; i != coll->GetItems().end(); ++i)
    {
    MergeOnDisk(*i);
    }
}

//--------------------------------------------------------------------------
void midasDatabaseProxy::MergeOnDisk(mdo::Item* item)
{
  std::string itemPath = this->GetRecordByUuid(item->GetUuid()).Path;
  kwsys::SystemTools::ConvertToUnixSlashes(itemPath);

  for(std::vector<mdo::Bitstream*>::const_iterator i =
      item->GetBitstreams().begin(); i != item->GetBitstreams().end(); ++i)
    {
    std::string path = this->GetRecordByUuid((*i)->GetUuid()).Path;
    std::string copyTo = itemPath + "/" + midasUtils::EscapeName(
      (*i)->GetName());
    kwsys::SystemTools::CopyAFile(path.c_str(),
      copyTo.c_str());

    if(!kwsys::SystemTools::ComparePath(path.c_str(), copyTo.c_str()))
      {
      std::stringstream query;
      query << "UPDATE resource_uuid SET path='" << copyTo <<
        "' WHERE uuid='" << (*i)->GetUuid() << "'";
      this->Database->Open(this->DatabasePath.c_str());
      this->Database->ExecuteQuery(query.str().c_str());
      this->Database->Close();
      }
    }
}

//--------------------------------------------------------------------------
std::vector<mdo::Object*> midasDatabaseProxy::Search(
                                              std::vector<std::string> tokens)
{
  std::vector<mdo::Object*> results;

  //TODO implement search

  return results;
}

//--------------------------------------------------------------------------
std::string midasDatabaseProxy::GetUuidFromPath(std::string path)
{
  // First make sure we use the absolute path
  kwsys::SystemTools::ConvertToUnixSlashes(path);
  if(!kwsys::SystemTools::FileIsFullPath(path.c_str()))
    {
    path = kwsys::SystemTools::GetCurrentWorkingDirectory()
      + "/" + path;
    }
  std::string uuid = "";
  std::stringstream query;
  query << "SELECT uuid FROM resource_uuid WHERE path='" << path << "'";
  this->Database->Open(this->DatabasePath.c_str());
  this->Database->ExecuteQuery(query.str().c_str());
  while(this->Database->GetNextRow())
    {
    uuid = this->Database->GetValueAsString(0);
    }
  this->Database->Close();

  return uuid;
}

bool midasDatabaseProxy::CheckModifiedBitstreams()
{
  std::stringstream query;
  query << "SELECT bitstream_id, last_modified, internal_id FROM bitstream "
        << "WHERE location='1'";
  this->Database->Open(this->DatabasePath.c_str());
  this->Database->ExecuteQuery(query.str().c_str());

  std::vector<midasBitstreamStamp> bitstreams;

  while(this->Database->GetNextRow())
    {
    midasBitstreamStamp stamp;
    stamp.Id = this->Database->GetValueAsInt(0);
    stamp.LastModified = this->Database->GetValueAsInt(1);
    stamp.Path = this->Database->GetValueAsString(2);
    bitstreams.push_back(stamp);
    }
  this->Database->Close();

  bool status = false;
  for(std::vector<midasBitstreamStamp>::iterator i = bitstreams.begin();
      i != bitstreams.end(); ++i)
    {
    if(!kwsys::SystemTools::FileExists(i->Path.c_str(), true))
      {
      std::stringstream text;
      text << "Bitstream " << i->Path << " no longer exists in the filesystem."
        << " Removing it from local MIDAS control." << std::endl;
      this->Log->Message(text.str());
      this->Log->Status(text.str());
      this->DeleteResource(this->GetUuid(midasResourceType::BITSTREAM, i->Id));
      status = true;
      continue;
      }

    long int lastModified =
      kwsys::SystemTools::ModifiedTime(i->Path.c_str());
    if(lastModified != i->LastModified)
      {
      std::stringstream text;
      text << "Bitstream " << i->Path << " was modified on disk. "
        << "Marking resource as dirty." << std::endl;
      this->Log->Message(text.str());
      this->Log->Status(text.str());

      std::stringstream query;
      query << "UPDATE bitstream SET last_modified='" << lastModified
        << "' WHERE bitstream_id='" << i->Id << "'";
      this->Database->Open(this->DatabasePath.c_str());
      this->Database->ExecuteQuery(query.str().c_str());
      this->Database->Close();

      this->MarkDirtyResource(
        this->GetUuid(midasResourceType::BITSTREAM, i->Id),
        midasDirtyAction::MODIFIED);
      }
    }
  return status;
}