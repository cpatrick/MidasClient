/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "mdsDatabaseAPI.h"

#include "mdoCommunity.h"
#include "mdoCollection.h"
#include "mdoItem.h"
#include "mdoBitstream.h"
#include "mdoVersion.h"
#include "mdsCommunity.h"
#include "mdsCollection.h"
#include "mdsItem.h"
#include "mdsBitstream.h"
#include "m3doCommunity.h"
#include "m3doFolder.h"
#include "mdsResourceUpdateHandler.h"

#include <QDir>
#include <QFileInfo>
#include <QDateTime>

namespace mds {

/* By default new instantiations of this class will inherit
 * the properties set on the singleton mds::DatabaseInfo::Instance.
 * If you wish to override these, do so after construction.
 */
DatabaseAPI::DatabaseAPI(const std::string& path)
{
  this->DatabasePath = path == "" ?
    mds::DatabaseInfo::Instance()->GetPath() :
    path;
  this->Log = mds::DatabaseInfo::Instance()->GetLog();
  this->UpdateHandler =
    mds::DatabaseInfo::Instance()->GetResourceUpdateHandler();

  this->Database = new mds::SQLiteDatabase();
}

DatabaseAPI::~DatabaseAPI()
{
  delete this->Database;
}

//-------------------------------------------------------------------------
std::string DatabaseAPI::GetKeyName(MidasAppSetting setting)
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
int DatabaseAPI::AddResource(int type, std::string uuid,
  std::string path, std::string name, int parentType, int parentId,
  int serverParent)
{
  return this->AddResource(type, uuid, path, name, 
    this->GetUuid(parentType, parentId), serverParent);
}

//-------------------------------------------------------------------------
int DatabaseAPI::AddResource(int type, std::string uuid,
  std::string path, std::string name, std::string parentUuid, int parentId)
{
  name = midasUtils::EscapeForSQL(name);

  if(parentUuid == "" && type != midasResourceType::COMMUNITY)
    {
    return -1;
    }
  midasResourceRecord parent;
  if(parentUuid != "") //fetch parent before starting db transaction
    {
    parent = this->GetRecordByUuid(parentUuid);
    }
  if(!this->ResourceExists(uuid))
    {
    this->Database->Open(this->DatabasePath.c_str());
    this->Database->ExecuteQuery("BEGIN");
    int id = -1;
    mdo::Bitstream* bitstream = NULL;
    mdo::Item* item = NULL;
    mdo::Collection* collection = NULL;
    mdo::Community* community = NULL;
    mdo::Community* parentComm = NULL;
    switch(type)
      {
      case midasResourceType::BITSTREAM:
        id = this->InsertBitstream(path, name);
        if(id > 0 && this->UpdateHandler)
          {
          item = new mdo::Item;
          item->SetUuid(parentUuid.c_str());
          item->SetId(parent.Id);
          bitstream = new mdo::Bitstream;
          bitstream->SetId(id);
          bitstream->SetUuid(uuid.c_str());
          bitstream->SetParentItem(item);
          bitstream->SetName(name.c_str());
          this->UpdateHandler->AddedResource(bitstream);
          }
        break;
      case midasResourceType::COLLECTION:
        id = this->InsertCollection(name);
        if(id > 0 && this->UpdateHandler)
          {
          community = new mdo::Community;
          community->SetUuid(parentUuid.c_str());
          community->SetId(parent.Id);
          collection = new mdo::Collection;
          collection->SetId(id);
          collection->SetUuid(uuid.c_str());
          collection->SetParentCommunity(community);
          collection->SetName(name.c_str());
          this->UpdateHandler->AddedResource(collection);
          }
        break;
      case midasResourceType::COMMUNITY:
        id = this->InsertCommunity(name);
        if(id > 0 && this->UpdateHandler)
          {
          community = new mdo::Community;
          if(parentUuid != "")
            {
            parentComm = new mdo::Community;
            parentComm->SetUuid(parentUuid.c_str());
            parentComm->SetId(parent.Id);
            community->SetParentCommunity(parentComm);
            }
          else
            {
            community->SetParentCommunity(NULL);
            }
          community->SetUuid(uuid.c_str());
          community->SetId(id);
          community->SetName(name.c_str());
          this->UpdateHandler->AddedResource(community);
          }
        break;
      case midasResourceType::ITEM:
        id = this->InsertItem(name);
        if(id > 0 && this->UpdateHandler)
          {
          collection = new mdo::Collection;
          collection->SetUuid(parentUuid.c_str());
          collection->SetId(parent.Id);
          item = new mdo::Item;
          item->SetId(id);
          item->SetUuid(uuid.c_str());
          item->SetParentCollection(collection);
          item->SetTitle(name.c_str());
          this->UpdateHandler->AddedResource(item);
          }
        break;
      default:
        break;
      }
    if(id <= 0)
      {
      this->Log->Error("Failed to insert resource.");
      this->Database->ExecuteQuery("ROLLBACK");
      this->Database->Close();
      return -1;
      }

    if(!this->InsertResourceRecord(type, id, path, uuid, parentId))
      {
      this->Log->Error("Failed to insert resource uuid record.");
      this->Database->ExecuteQuery("ROLLBACK");
      this->Database->Close();
      return -1;
      }

    if(parentUuid != "" && !this->AddChild(parent.Type, parent.Id, type, id))
      {
      this->Log->Error(
        "Failed to add parent/child relationship to database.");
      this->Database->ExecuteQuery("ROLLBACK");
      this->Database->Close();
      return -1;
      }
    this->Database->ExecuteQuery("COMMIT");
    this->Database->Close();
    return id;
    }
  else
    {
    return this->GetRecordByUuid(uuid).Id;
    }
}

//-------------------------------------------------------------------------
midasAuthProfile DatabaseAPI::GetAuthProfile(std::string name)
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
    while(this->Database->GetNextRow()) {}
    }
  this->Database->Close();
  return profile;
}

