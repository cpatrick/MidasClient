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
void midasDatabaseProxy::FetchInfo(mdo::Community* community)
{
  if(community->IsFetched())
    {
    return;
    }
  std::stringstream query;
  query << "SELECT short_description, introductory_text, copyright_text "
    "FROM community WHERE community_id='" << community->GetId() << "'";

  this->Database->ExecuteQuery(query.str().c_str());

  while(this->Database->GetNextRow())
    {
    community->SetDescription(this->Database->GetValueAsString(0));
    community->SetIntroductoryText(this->Database->GetValueAsString(1));
    community->SetCopyright(this->Database->GetValueAsString(2));
    }
  community->SetFetched(true);
}

//-------------------------------------------------------------------------
void midasDatabaseProxy::FetchInfo(mdo::Collection* collection)
{
  if(collection->IsFetched())
    {
    return;
    }
  std::stringstream query;
  query << "SELECT short_description, introductory_text, copyright_text "
    "FROM collection WHERE collection_id='" << collection->GetId() << "'";

  this->Database->ExecuteQuery(query.str().c_str());

  while(this->Database->GetNextRow())
    {
    collection->SetDescription(this->Database->GetValueAsString(0));
    collection->SetIntroductoryText(this->Database->GetValueAsString(1));
    collection->SetCopyright(this->Database->GetValueAsString(2));
    }
  collection->SetFetched(true);
}

void midasDatabaseProxy::FetchInfo(mdo::Bitstream* bitstream)
{
  if(bitstream->IsFetched())
    {
    return;
    }
  std::stringstream query;
  query << "SELECT size_bytes FROM bitstream WHERE bitstream_id='"
    << bitstream->GetId() << "'";

  this->Database->ExecuteQuery(query.str().c_str());

  while(this->Database->GetNextRow())
    {
    std::stringstream val;
    val << this->Database->GetValueAsInt(0);
    bitstream->SetSize(val.str());
    }
  bitstream->SetFetched(true);
}

//-------------------------------------------------------------------------
void midasDatabaseProxy::FetchInfo(mdo::Item* item)
{
  if(item->IsFetched())
    {
    return;
    }

  std::stringstream query;
  query << "SELECT text_value FROM metadatavalue WHERE item_id='"
    << item->GetId() << "' AND metadata_field_id='27'";
  this->Database->ExecuteQuery(query.str().c_str());

  while(this->Database->GetNextRow())
    {
    item->SetAbstract(this->Database->GetValueAsString(0));
    }

  query.str(std::string());
  query << "SELECT text_value FROM metadatavalue WHERE item_id='"
    << item->GetId() << "' AND metadata_field_id='3'";
  this->Database->ExecuteQuery(query.str().c_str());

  while(this->Database->GetNextRow())
    {
    item->AddAuthor(this->Database->GetValueAsString(0));
    }
  
  query.str(std::string());
  query << "SELECT text_value FROM metadatavalue WHERE item_id='"
    << item->GetId() << "' AND metadata_field_id='57'";
  this->Database->ExecuteQuery(query.str().c_str());

  while(this->Database->GetNextRow())
    {
    item->AddKeyword(this->Database->GetValueAsString(0));
    }

  query.str(std::string());
  query << "SELECT text_value FROM metadatavalue WHERE item_id='"
    << item->GetId() << "' AND metadata_field_id='26'";
  this->Database->ExecuteQuery(query.str().c_str());

  while(this->Database->GetNextRow())
    {
    item->SetDescription(this->Database->GetValueAsString(0));
    }

  item->SetFetched(true);
}

//-------------------------------------------------------------------------
bool midasDatabaseProxy::SaveInfo(mdo::Community* community)
{
  std::stringstream query;
  query << "UPDATE community SET " <<
    "short_description='" << community->GetDescription() << "', "
    "introductory_text='" << community->GetIntroductoryText() << "', "
    "copyright_text='" << community->GetCopyright() << "' WHERE "
    "community_id='" << community->GetId() << "'";

  return this->Database->ExecuteQuery(query.str().c_str());
}

//-------------------------------------------------------------------------
bool midasDatabaseProxy::SaveInfo(mdo::Collection* coll)
{
  std::stringstream query;
  query << "UPDATE collection SET " << 
    "short_description='" << coll->GetDescription() << "', "
    "introductory_text='" << coll->GetIntroductoryText() << "', "
    "copyright_text='" << coll->GetCopyright() << "'" <<
    " WHERE collection_id='" << coll->GetId() << "'";

  return this->Database->ExecuteQuery(query.str().c_str());
}

