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
#include "mdsCollection.h"
#include "mdsItem.h"
#include "mdsBitstream.h"

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
    "NOT IN (SELECT child_comm_id FROM community2community) ORDER BY community.name COLLATE NOCASE ASC";

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

  bool ok = true;
  mds::Community mdsComm;
  mds::Collection mdsColl;
  mds::Item mdsItem;
  mds::Bitstream mdsBitstream;
  mdo::Community* comm;
  mdo::Collection* coll;
  mdo::Item* item;
  mdo::Bitstream* bitstream;

  switch(record.Type)
    {
    case midasResourceType::COMMUNITY:
      comm = new mdo::Community;
      comm->SetId(record.Id);
      comm->SetUuid(uuid.c_str());
      mdsComm.SetObject(comm);
      mdsComm.SetDatabase(this);
      mdsComm.SetPath(record.Path);
      ok = mdsComm.Delete(deleteFiles);
      delete comm;
      break;
    case midasResourceType::COLLECTION:
      coll = new mdo::Collection;
      coll->SetId(record.Id);
      coll->SetUuid(uuid.c_str());
      mdsColl.SetObject(coll);
      mdsColl.SetDatabase(this);
      mdsColl.SetPath(record.Path);
      ok = mdsColl.Delete(deleteFiles);
      delete coll;
      break;
    case midasResourceType::ITEM:
      item = new mdo::Item;
      item->SetId(record.Id);
      item->SetUuid(uuid.c_str());
      mdsItem.SetObject(item);
      mdsItem.SetDatabase(this);
      mdsItem.SetPath(record.Path);
      ok = mdsItem.Delete(deleteFiles);
      delete item;
      break;
    case midasResourceType::BITSTREAM:
      bitstream = new mdo::Bitstream;
      bitstream->SetId(record.Id);
      bitstream->SetUuid(uuid.c_str());
      mdsBitstream.SetObject(bitstream);
      mdsBitstream.SetDatabase(this);
      mdsBitstream.SetPath(record.Path);
      ok = mdsBitstream.Delete(deleteFiles);
      delete bitstream;
      break;
    default:
      return false;
    }
  return ok;
}

//--------------------------------------------------------------------------
bool midasDatabaseProxy::UnifyTree()
{
  std::string rootDir = this->GetSetting(midasDatabaseProxy::ROOT_DIR);
  kwsys::SystemTools::ConvertToUnixSlashes(rootDir);
  rootDir = midasUtils::TrimTrailingSlash(rootDir);
  this->Log->Message("Copying all resources under " + rootDir);
  std::vector<mdo::Community*> topLevel = this->GetTopLevelCommunities(true);

  bool ok = true;
  for(std::vector<mdo::Community*>::iterator i = topLevel.begin();
      i != topLevel.end(); ++i)
    {
    ok &= this->Relocate(*i, rootDir);
    }
  return ok;
}

//--------------------------------------------------------------------------
bool midasDatabaseProxy::Relocate(mdo::Community* comm, std::string parentDir)
{
  std::string copyTo = parentDir + "/" + comm->GetName();
  
  std::stringstream text;
  text << "Relocating community to " << copyTo << std::endl;
  this->Log->Status(text.str());
  if(!kwsys::SystemTools::FileIsDirectory(copyTo.c_str())
     && !kwsys::SystemTools::MakeDirectory(copyTo.c_str()))
    {
    std::stringstream error;
    error << "Error: unable to create community directory at "
      << copyTo << std::endl;
    this->Log->Error(error.str());
    return false;
    }

  std::stringstream query;
  query << "UPDATE resource_uuid SET path='" << copyTo <<
    "' WHERE uuid='" << comm->GetUuid() << "'";
  this->Database->Open(this->DatabasePath.c_str());
  this->Database->ExecuteQuery(query.str().c_str());
  this->Database->Close();

  bool ok = true;
  for(std::vector<mdo::Community*>::const_iterator i =
      comm->GetCommunities().begin(); i != comm->GetCommunities().end(); ++i)
    {
    ok &= this->Relocate(*i, copyTo);
    }
  for(std::vector<mdo::Collection*>::const_iterator i =
      comm->GetCollections().begin(); i != comm->GetCollections().end(); ++i)
    {
    ok &= this->Relocate(*i, copyTo);
    }
  return ok;
}