//-------------------------------------------------------------------------
std::vector<std::string> DatabaseAPI::GetAuthProfiles()
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
bool DatabaseAPI::AddAuthProfile(std::string user, std::string appName,
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
void DatabaseAPI::DeleteProfile(std::string name)
{
  this->Database->Open(this->DatabasePath.c_str());
  std::stringstream query;
  query << "DELETE FROM auth_profile WHERE profile_name = '" << name << "'";
  this->Database->ExecuteQuery(query.str().c_str());
  this->Database->Close();
}

//-------------------------------------------------------------------------
void DatabaseAPI::MarkDirtyResource(std::string uuid, int dirtyAction)
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
void DatabaseAPI::ClearDirtyResource(std::string uuid)
{
  this->Database->Open(this->DatabasePath.c_str());
  std::stringstream query;
  query << "DELETE FROM dirty_resource WHERE uuid='" << uuid << "'";
  this->Database->ExecuteQuery(query.str().c_str());
  this->Database->Close();
}

//-------------------------------------------------------------------------
bool DatabaseAPI::IsResourceDirty(std::string uuid)
{
  std::stringstream query;
  query << "SELECT uuid FROM dirty_resource WHERE uuid='"
    << uuid << "'";
  this->Database->Open(this->DatabasePath.c_str());
  this->Database->ExecuteQuery(query.str().c_str());

  bool dirty = this->Database->GetNextRow();
  while(this->Database->GetNextRow()) {}
  this->Database->Close();
  return dirty;
}

//-------------------------------------------------------------------------
std::string DatabaseAPI::GetName(int type, int id)
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
  while(this->Database->GetNextRow()) {}
  this->Database->Close();
  return name;
}

//-------------------------------------------------------------------------
int DatabaseAPI::GetParentId(int type, int id)
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
  while(this->Database->GetNextRow()) {}
  this->Database->Close();
  return parentId;
}