//-------------------------------------------------------------------------
bool midasDatabaseProxy::SaveInfo(mdo::Item* item)
{
  bool ok = true;
  std::stringstream query;
  query << "DELETE FROM metadatavalue WHERE item_id='" << item->GetId() << "'";
  this->Database->ExecuteQuery(query.str().c_str());
  
  query.str(std::string());
  query << "INSERT INTO metadatavalue (item_id,metadata_field_id,text_value) "
    << "VALUES ('" << item->GetId() << "','27','" << item->GetAbstract()
    << "')";
  ok &= this->Database->ExecuteQuery(query.str().c_str());
 
  query.str(std::string());
  query << "INSERT INTO metadatavalue (item_id,metadata_field_id,text_value) "
    << "VALUES ('" << item->GetId() << "','64','" << item->GetTitle() << "')";
  ok &= this->Database->ExecuteQuery(query.str().c_str());

  query.str(std::string());
  query << "INSERT INTO metadatavalue (item_id,metadata_field_id,text_value) "
    << "VALUES ('" << item->GetId() << "','26','" << item->GetDescription() << "')";
  ok &= this->Database->ExecuteQuery(query.str().c_str());

  for(std::vector<std::string>::iterator i = item->GetAuthors().begin();
      i != item->GetAuthors().end(); ++i)
    {
    query.str(std::string());
    query << "INSERT INTO metadatavalue (item_id,metadata_field_id,text_value) "
      << "VALUES ('" << item->GetId() << "','3','" << *i << "')";
    ok &= this->Database->ExecuteQuery(query.str().c_str());
    }

  for(std::vector<std::string>::iterator i = item->GetKeywords().begin();
      i != item->GetKeywords().end(); ++i)
    {
    query.str(std::string());
    query << "INSERT INTO metadatavalue (item_id,metadata_field_id,text_value) "
      << "VALUES ('" << item->GetId() << "','57','" << *i << "')";
    ok &= this->Database->ExecuteQuery(query.str().c_str());
    }
  return ok;
}