//--------------------------------------------------------------------------
bool midasDatabaseProxy::Relocate(mdo::Collection* coll, std::string parentDir)
{
  std::string copyTo = parentDir + "/" + coll->GetName();
  
  std::stringstream text;
  text << "Relocating collection to " << copyTo << std::endl;
  this->Log->Status(text.str());
  if(!kwsys::SystemTools::FileIsDirectory(copyTo.c_str())
     && !kwsys::SystemTools::MakeDirectory(copyTo.c_str()))
    {
    std::stringstream error;
    error << "Error: unable to create collection directory at "
      << copyTo << std::endl;
    this->Log->Error(error.str());
    return false;
    }

  std::stringstream query;
  query << "UPDATE resource_uuid SET path='" << copyTo <<
    "' WHERE uuid='" << coll->GetUuid() << "'";
  this->Database->Open(this->DatabasePath.c_str());
  this->Database->ExecuteQuery(query.str().c_str());
  this->Database->Close();

  bool ok = true;
  for(std::vector<mdo::Item*>::const_iterator i = coll->GetItems().begin();
      i != coll->GetItems().end(); ++i)
    {
    ok &= this->Relocate(*i, copyTo);
    }
  return ok;
}

//--------------------------------------------------------------------------
bool midasDatabaseProxy::Relocate(mdo::Item* item, std::string parentDir)
{
  std::string copyTo = parentDir + "/" + item->GetName();
  
  std::stringstream text;
  text << "Relocating item to " << copyTo << std::endl;
  this->Log->Status(text.str());
  if(!kwsys::SystemTools::FileIsDirectory(copyTo.c_str())
     && !kwsys::SystemTools::MakeDirectory(copyTo.c_str()))
    {
    std::stringstream error;
    error << "Error: unable to create item directory at " << copyTo
      << std::endl;
    this->Log->Error(error.str());
    return false;
    }

  std::stringstream query;
  query << "UPDATE resource_uuid SET path='" << copyTo <<
    "' WHERE uuid='" << item->GetUuid() << "'";
  this->Database->Open(this->DatabasePath.c_str());
  this->Database->ExecuteQuery(query.str().c_str());
  this->Database->Close();

  bool ok = true;
  for(std::vector<mdo::Bitstream*>::const_iterator i =
      item->GetBitstreams().begin(); i != item->GetBitstreams().end(); ++i)
    {
    ok &= this->Relocate(*i, copyTo);
    }
  return ok;
}

//--------------------------------------------------------------------------
bool midasDatabaseProxy::Relocate(mdo::Bitstream* bitstream,
                                  std::string parentDir)
{
  std::string copyTo = parentDir + "/" + bitstream->GetName();
  std::string path = this->GetRecordByUuid(bitstream->GetUuid()).Path;
  if(kwsys::SystemTools::ComparePath(path.c_str(), copyTo.c_str()))
    {
    return true; //if the new path is the same as old one, don't copy
    }
  
  bitstream->SetFetched(false);
  std::stringstream text;
  text << "Relocating bitstream to " << copyTo << std::endl;
  this->Log->Status(text.str());
  if(!kwsys::SystemTools::CopyAFile(path.c_str(),
    copyTo.c_str()))
    {
    std::stringstream error;
    error << "Error: unable to copy bitstream to " << copyTo << std::endl;
    this->Log->Error(error.str());
    return false;
    }
  long lastModified = kwsys::SystemTools::ModifiedTime(copyTo.c_str());

  this->Database->Open(this->DatabasePath.c_str());
  this->Database->ExecuteQuery("BEGIN");

  std::stringstream query;
  query << "UPDATE resource_uuid SET path='" << copyTo <<
    "' WHERE uuid='" << bitstream->GetUuid() << "'";
  if(!this->Database->ExecuteQuery(query.str().c_str()))
    {
    this->Database->ExecuteQuery("ROLLBACK");
    this->Database->Close();
    return false;
    }
  
  query.str(std::string());
  query << "UPDATE bitstream SET internal_id='" << copyTo <<
    "', last_modified='" << lastModified << "' WHERE bitstream_id='"
    << bitstream->GetId() << "'";
  if(!this->Database->ExecuteQuery(query.str().c_str()))
    {
    this->Database->ExecuteQuery("ROLLBACK");
    this->Database->Close();
    return false;
    }
  
  this->Database->ExecuteQuery("COMMIT");
  this->Database->Close();
  return true;
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

      std::stringstream query;
      query << "UPDATE bitstream SET last_modified='" << lastModified
        << "' WHERE bitstream_id='" << i->Id << "'";
      this->Database->Open(this->DatabasePath.c_str());
      this->Database->ExecuteQuery(query.str().c_str());
      this->Database->Close();

      this->MarkDirtyResource(
        this->GetUuid(midasResourceType::BITSTREAM, i->Id),
        midasDirtyAction::MODIFIED);
      status = true;
      }
    }
  return status;
}