//-------------------------------------------------------------------------
std::string DatabaseAPI::GetUuid(int type, int id)
{
  std::stringstream query;
  query << "SELECT uuid FROM resource_uuid WHERE resource_type_id='" 
    << type << "' AND resource_id='" << id << "'";
  this->Database->Open(this->DatabasePath.c_str());
  this->Database->ExecuteQuery(query.str().c_str());

  std::string uuid = this->Database->GetNextRow() ? 
    this->Database->GetValueAsString(0) : "";
  while(this->Database->GetNextRow()) {}
  this->Database->Close();
  return uuid;
}

//-------------------------------------------------------------------------
midasResourceRecord DatabaseAPI::GetRecordByUuid(std::string uuid)
{
  std::stringstream query;
  query << "SELECT resource_type_id, resource_id, server_parent, path FROM "
    "resource_uuid WHERE uuid='" << uuid << "'";
  this->Database->Open(this->DatabasePath.c_str());
  this->Database->ExecuteQuery(query.str().c_str());
  midasResourceRecord record;
  record.Uuid = uuid;

  if(this->Database->GetNextRow())
    {
    record.Type = this->Database->GetValueAsInt(0);
    record.Id = this->Database->GetValueAsInt(1);
    record.Parent = this->Database->GetValueAsInt(2);
    record.Path = this->Database->GetValueAsString(3);
    }
  while(this->Database->GetNextRow()) {}
  this->Database->Close();
  return record;
}

//-------------------------------------------------------------------------
bool DatabaseAPI::AddChild(int parentType, int parentId,
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
int DatabaseAPI::InsertBitstream(std::string path, std::string name)
{
  std::stringstream query;
  query << "INSERT INTO bitstream (location, internal_id, name) VALUES ('1','"
    << path << "', '" << name << "')";

  if(!this->Database->ExecuteQuery(query.str().c_str()))
    {
    return -1;
    }
  return this->Database->GetLastInsertId();
}

//-------------------------------------------------------------------------
int DatabaseAPI::InsertCollection(std::string name)
{
  std::stringstream query;
  query << "INSERT INTO collection (name) VALUES ('" << name << "')";

  if(!this->Database->ExecuteQuery(query.str().c_str()))
    {
    return -1;
    }
  return this->Database->GetLastInsertId();
}

//-------------------------------------------------------------------------
int DatabaseAPI::InsertCommunity(std::string name)
{
  std::stringstream query;
  query << "INSERT INTO community (name) VALUES ('" << name << "')";

  if(!this->Database->ExecuteQuery(query.str().c_str()))
    {
    return -1;
    }
  return this->Database->GetLastInsertId();
}

//-------------------------------------------------------------------------
int DatabaseAPI::InsertItem(std::string name)
{
  std::stringstream query;
  query << "INSERT INTO item (title) VALUES ('" << name << "')";

  if(!this->Database->ExecuteQuery(query.str().c_str()))
    {
    return -1;
    }
  return this->Database->GetLastInsertId();
}

//-------------------------------------------------------------------------
bool DatabaseAPI::InsertResourceRecord(int type, int id,
                                              std::string path,
                                              std::string uuid, int parentId)
{
  std::stringstream query;
  query << "INSERT INTO resource_uuid (resource_type_id, resource_id, path, "
    "uuid, server_parent) VALUES ('" << type << "', '" << id << "', '"
    << path << "', '" << uuid << "', '" << parentId << "')";
  return this->Database->ExecuteQuery(query.str().c_str());
}

//-------------------------------------------------------------------------
bool DatabaseAPI::ResourceExists(std::string uuid)
{
  std::stringstream query;
  query << "SELECT * FROM resource_uuid WHERE uuid='" << uuid << "'";
  this->Database->Open(this->DatabasePath.c_str());
  this->Database->ExecuteQuery(query.str().c_str());
  
  bool exists = this->Database->GetNextRow();
  while(this->Database->GetNextRow()) {}
  this->Database->Close();
  return exists;
}

//-------------------------------------------------------------------------
bool DatabaseAPI::Open()
{
  return this->Database->Open(this->DatabasePath.c_str());
}

//-------------------------------------------------------------------------
bool DatabaseAPI::Close()
{
  return this->Database->Close();
}

//-------------------------------------------------------------------------
void DatabaseAPI::Clean()
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
std::string DatabaseAPI::GetSetting(MidasAppSetting setting)
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

//-------------------------------------------------------------------------
int DatabaseAPI::GetSettingInt(MidasAppSetting setting)
{
  std::string val = this->GetSetting(setting);
  return atoi(val.c_str());
}

//-------------------------------------------------------------------------
bool DatabaseAPI::GetSettingBool(MidasAppSetting setting)
{
  std::string val = this->GetSetting(setting);
  return atoi(val.c_str()) != 0;
}

//-------------------------------------------------------------------------
void DatabaseAPI::SetSetting(MidasAppSetting setting, std::string value)
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

void DatabaseAPI::SetSetting(MidasAppSetting setting, int value)
{
  std::stringstream s;
  s << value;
  this->SetSetting(setting, s.str());
}

void DatabaseAPI::SetSetting(MidasAppSetting setting, bool value)
{
  this->SetSetting(setting, value ? 1 : 0);
}

//--------------------------------------------------------------------------
std::vector<midasStatus> DatabaseAPI::GetStatusEntries()
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

    midasStatus status(record.Id, uuids[i], name, actions[i],
      midasResourceType::ResourceType(record.Type), record.Path);

    statlist.push_back(status);
    }
  return statlist;
}