//-------------------------------------------------------------------------
bool midasDatabaseProxy::SaveInfo(mdo::Bitstream* bitstream)
{
  std::stringstream query;
  query << "UPDATE bitstream SET size_bytes='" << bitstream->GetSize()
    << "' WHERE bitstream_id='" << bitstream->GetId() << "'";

  return this->Database->ExecuteQuery(query.str().c_str());
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
midasAuthProfile midasDatabaseProxy::GetAuthProfile(std::string name)
{
  std::stringstream query;
  query << "SELECT eperson, apikey, app_name, url FROM auth_profile WHERE "
    "profile_name='" << name << "'";

  midasAuthProfile profile;

  this->Database->ExecuteQuery(query.str().c_str());
  if(this->Database->GetNextRow())
    {
    profile.Name = name;
    profile.User = this->Database->GetValueAsString(0);
    profile.ApiKey = this->Database->GetValueAsString(1);
    profile.AppName = this->Database->GetValueAsString(2);
    profile.Url = this->Database->GetValueAsString(3);
    while(this->Database->GetNextRow());
    }
  return profile;
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
                                        std::string apiKey, std::string name,
                                        std::string url)
{
  this->DeleteProfile(name);

  std::stringstream query;
  query << "INSERT INTO auth_profile (profile_name, eperson, apikey, "
    "app_name, url) VALUES ('" << name << "', '" << user << "', '" << apiKey
    << "', '" << appName << "', '" << url << "')";

  return this->Database->ExecuteQuery(query.str().c_str());
}

//-------------------------------------------------------------------------
void midasDatabaseProxy::DeleteProfile(std::string name)
{
  std::stringstream query;
  query << "DELETE FROM auth_profile WHERE profile_name = '" << name << "'";
  this->Database->ExecuteQuery(query.str().c_str());
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
bool midasDatabaseProxy::IsResourceDirty(std::string uuid)
{
  std::stringstream query;
  query << "SELECT uuid FROM dirty_resource WHERE uuid='"
    << uuid << "'";
  this->Database->ExecuteQuery(query.str().c_str());

  bool dirty = this->Database->GetNextRow();
  while(this->Database->GetNextRow());
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
  this->Database->ExecuteQuery(query.str().c_str());
  std::string name = (this->Database->GetNextRow() ? 
    this->Database->GetValueAsString(0) : "");
  while(this->Database->GetNextRow());
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
  this->Database->ExecuteQuery(query.str().c_str());
  int parentId = (this->Database->GetNextRow() ? 
    this->Database->GetValueAsInt(0) : 0);
  while(this->Database->GetNextRow());
  return parentId;
}

//-------------------------------------------------------------------------
int midasDatabaseProxy::GetIdForUuid(std::string uuid)
{
  std::stringstream query;
  query << "SELECT resource_id FROM resource_uuid WHERE uuid='"
    << uuid << "'";
  this->Database->ExecuteQuery(query.str().c_str());

  int id = this->Database->GetNextRow() ? 
    this->Database->GetValueAsInt(0) : -1;
  while(this->Database->GetNextRow());
  return id;
}

//-------------------------------------------------------------------------
std::string midasDatabaseProxy::GetUuidFromPath(std::string path)
{
  std::stringstream query;
  query << "SELECT uuid FROM resource_uuid WHERE path='" << path << "'";
  this->Database->ExecuteQuery(query.str().c_str());

  std::string uuid = this->Database->GetNextRow() ? 
    this->Database->GetValueAsString(0) : "";
  while(this->Database->GetNextRow());
  return uuid;
}

//-------------------------------------------------------------------------
std::string midasDatabaseProxy::GetUuid(int type, int id)
{
  std::stringstream query;
  query << "SELECT uuid FROM resource_uuid WHERE resource_type_id='" 
    << type << "' AND resource_id='" << id << "'";
  this->Database->ExecuteQuery(query.str().c_str());

  std::string uuid = this->Database->GetNextRow() ? 
    this->Database->GetValueAsString(0) : "";
  while(this->Database->GetNextRow());
  return uuid;
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
  while(this->Database->GetNextRow());
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
  
  bool exists = this->Database->GetNextRow();
  while(this->Database->GetNextRow());
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
  //this->Database->ExecuteQuery("DELETE FROM app_settings");
}

//-------------------------------------------------------------------------
std::string midasDatabaseProxy::GetSetting(MidasAppSetting setting)
{
  std::string key = this->GetKeyName(setting);

  std::stringstream query;
  query << "SELECT value FROM app_settings WHERE key='" << key << "' LIMIT 1";
  this->Database->ExecuteQuery(query.str().c_str());
  
  std::string value;
  while(this->Database->GetNextRow())
    {
    value = this->Database->GetValueAsString(0);
    }
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
  this->Database->ExecuteQuery(query.str().c_str());
  query.str(std::string());

  query << "INSERT INTO app_settings (key, value) VALUES ('" << key << "', '"
    << value << "')";
  this->Database->ExecuteQuery(query.str().c_str());
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
std::vector<mdo::Community*> midasDatabaseProxy::GetTopLevelCommunities(
                                                            bool buildTree)
{
  std::vector<mdo::Community*> communities;
  std::stringstream query;
  query << "SELECT resource_uuid.uuid, community.community_id, community.name "
    "FROM community, resource_uuid WHERE resource_uuid.resource_id=community.community_id AND "
    "resource_uuid.resource_type_id='" << midasResourceType::COMMUNITY << "' AND community.community_id "
    "NOT IN (SELECT child_comm_id FROM community2community)";

  this->Database->ExecuteQuery(query.str().c_str());

  while(this->Database->GetNextRow())
    {
    mdo::Community* community = new mdo::Community;
    community->SetUuid(this->Database->GetValueAsString(0));
    community->SetId(this->Database->GetValueAsInt(1));
    community->SetName(this->Database->GetValueAsString(2));
    communities.push_back(community);
    }

  if(buildTree)
    {
    for(std::vector<mdo::Community*>::iterator i = communities.begin();
        i != communities.end(); ++i)
      {
      this->Populate(*i);
      }
    }
  return communities;
}

void midasDatabaseProxy::Populate(mdo::Community* node, bool recurse, bool checkDirty)
{
  if(checkDirty)
    {
    node->SetDirty(IsResourceDirty(GetUuid(
      midasResourceType::COMMUNITY, node->GetId())));
    }
  std::stringstream query;
  query << "SELECT community.community_id, community.name, resource_uuid.uuid "
    "FROM community, resource_uuid WHERE resource_uuid.resource_type_id='"
    << midasResourceType::COMMUNITY << "' AND resource_uuid.resource_id="
    "community.community_id AND community.community_id IN "
    "(SELECT child_comm_id FROM community2community WHERE parent_comm_id=" 
    << node->GetId() << ")";
  this->Database->ExecuteQuery(query.str().c_str());

  std::vector<mdo::Community*> childCommunities;
  while(this->Database->GetNextRow())
    {
    mdo::Community* community = new mdo::Community;
    community->SetId(this->Database->GetValueAsInt(0));
    community->SetName(this->Database->GetValueAsString(1));
    community->SetUuid(this->Database->GetValueAsString(2));
    childCommunities.push_back(community);
    }

  if(recurse)
    {
    // We can only recurse after fetching all database rows
    for(std::vector<mdo::Community*>::iterator i = childCommunities.begin();
        i != childCommunities.end(); ++i)
      {
      node->AddCommunity(*i);
      this->Populate(*i);
      }

    query.str(std::string());
    query << "SELECT collection.collection_id, collection.name, resource_uuid.uuid "
      "FROM collection, resource_uuid WHERE resource_uuid.resource_type_id='"
      << midasResourceType::COLLECTION << "' AND resource_uuid.resource_id="
      "collection.collection_id AND collection.collection_id IN (SELECT collection_id "
      "FROM community2collection WHERE community_id=" << node->GetId() << ")";
    this->Database->ExecuteQuery(query.str().c_str());

    std::vector<mdo::Collection*> collections;
    while(this->Database->GetNextRow())
      {
      mdo::Collection* collection = new mdo::Collection;
      collection->SetId(this->Database->GetValueAsInt(0));
      collection->SetName(this->Database->GetValueAsString(1));
      collection->SetUuid(this->Database->GetValueAsString(2));
      node->AddCollection(collection);
      collections.push_back(collection);
      }

    for(std::vector<mdo::Collection*>::iterator i = collections.begin();
        i != collections.end(); ++i)
      {
      this->Populate(*i);
      }
  }
}

void midasDatabaseProxy::Populate(mdo::Collection* node, bool recurse, bool checkDirty)
{
  if(checkDirty)
    {
    node->SetDirty(IsResourceDirty(GetUuid(
      midasResourceType::COLLECTION, node->GetId())));
    }
  std::stringstream query;
  query << "SELECT item.item_id, item.title, resource_uuid.uuid FROM item, resource_uuid "
    "WHERE resource_uuid.resource_type_id='" << midasResourceType::ITEM << "' AND "
    "resource_uuid.resource_id=item.item_id AND item.item_id IN (SELECT item_id FROM "
    "collection2item WHERE collection_id=" << node->GetId() << ")";
  this->Database->ExecuteQuery(query.str().c_str());

  std::vector<mdo::Item*> items;
  while(this->Database->GetNextRow())
    {
    mdo::Item* item = new mdo::Item;
    item->SetId(this->Database->GetValueAsInt(0));
    item->SetTitle(this->Database->GetValueAsString(1));
    item->SetUuid(this->Database->GetValueAsString(2));
    node->AddItem(item);
    items.push_back(item);
    }

  if(recurse)
    {
    for(std::vector<mdo::Item*>::iterator i = items.begin();
        i != items.end(); ++i)
      {
      this->Populate(*i);
      }
    }
}

void midasDatabaseProxy::Populate(mdo::Item* node, bool checkDirty)
{
  if(checkDirty)
    {
    node->SetDirty(IsResourceDirty(GetUuid(
      midasResourceType::ITEM, node->GetId())));
    }
  std::stringstream query;
  query << "SELECT bitstream.bitstream_id, bitstream.name, resource_uuid.uuid "
    "FROM bitstream, resource_uuid WHERE resource_uuid.resource_id=bitstream.bitstream_id "
    "AND resource_uuid.resource_type_id='" << midasResourceType::BITSTREAM << "' AND "
    "bitstream.bitstream_id IN (SELECT bitstream_id FROM item2bitstream WHERE item_id="
    << node->GetId() << ")";
  this->Database->ExecuteQuery(query.str().c_str());

  std::vector<mdo::Bitstream*> bitstreams;
  while(this->Database->GetNextRow())
    {
    mdo::Bitstream* bitstream = new mdo::Bitstream;
    bitstream->SetId(this->Database->GetValueAsInt(0));
    bitstream->SetName(this->Database->GetValueAsString(1));
    bitstream->SetUuid(this->Database->GetValueAsString(2));
    node->AddBitstream(bitstream);
    bitstreams.push_back(bitstream);
    }

  if(checkDirty)
    {
    for(std::vector<mdo::Bitstream*>::iterator i = bitstreams.begin();
        i != bitstreams.end(); ++i)
      {
      (*i)->SetDirty(IsResourceDirty(GetUuid(
        midasResourceType::BITSTREAM, (*i)->GetId())));
      }
    }
}

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
      this->Database->ExecuteQuery(query.str().c_str());

      while(this->Database->GetNextRow())
        {
        children.push_back(this->Database->GetValueAsInt(0));
        }
      for(std::vector<int>::iterator i = children.begin();
          i != children.end(); ++i)
        {
        this->DeleteResource(this->GetUuid(midasResourceType::COMMUNITY, *i));
        }

      query.str(std::string());
      query << "SELECT collection_id FROM community2collection WHERE "
        "community_id='" << record.Id << "'";
      this->Database->ExecuteQuery(query.str().c_str());

      children.clear();
      while(this->Database->GetNextRow())
        {
        children.push_back(this->Database->GetValueAsInt(0)); 
        }
      for(std::vector<int>::iterator i = children.begin();
          i != children.end(); ++i)
        {
        this->DeleteResource(this->GetUuid(midasResourceType::COLLECTION, *i));
        }

      query.str(std::string());
      query << "DELETE FROM community2community WHERE parent_comm_id='" <<
        record.Id << "'";
      this->Database->ExecuteQuery(query.str().c_str());

      query.str(std::string());
      query << "DELETE FROM community2collection WHERE community_id='" <<
        record.Id << "'";
      this->Database->ExecuteQuery(query.str().c_str());

      query.str(std::string());
      query << "DELETE FROM community WHERE community_id='" <<
        record.Id << "'";
      this->Database->ExecuteQuery(query.str().c_str());
      break;
    case midasResourceType::COLLECTION:
      query << "SELECT item_id FROM collection2item WHERE "
        "collection_id='" << record.Id << "'";
      this->Database->ExecuteQuery(query.str().c_str());

      while(this->Database->GetNextRow())
        {
        children.push_back(this->Database->GetValueAsInt(0));
        }
      for(std::vector<int>::iterator i = children.begin();
          i != children.end(); ++i)
        {
        this->DeleteResource(this->GetUuid(midasResourceType::ITEM, *i));
        }

      query.str(std::string());
      query << "DELETE FROM collection2item WHERE collection_id='" <<
        record.Id << "'";
      this->Database->ExecuteQuery(query.str().c_str());

      query.str(std::string());
      query << "DELETE FROM collection WHERE collection_id='" <<
        record.Id << "'";
      this->Database->ExecuteQuery(query.str().c_str());
      break;
    case midasResourceType::ITEM:
      query << "SELECT bitstream_id FROM item2bitstream WHERE "
        "item_id='" << record.Id << "'";
      this->Database->ExecuteQuery(query.str().c_str());

      while(this->Database->GetNextRow())
        {
        children.push_back(this->Database->GetValueAsInt(0));
        }
      for(std::vector<int>::iterator i = children.begin();
          i != children.end(); ++i)
        {
        this->DeleteResource(this->GetUuid(midasResourceType::BITSTREAM, *i));
        }

      query.str(std::string());
      query << "DELETE FROM item2bitstream WHERE item_id='" <<
        record.Id << "'";
      this->Database->ExecuteQuery(query.str().c_str());

      query.str(std::string());
      query << "DELETE FROM item WHERE item_id='" << record.Id << "'";
      this->Database->ExecuteQuery(query.str().c_str());
      break;
    case midasResourceType::BITSTREAM:
      query << "DELETE FROM bitstream WHERE bitstream_id='" << record.Id << "'";
      this->Database->ExecuteQuery(query.str().c_str());
      break;
    default:
      return false;
    }

  query.str(std::string());
  query << "DELETE FROM dirty_resource WHERE uuid='" << uuid << "'";
  this->Database->ExecuteQuery(query.str().c_str());

  query.str(std::string());
  query << "DELETE FROM resource_uuid WHERE uuid='" << uuid << "'";
  this->Database->ExecuteQuery(query.str().c_str());

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

void midasDatabaseProxy::UnifyTree()
{
  std::vector<mdo::Community*> topLevel = this->GetTopLevelCommunities(true);

  for(std::vector<mdo::Community*>::iterator i = topLevel.begin();
      i != topLevel.end(); ++i)
    {
    this->MergeOnDisk(*i);
    }
}

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

void midasDatabaseProxy::MergeOnDisk(mdo::Collection* coll)
{
  std::vector<mdo::Item*>::const_iterator i =
    coll->GetItems().begin();
  for(; i != coll->GetItems().end(); ++i)
    {
    MergeOnDisk(*i);
    }
}

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
      this->Database->ExecuteQuery(query.str().c_str());
      }
    }
}