//--------------------------------------------------------------------------
std::vector<mdo::Community*> DatabaseAPI::GetTopLevelCommunities(
                                                            bool buildTree)
{
  std::vector<mdo::Community*> communities;
  std::stringstream query;
  query << "SELECT resource_uuid.uuid, community.community_id, community.name "
    "FROM community, resource_uuid WHERE resource_uuid.resource_id=community.community_id AND "
    "resource_uuid.resource_type_id='" << static_cast<int>(midasResourceType::COMMUNITY) << "' AND community.community_id "
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
  for(std::vector<mdo::Community*>::iterator i = communities.begin();
      i != communities.end(); ++i)
    {
    (*i)->SetDirty(this->IsResourceDirty((*i)->GetUuid()));
    }

  if(buildTree)
    {
    for(std::vector<mdo::Community*>::iterator i = communities.begin();
        i != communities.end(); ++i)
      {
      mds::Community mdsComm;
      mdsComm.SetObject(*i);
      mdsComm.FetchTree();
      }
    }
  return communities;
}

//-------------------------------------------------------------------------
std::vector<m3do::Folder*> DatabaseAPI::GetTopLevelFolders()
{
  this->Database->Open(this->DatabasePath.c_str());
  this->Database->ExecuteQuery("SELECT parent_id, folder_id, name, uuid, "
    "description FROM folder WHERE parent_id < 0");
  std::vector<m3do::Folder*> folders;

  while(this->Database->GetNextRow())
    {
    int parentId = this->Database->GetValueAsInt(0);
    m3do::Folder* folder = parentId == -1 ?
      new m3do::Folder : new m3do::Community;
    
    folder->SetId(this->Database->GetValueAsInt(1));
    folder->SetName(this->Database->GetValueAsString(2));
    folder->SetUuid(this->Database->GetValueAsString(3));
    folder->SetDescription(this->Database->GetValueAsString(4));

    folders.push_back(folder);
    }
  this->Database->Close();

  return folders;
}

//--------------------------------------------------------------------------
bool DatabaseAPI::DeleteResource(std::string uuid, bool deleteFiles)
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
      mdsComm.SetPath(record.Path);
      ok = mdsComm.Delete(deleteFiles);
      if(ok && this->UpdateHandler)
        {
        this->UpdateHandler->DeletedResource(comm);
        }
      delete comm;
      break;
    case midasResourceType::COLLECTION:
      coll = new mdo::Collection;
      coll->SetId(record.Id);
      coll->SetUuid(uuid.c_str());
      mdsColl.SetObject(coll);
      mdsColl.SetPath(record.Path);
      ok = mdsColl.Delete(deleteFiles);
      if(ok && this->UpdateHandler)
        {
        this->UpdateHandler->DeletedResource(coll);
        }
      delete coll;
      break;
    case midasResourceType::ITEM:
      item = new mdo::Item;
      item->SetId(record.Id);
      item->SetUuid(uuid.c_str());
      mdsItem.SetObject(item);
      mdsItem.SetPath(record.Path);
      ok = mdsItem.Delete(deleteFiles);
      if(ok && this->UpdateHandler)
        {
        this->UpdateHandler->DeletedResource(item);
        }
      delete item;
      break;
    case midasResourceType::BITSTREAM:
      bitstream = new mdo::Bitstream;
      bitstream->SetId(record.Id);
      bitstream->SetUuid(uuid.c_str());
      bitstream->SetPath(record.Path);
      mdsBitstream.SetObject(bitstream);
      ok = mdsBitstream.Delete(deleteFiles);
      if(ok && this->UpdateHandler)
        {
        this->UpdateHandler->DeletedResource(bitstream);
        }
      delete bitstream;
      break;
    default:
      return false;
    }
  return ok;
}

//--------------------------------------------------------------------------
bool DatabaseAPI::UnifyTree(bool copy)
{
  std::string rootDir = QDir(
    this->GetSetting(DatabaseAPI::ROOT_DIR).c_str()).path().toStdString();

  rootDir = midasUtils::TrimTrailingSlash(rootDir);
  this->Log->Message("Relocating all resources under " + rootDir);
  std::vector<mdo::Community*> topLevel = this->GetTopLevelCommunities(true);

  bool ok = true;
  for(std::vector<mdo::Community*>::iterator i = topLevel.begin();
      i != topLevel.end(); ++i)
    {
    ok &= this->Relocate(*i, rootDir, copy);
    }
  return ok;
}

//--------------------------------------------------------------------------
bool DatabaseAPI::Relocate(mdo::Community* comm, std::string parentDir,
                           bool copy)
{
  std::string copyTo = parentDir + "/" + comm->GetName();
  
  std::stringstream text;
  text << "Relocating community to " << copyTo << std::endl;
  this->Log->Status(text.str());
  QFileInfo copyToInfo(copyTo.c_str());
  if(!copyToInfo.isDir() && !QDir().mkdir(copyTo.c_str()))
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
    ok &= this->Relocate(*i, copyTo, copy);
    }
  for(std::vector<mdo::Collection*>::const_iterator i =
      comm->GetCollections().begin(); i != comm->GetCollections().end(); ++i)
    {
    ok &= this->Relocate(*i, copyTo, copy);
    }
  return ok;
}

//--------------------------------------------------------------------------
bool DatabaseAPI::Relocate(mdo::Collection* coll,
                           std::string parentDir, bool copy)
{
  std::string copyTo = parentDir + "/" + coll->GetName();
  
  std::stringstream text;
  text << "Relocating collection to " << copyTo << std::endl;
  this->Log->Status(text.str());
  QFileInfo copyToInfo(copyTo.c_str());
  if(!copyToInfo.isDir() && !QDir().mkdir(copyTo.c_str()))
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
    ok &= this->Relocate(*i, copyTo, copy);
    }
  return ok;
}

//--------------------------------------------------------------------------
bool DatabaseAPI::Relocate(mdo::Item* item, std::string parentDir,
                                  bool copy)
{
  std::string copyTo = parentDir + "/" + item->GetName();
  
  std::stringstream text;
  text << "Relocating item to " << copyTo << std::endl;
  this->Log->Status(text.str());
  QFileInfo copyToInfo(copyTo.c_str());
  if(!copyToInfo.isDir() && !QDir().mkdir(copyTo.c_str()))
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
    ok &= this->Relocate(*i, copyTo, copy);
    }
  return ok;
}

//--------------------------------------------------------------------------
bool DatabaseAPI::Relocate(mdo::Bitstream* bitstream,
                                  std::string parentDir, bool copy)
{
  std::string copyTo = parentDir + "/" + bitstream->GetName();
  std::string path = this->GetRecordByUuid(bitstream->GetUuid()).Path;

  QFileInfo copyToInfo(copyTo.c_str());
  QFileInfo pathInfo(path.c_str());
  if(copyToInfo.absoluteFilePath() == pathInfo.absoluteFilePath())
    {
    return true; //if the new path is the same as old one, don't copy
    }

  bitstream->SetFetched(false);
  std::stringstream text;
  text << "Relocating bitstream to " << copyTo << std::endl;
  this->Log->Status(text.str());
  
  if(copy)
    {
    if(!QFile::copy(path.c_str(), copyTo.c_str()))
      {
      std::stringstream error;
      error << "Error: unable to copy bitstream to " << copyTo << std::endl;
      this->Log->Error(error.str());
      return false;
      }
    }
  else //move
    {
    if(!midasUtils::RenameFile(path.c_str(), copyTo.c_str()))
      {
      std::stringstream error;
      error << "Error: unable to move bitstream to " << copyTo << std::endl;
      this->Log->Error(error.str());
      return false;
      }
    }

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
    "', last_modified='" << copyToInfo.lastModified().toTime_t() <<
    "' WHERE bitstream_id='" << bitstream->GetId() << "'";
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
std::vector<mdo::Object*> DatabaseAPI::Search(std::vector<std::string> tokens)
{
  std::vector<mdo::Object*> results;
  (void)tokens;
  //TODO implement search

  return results;
}

//--------------------------------------------------------------------------
std::string DatabaseAPI::GetUuidFromPath(std::string path)
{
  // First make sure we use the absolute path
  QFileInfo fileInfo(path.c_str());
  if(!fileInfo.isAbsolute())
    {
    path = QDir::currentPath().toStdString() + "/" + path;
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

//-------------------------------------------------------------------------
bool DatabaseAPI::CheckModifiedBitstreams()
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
    QFileInfo fileInfo(i->Path.c_str());
    if(!fileInfo.isFile())
      {
      std::stringstream text;
      text << "Bitstream " << i->Path << " no longer exists in the filesystem."
        << " Removing it from local MIDAS control." << std::endl;
      this->Log->Message(text.str());
      this->DeleteResource(this->GetUuid(midasResourceType::BITSTREAM, i->Id));
      status = true;
      continue;
      }

    if(fileInfo.lastModified().toTime_t() != i->LastModified)
      {
      std::stringstream text;
      text << "Bitstream " << i->Path << " was modified on disk. "
        << "Marking resource as dirty." << std::endl;
      this->Log->Message(text.str());

      std::stringstream query;
      query << "UPDATE bitstream SET last_modified='" <<
        fileInfo.lastModified().toTime_t() << "' WHERE bitstream_id='"
        << i->Id << "'";
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

// Static method to get the current database version
mdo::Version DatabaseAPI::GetCurrentDatabaseVersion()
{
  mds::DatabaseAPI db;
  if(!db.Open())
    {
    return mdo::Version(0, 0, 0);
    }
  if(!db.Database->ExecuteQuery("SELECT major, minor, patch FROM version "
                      "WHERE name='MIDASClient'"))
    {
    db.Close();
    return mdo::Version(0, 0, 0);
    }
  mdo::Version dbVersion;
  while(db.Database->GetNextRow())
    {
    dbVersion.Major = db.Database->GetValueAsInt(0);
    dbVersion.Minor = db.Database->GetValueAsInt(1);
    dbVersion.Patch = db.Database->GetValueAsInt(2);
    }
  db.Close();
  return dbVersion;
}

} //end namespace mds
