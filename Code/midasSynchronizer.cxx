/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "midasSynchronizer.h"

#include "mwsWebAPI.h"
#include "mwsCommunity.h"
#include "mdoCommunity.h"
#include "mwsCollection.h"
#include "mdoCollection.h"
#include "mwsBitstream.h"
#include "mwsItem.h"
#include "mwsRestXMLParser.h"
#include "mdoItem.h"
#include "midasProgressReporter.h"
#include "midasDatabaseProxy.h"
#include "midasStdOutLog.h"

#define WORKING_DIR kwsys::SystemTools::GetCurrentWorkingDirectory
#define CHANGE_DIR kwsys::SystemTools::ChangeDirectory
#define MKDIR kwsys::SystemTools::MakeDirectory

#define NO_PARENT -1

midasSynchronizer::midasSynchronizer()
{
  this->Recursive = false;
  this->ShouldCancel = false;
  this->Operation = OPERATION_NONE;
  this->ResourceType = midasResourceType::NONE;
  this->ServerURL = "";
  this->Progress = NULL;
  this->Log = NULL;
  this->Database = "";
  this->DatabaseProxy = NULL;
  this->Authenticator = new midasAuthenticator;
  this->Authenticator->SetLog(this->Log);
  this->ParentId = 0;
}

midasSynchronizer::~midasSynchronizer()
{
  delete this->DatabaseProxy;
  delete this->Authenticator;
}

midasAuthenticator* midasSynchronizer::GetAuthenticator()
{
  return this->Authenticator;
}

int midasSynchronizer::GetParentId()
{
  return this->ParentId;
}

void midasSynchronizer::SetParentId(int id)
{
  this->ParentId = id;
}

void midasSynchronizer::SetDatabase(std::string path)
{
  this->Database = path;
  delete this->DatabaseProxy;
  this->DatabaseProxy = new midasDatabaseProxy(path);
  this->Authenticator->SetDatabase(path);
  this->SetServerURL(this->GetServerURL());
}

void midasSynchronizer::SetLog(midasLog* log)
{
  this->Log = log;
  this->Authenticator->SetLog(log);
}

midasDatabaseProxy* midasSynchronizer::GetDatabase()
{
  return this->DatabaseProxy;
}

midasLog* midasSynchronizer::GetLog()
{
  return this->Log;
}

void midasSynchronizer::DeleteLog()
{
  delete this->Log;
}

void midasSynchronizer::SetProgressReporter(midasProgressReporter* progress)
{
  this->Progress = progress;
}

midasProgressReporter* midasSynchronizer::GetProgressReporter()
{
  return this->Progress;
}

void midasSynchronizer::SetRecursive(bool recursive)
{
  this->Recursive = recursive;
}

bool midasSynchronizer::GetRecursive()
{
  return this->Recursive;
}

void midasSynchronizer::SetOperation(midasSynchronizer::SynchOperation op)
{
  this->Operation = op;
}

midasSynchronizer::SynchOperation midasSynchronizer::GetOperation()
{
  return this->Operation;
}

void midasSynchronizer::SetResourceType(int type)
{
  this->ResourceType = type;
}

int midasSynchronizer::GetResourceType()
{
  return this->ResourceType;
}

void midasSynchronizer::SetResourceHandle(std::string handle)
{
  this->ResourceHandle = handle;
}

std::string midasSynchronizer::GetResourceHandle()
{
  return this->ResourceHandle;
}

std::string midasSynchronizer::GetServerURL()
{
  if(this->ServerURL == "")
    {
    this->DatabaseProxy->Open();
    this->ServerURL =
      this->DatabaseProxy->GetSetting(midasDatabaseProxy::LAST_URL);
    this->DatabaseProxy->Close();
    }
  return this->ServerURL;
}

void midasSynchronizer::SetServerURL(std::string url)
{
  mws::WebAPI::Instance()->SetServerUrl(url.c_str());
  this->Authenticator->SetServerURL(url.c_str());
  this->DatabaseProxy->Open();
  this->DatabaseProxy->SetSetting(midasDatabaseProxy::LAST_URL, url);
  this->DatabaseProxy->Close();
  this->ServerURL = url;
}

//-------------------------------------------------------------------
std::vector<midasStatus> midasSynchronizer::GetStatusEntries()
{
  this->DatabaseProxy->Open();
  std::vector<midasStatus> stats = this->DatabaseProxy->GetStatusEntries();
  this->DatabaseProxy->Close();
  return stats;
}

//-------------------------------------------------------------------
int midasSynchronizer::Perform()
{
  int rc = 0;
  this->ShouldCancel = false;
  if(!this->Authenticator->Login(mws::WebAPI::Instance()))
    {
    std::stringstream text;
    text << "Login failed." << std::endl;
    this->Log->Error(text.str());
    this->Reset();
    return MIDAS_LOGIN_FAILED;
    }

  std::string temp = WORKING_DIR();
  this->DatabaseProxy->Open();
  std::string wdir = this->DatabaseProxy->GetSetting(midasDatabaseProxy::ROOT_DIR);
  this->DatabaseProxy->Close();

  if(wdir != "")
    {
    CHANGE_DIR(wdir.c_str());
    }
  switch(this->Operation)
    {
    case OPERATION_ADD:
      rc = this->Add();
      break;
    case OPERATION_CLEAN:
      rc = this->Clean();
      break;
    case OPERATION_CLONE:
      rc = this->Clone();
      break;
    case OPERATION_PULL:
      rc = this->Pull();
      break;
    case OPERATION_PUSH:
      rc = this->Push();
      break;
    default:
      rc = MIDAS_BAD_OP;
      break;
    }
  CHANGE_DIR(temp.c_str());
  if(this->ShouldCancel)
    {
    rc = MIDAS_CANCELED;
    }
  this->Reset();
  return rc;
}

//-------------------------------------------------------------------
int midasSynchronizer::Add()
{
  std::string path = 
    kwsys::SystemTools::FileIsFullPath(this->ResourceHandle.c_str()) ?
    this->ResourceHandle : WORKING_DIR() + "/" + this->ResourceHandle;

  if(!kwsys::SystemTools::FileExists(path.c_str()))
    {
    std::stringstream text;
    text << "Error: \"" << this->ResourceHandle << "\" does "
      "not refer to a valid absolute or relative path." << std::endl;
    this->Log->Error(text.str());
    return MIDAS_INVALID_PATH;
    }
  if(kwsys::SystemTools::FileIsDirectory(path.c_str()) &&
     this->ResourceType == midasResourceType::BITSTREAM)
    {
    std::stringstream text;
    text << "Error: \"" << path << "\" is a directory. A "
      "bitstream refers to a file, not a directory." << std::endl;
    this->Log->Error(text.str());
    return MIDAS_BAD_FILE_TYPE;
    }
  if(!kwsys::SystemTools::FileIsDirectory(path.c_str()) &&
    this->ResourceType != midasResourceType::BITSTREAM)
    {
    std::stringstream text;
    text << "Error: \"" << path << "\" is not a directory. For "
      "this resource type, you must specify a directory." << std::endl;
    this->Log->Error(text.str());
    return MIDAS_BAD_FILE_TYPE;
    }
  if(this->ResourceType == midasResourceType::BITSTREAM &&
    kwsys::SystemTools::FileLength(path.c_str()) == 0)
    {
    std::stringstream text;
    text << "Error: \"" << path << "\" is 0 bytes. You may "
      "not add an empty bitstream." << std::endl;
    this->Log->Error(text.str());
    return MIDAS_EMPTY_FILE;
    }

  // Make slashes uniform in the database
  kwsys::SystemTools::ConvertToUnixSlashes(path);
  std::string uuid = midasUtils::GenerateUUID();

  std::string name = kwsys::SystemTools::GetFilenameName(path);
  std::string parentDir = 
    kwsys::SystemTools::GetParentDirectory(path.c_str());

  this->DatabaseProxy->Open();
  if(this->DatabaseProxy->GetUuidFromPath(path) != "")
    {
    std::stringstream text;
    text << "Error: \"" << path << "\" is already in the "
      "database." << std::endl;
    this->Log->Error(text.str());
    this->DatabaseProxy->Close();
    return MIDAS_DUPLICATE_PATH;
    }

  std::string parentUuid = this->DatabaseProxy->GetUuidFromPath(parentDir);

  if(!this->ValidateParentId(this->ParentId,
    midasResourceType::ResourceType(this->ResourceType)) && parentUuid == "")
    {
    std::stringstream text;
    text << "The parent of this resource could not be resolved." << std::endl;
    this->Log->Error(text.str());
    this->DatabaseProxy->Close();
    return MIDAS_INVALID_PARENT;
    }

  int id = this->DatabaseProxy->AddResource(this->ResourceType, uuid, 
    path, name, parentUuid, this->ParentId);
  if(id <= 0)
    {
    std::stringstream text;
    text << "AddResource failed: " << path << std::endl;
    this->Log->Error(text.str());
    this->DatabaseProxy->Close();
    return MIDAS_FAILURE;
    }
  this->DatabaseProxy->MarkDirtyResource(uuid, midasDirtyAction::ADDED);

  //TODO propagate last modified stamp up the local tree. (perhaps in MarkDirtyResource())

  this->DatabaseProxy->Close();
  return MIDAS_OK;
}

//-------------------------------------------------------------------
bool midasSynchronizer::ValidateParentId(int parentId,
                                         midasResourceType::ResourceType type)
{
  if(parentId == 0)
    {
    // Only communities may have no parent
    return type == midasResourceType::COMMUNITY;
    }
  std::stringstream fields;
  fields << "midas.";
  switch(type)
    {
    case midasResourceType::BITSTREAM:
      fields << "item";
      break;
    case midasResourceType::COLLECTION:
      fields << "community";
      break;
    case midasResourceType::COMMUNITY:
      fields << "community";
      break;
    case midasResourceType::ITEM:
      fields << "collection";
    default:
      return false;
    }
  fields << ".get?id=" << parentId;
  
  mws::RestXMLParser parser;
  mws::WebAPI::Instance()->GetRestAPI()->SetXMLParser(&parser);
  return mws::WebAPI::Instance()->Execute(fields.str().c_str());
}

//-------------------------------------------------------------------
int midasSynchronizer::Clean()
{
  this->DatabaseProxy->Open();
  this->DatabaseProxy->Clean();
  this->DatabaseProxy->Close();
  return 0;
}

//-------------------------------------------------------------------
int midasSynchronizer::Clone()
{
  if(this->GetServerURL() == "")
    {
    std::stringstream text;
    text << "You must specify a server url. No last used URL "
      "exists in the database." << std::endl;
    this->Log->Error(text.str());
    return MIDAS_NO_URL;
    }

  this->Log->Status("Cloning the MIDAS repository...");

  mws::Community remote;
  mdo::Community* community = new mdo::Community;
  remote.SetWebAPI(mws::WebAPI::Instance());
  remote.SetObject(community);

  this->Progress->SetIndeterminate();
  if(!remote.FetchTree())
    {
    std::stringstream text;
    text << "Unable to fetch resource tree via the Web API" << std::endl;
    Log->Error(text.str());
    return MIDAS_WEB_API_FAILED;
    }

  this->RecurseCommunities(NO_PARENT, community);
  delete community;

  return 0;
}

//-------------------------------------------------------------------
int ProgressCallback(void *clientp, double dltotal, double dlnow, 
                     double ultotal, double ulnow)
{
  midasProgressReporter* out
    = reinterpret_cast<midasProgressReporter*>(clientp);
  out->UpdateProgress(dlnow, dltotal);
  return 0;
}

//-------------------------------------------------------------------
int midasSynchronizer::Pull()
{
  if(this->ResourceType == midasResourceType::NONE)
    {
    std::stringstream text;
    text << "You must specify a resource type." << std::endl;
    Log->Error(text.str());
    return MIDAS_NO_RTYPE;
    }
  if(this->GetServerURL() == "")
    {
    std::stringstream text;
    text << "You must specify a server url. No last used URL "
      "exists in the database." << std::endl;
    Log->Error(text.str());
    return MIDAS_NO_URL;
    }

  switch(this->ResourceType)
    {
    case midasResourceType::BITSTREAM:
      return this->PullBitstream(NO_PARENT) ? MIDAS_OK : MIDAS_FAILURE;
    case midasResourceType::COLLECTION:
      return this->PullCollection(NO_PARENT) ? MIDAS_OK : MIDAS_FAILURE;
    case midasResourceType::COMMUNITY:
      return this->PullCommunity(NO_PARENT) ? MIDAS_OK : MIDAS_FAILURE;
    case midasResourceType::ITEM:
      return this->PullItem(NO_PARENT) ? MIDAS_OK : MIDAS_FAILURE;
    default:
      return MIDAS_NO_RTYPE;
    }
}

//-------------------------------------------------------------------
bool midasSynchronizer::PullBitstream(int parentId)
{
  if(this->ShouldCancel)
    {
    return false;
    }
  mws::Bitstream remote;
  mdo::Bitstream* bitstream = new mdo::Bitstream;
  bitstream->SetId(atoi(this->ResourceHandle.c_str()));
  remote.SetWebAPI(mws::WebAPI::Instance());
  remote.SetObject(bitstream);
  
  this->Progress->SetIndeterminate();
  this->Log->Status("Fetching bitstream information...");
  if(!remote.Fetch())
    {
    std::stringstream text;
    text << "Unable to get bitstream via the web API."
      << std::endl;
    Log->Error(text.str());
    return false;
    }

  // Pull any parents we need
  if(parentId == NO_PARENT)
    {
    bool recurse = this->Recursive;
    this->Recursive = false;
    std::string handle = this->ResourceHandle;
    this->ResourceHandle = bitstream->GetParent();

    this->PullItem(NO_PARENT);
    this->DatabaseProxy->Open();
    CHANGE_DIR(this->LastDir.c_str());

    this->ResourceHandle = handle;
    this->Recursive = recurse;
    parentId = this->LastId;
    }

  if(bitstream->GetName() == "")
    {
    std::stringstream text;
    text << "Bitstream " << this->ResourceHandle <<
      " does not exist." << std::endl;
    Log->Error(text.str());
    return false;
    }

  this->DatabaseProxy->Open();
  midasResourceRecord record =
    this->DatabaseProxy->GetRecordByUuid(bitstream->GetUuid());

  //TODO check md5 sum of file at location against server's checksum?
  if(record.Path != "" &&
     kwsys::SystemTools::FileExists(record.Path.c_str(), true))
    {
    //we already have this bitstream, no need to download again
    this->DatabaseProxy->Close();
    return true;
    }

  std::stringstream fields;
  fields << "midas.bitstream.download?id=" << this->GetResourceHandle();
  
  if(this->Progress)
    {
    mws::WebAPI::Instance()->GetRestAPI()->SetProgressCallback(
      ProgressCallback, this->Progress);
    this->Progress->SetMessage(bitstream->GetName());
    this->Progress->ResetProgress();
    }
  std::stringstream status;
  status << "Downloading bitstream " << bitstream->GetName();
  this->Log->Status(status.str());

  mws::WebAPI::Instance()->DownloadFile(fields.str().c_str(),
                             bitstream->GetName().c_str());

  int id = this->DatabaseProxy->AddResource(midasResourceType::BITSTREAM,
    bitstream->GetUuid(), WORKING_DIR() + "/" + bitstream->GetName(),
    bitstream->GetName(), midasResourceType::ITEM, parentId, 0);
  bitstream->SetId(id);
  this->DatabaseProxy->SaveInfo(bitstream);
  this->DatabaseProxy->Close();
  
  return true;
}

//-------------------------------------------------------------------
bool midasSynchronizer::PullCollection(int parentId)
{
  if(this->ShouldCancel)
    {
    return false;
    }
  this->DatabaseProxy->Open();

  mws::Collection remote;
  mdo::Collection* collection = new mdo::Collection;
  collection->SetId(atoi(this->GetResourceHandle().c_str()));
  remote.SetWebAPI(mws::WebAPI::Instance());
  remote.SetObject(collection);

  this->Progress->SetIndeterminate();
  this->Log->Status("Fetching collection information...");
  if(!remote.Fetch())
    {
    std::stringstream text;
    text << "Unable to fetch the collection via the Web API."
      << std::endl;
    Log->Error(text.str());
    this->DatabaseProxy->Close();
    return false;
    }

  std::stringstream status;
  status << "Pulled collection " << collection->GetName();
  this->Log->Status(status.str());

  // Pull any parents we need
  if(parentId == NO_PARENT)
    {
    bool recurse = this->Recursive;
    this->Recursive = false;
    std::string handle = this->ResourceHandle;
    this->ResourceHandle = collection->GetParent();

    this->PullCommunity(NO_PARENT);
    this->DatabaseProxy->Open();
    CHANGE_DIR(this->LastDir.c_str());

    this->ResourceHandle = handle;
    this->Recursive = recurse;
    parentId = this->LastId;
    }

  int id = this->DatabaseProxy->AddResource(midasResourceType::COLLECTION,
    collection->GetUuid(), WORKING_DIR() + "/" + collection->GetName(),
    collection->GetName(), midasResourceType::COMMUNITY, parentId, 0);
  collection->SetId(id);
  this->DatabaseProxy->SaveInfo(collection);
  this->DatabaseProxy->Close();
  this->LastId = id;

  if(!kwsys::SystemTools::FileIsDirectory(collection->GetName().c_str()))
    {
    MKDIR(collection->GetName().c_str());
    }
  this->LastDir = WORKING_DIR() + "/" + collection->GetName();

  if(this->Recursive)
    {
    std::string temp = WORKING_DIR();
    CHANGE_DIR(collection->GetName().c_str());
    for(std::vector<mdo::Item*>::const_iterator i = 
        collection->GetItems().begin();
        i != collection->GetItems().end(); ++i)
      {
      std::stringstream s;
      s << (*i)->GetId();
      this->SetResourceHandle(s.str());
      this->PullItem(id);
      }
    CHANGE_DIR(temp.c_str());
    }

  delete collection;
  return true;
}

/**
 * Helper function to search the tree for a community with the given id.
 */
mdo::Community* FindInTree(mdo::Community* root, int id)
{
  for(std::vector<mdo::Community*>::const_iterator i =
      root->GetCommunities().begin(); i != root->GetCommunities().end(); ++i)
    {
    if((*i)->GetId() == id)
      {
      return *i;
      }
    mdo::Community* result = FindInTree(*i, id);
    if(result != NULL)
      {
      return result;
      }
    }
  return NULL;
}

//-------------------------------------------------------------------
bool midasSynchronizer::PullCommunity(int parentId)
{
  if(this->ShouldCancel)
    {
    return false;
    }
  this->DatabaseProxy->Open();

  mws::Community remote;
  mdo::Community* community = new mdo::Community;
  community->SetId(atoi(this->ResourceHandle.c_str()));
  remote.SetWebAPI(mws::WebAPI::Instance());
  remote.SetObject(community);

  this->Progress->SetIndeterminate();
  this->Log->Status("Fetching community information...");
  if(!remote.FetchTree())
    {
    std::stringstream text;
    text << "Unable to fetch the community via the Web API."
      << std::endl;
    Log->Error(text.str());
    this->DatabaseProxy->Close();
    return false;
    }

  std::stringstream status;
  status << "Pulled community " << community->GetName();
  this->Log->Status(status.str());

  community = FindInTree(community, atoi(this->ResourceHandle.c_str()));
  if(!community)
    {
    std::stringstream text;
    text << "Error: Community " << this->ResourceHandle 
      << " does not exist." << std::endl;
    Log->Error(text.str());
    this->DatabaseProxy->Close();
    return false;
    }
  remote.SetObject(community);
  remote.Fetch();

  // Pull any parents we need
  if(parentId == NO_PARENT && community->GetParentId())
    {
    bool recurse = this->Recursive;
    this->Recursive = false;
    std::string handle = this->ResourceHandle;
    this->ResourceHandle = community->GetParent();

    this->PullCommunity(NO_PARENT);
    this->DatabaseProxy->Open();
    CHANGE_DIR(this->LastDir.c_str());

    this->ResourceHandle = handle;
    this->Recursive = recurse;
    parentId = this->LastId;
    }

  std::string topLevelDir = WORKING_DIR();

  if(!kwsys::SystemTools::FileIsDirectory(community->GetName().c_str()))
    {
    MKDIR(community->GetName().c_str());
    }
  this->LastDir = WORKING_DIR() + "/" + community->GetName();

  int id = this->DatabaseProxy->AddResource(midasResourceType::COMMUNITY,
    community->GetUuid(), WORKING_DIR() + "/" + community->GetName(),
    community->GetName(), midasResourceType::COMMUNITY, parentId, 0);
  community->SetId(id);
  this->DatabaseProxy->SaveInfo(community);
  this->DatabaseProxy->Close();
  this->LastId = id;

  if(this->Recursive)
    {
    CHANGE_DIR(community->GetName().c_str());
    // Pull everything under this community.
    this->RecurseCommunities(id, community);
    }

  // Revert working dir to top level
  CHANGE_DIR(topLevelDir.c_str());
  delete community;
  return true;
}

/**
 * Function to recursively pull all collections
 * underneath the given community, including in subcommunities.
 */
void midasSynchronizer::RecurseCommunities(int parentId, 
                                           mdo::Community* community)
{
  for(std::vector<mdo::Collection*>::const_iterator i = 
      community->GetCollections().begin();
      i != community->GetCollections().end(); ++i)
    {
    std::stringstream s;
    s << (*i)->GetId();
    this->SetResourceHandle(s.str());
    this->PullCollection(parentId);
    }
  for(std::vector<mdo::Community*>::const_iterator i =
      community->GetCommunities().begin();
      i != community->GetCommunities().end(); ++i)
    {
    std::stringstream s;
    s << (*i)->GetId();
    this->SetResourceHandle(s.str());
    this->PullCommunity(parentId);
    }
}

//-------------------------------------------------------------------
bool midasSynchronizer::PullItem(int parentId)
{
  if(this->ShouldCancel)
    {
    return false;
    }
  this->DatabaseProxy->Open();

  mws::Item remote;
  mdo::Item* item = new mdo::Item;
  item->SetId(atoi(this->GetResourceHandle().c_str()));
  remote.SetWebAPI(mws::WebAPI::Instance());
  remote.SetObject(item);

  this->Progress->SetIndeterminate();
  this->Log->Status("Fetching item information...");
  if(!remote.Fetch())
    {
    std::stringstream text;
    text << "Unable to fetch the item via the Web API"
      << std::endl;
    Log->Error(text.str());
    this->DatabaseProxy->Close();
    return false;
    }

  std::stringstream status;
  status << "Pulled item " << item->GetTitle();
  this->Log->Status(status.str());

  // Pull any parents we need
  if(parentId == NO_PARENT)
    {
    bool recurse = this->Recursive;
    this->Recursive = false;
    std::string handle = this->ResourceHandle;
    this->ResourceHandle = item->GetParent();

    this->PullCollection(NO_PARENT);
    this->DatabaseProxy->Open();
    CHANGE_DIR(this->LastDir.c_str());

    this->ResourceHandle = handle;
    this->Recursive = recurse;
    parentId = this->LastId;
    }
  
  std::stringstream altTitle;
  altTitle << "item" << item->GetId();
  std::string title = item->GetTitle() == "" ? altTitle.str() :
    item->GetTitle();

  if(!kwsys::SystemTools::FileIsDirectory(title.c_str()))
    {
    MKDIR(title.c_str());
    }
  this->LastDir = WORKING_DIR() + "/" + title;

  int id = this->DatabaseProxy->AddResource(midasResourceType::ITEM,
    item->GetUuid(), WORKING_DIR() + "/" + title, item->GetTitle(),
    midasResourceType::COLLECTION, parentId, 0);
  item->SetId(id);
  this->DatabaseProxy->SaveInfo(item);
  this->DatabaseProxy->Close();
  this->LastId = id;

  if(this->Recursive)
    {
    std::string temp = WORKING_DIR();
    CHANGE_DIR(title.c_str());
    for(std::vector<mdo::Bitstream*>::const_iterator i = 
        item->GetBitstreams().begin();
        i != item->GetBitstreams().end(); ++i)
      {
      std::stringstream s;
      s << (*i)->GetId();
      this->SetResourceHandle(s.str());
      this->PullBitstream(id);
      }
    CHANGE_DIR(temp.c_str());
    }

  delete item;
  return true;
}

//-------------------------------------------------------------------
int midasSynchronizer::Push()
{
  if(this->GetServerURL() == "")
    {
    std::stringstream text;
    text << "You must specify a server url. No last used URL "
      "exists in the database." << std::endl;
    Log->Error(text.str());
    return MIDAS_NO_URL;
    }
  this->DatabaseProxy->Open();
  // breaking proxy rules for the sake of efficiency here
  this->DatabaseProxy->GetDatabase()->ExecuteQuery(
    "SELECT uuid FROM dirty_resource");

  bool success = true;
  std::vector<std::string> uuids;

  while(this->DatabaseProxy->GetDatabase()->GetNextRow())
    {
    // store the list so we can delete from dirty list while iterating it
    uuids.push_back(this->DatabaseProxy->GetDatabase()->GetValueAsString(0));
    }

  if(!uuids.size())
    {
    std::stringstream text;
    text << "There are no staged resources to push." << std::endl;
    Log->Error(text.str());
    }

  for(std::vector<std::string>::iterator i = uuids.begin(); i != uuids.end();
      ++i)
    {
    midasResourceRecord record = this->DatabaseProxy->GetRecordByUuid(*i);

    switch(record.Type)
      {
      case midasResourceType::BITSTREAM:
        success &= this->PushBitstream(record.Id);
        break;
      case midasResourceType::COLLECTION:
        success &= this->PushCollection(record.Id);
        break;
      case midasResourceType::COMMUNITY:
        success &= this->PushCommunity(record.Id);
        break;
      case midasResourceType::ITEM:
        success &= this->PushItem(record.Id);
        break;
      default:
        return MIDAS_NO_RTYPE;
      }

    if(mws::WebAPI::Instance()->GetErrorCode() == INVALID_POLICY
      && this->Authenticator->IsAnonymous())
      {
      std::stringstream text;
      text << "You are not logged in. Please specify a user "
        "profile." << std::endl;
      Log->Error(text.str());
      this->DatabaseProxy->Close();
      return MIDAS_LOGIN_FAILED;
      }
    }
  this->DatabaseProxy->Close();
  return success ? MIDAS_OK : MIDAS_FAILURE;
}

//-------------------------------------------------------------------
int midasSynchronizer::GetServerParentId(midasResourceType::ResourceType type,
                                         int parentId)
{
  if(parentId)
    {
    std::stringstream fields;
    std::string server_parentId;
    // Get uuid from parent id/type
    std::string parentUuid = this->DatabaseProxy->GetUuid(type, parentId);

    // Get server-side id of parent from the uuid
    fields << "midas.resource.get?uuid=" << parentUuid;
    mws::RestXMLParser parser;
    parser.AddTag("/rsp/id", server_parentId);
    mws::WebAPI::Instance()->GetRestAPI()->SetXMLParser(&parser);
    mws::WebAPI::Instance()->Execute(fields.str().c_str());
    parentId = atoi(server_parentId.c_str());
    }
  return parentId;
}

//-------------------------------------------------------------------
bool midasSynchronizer::PushBitstream(int id)
{
  if(this->ShouldCancel)
    {
    return false;
    }
  std::string uuid = this->DatabaseProxy->GetUuid(
    midasResourceType::BITSTREAM, id);
  std::string name = this->DatabaseProxy->GetName(
    midasResourceType::BITSTREAM, id);

  midasResourceRecord record = this->DatabaseProxy->GetRecordByUuid(uuid);

  if(kwsys::SystemTools::FileLength(record.Path.c_str()) == 0)
    {
    std::stringstream text;
    text << "Error: \"" << record.Path << "\" is 0 bytes. You "
      "may not push an empty bitstream." << std::endl;
    Log->Error(text.str());
    return false;
    }

  if(record.Parent == 0)
    {
    record.Parent = this->GetServerParentId(midasResourceType::ITEM,
      this->DatabaseProxy->GetParentId(midasResourceType::BITSTREAM, id));
    }
  if(record.Parent == 0)
    {
    std::stringstream text;
    text << "The parent of bitstream \"" << name <<
      "\" could not be resolved." << std::endl;
    Log->Error(text.str());
    return false;
    }

  std::stringstream status;
  status << "Uploading bitstream " << name << "...";
  this->Log->Status(status.str());
  std::stringstream fields;
  fields << "midas.upload.bitstream?uuid=" << uuid << "&itemid="
    << record.Parent;

  if(this->Progress)
    {
    mws::WebAPI::Instance()->GetRestAPI()->SetProgressCallback(
      ProgressCallback, this->Progress);
    this->Progress->SetMessage(name);
    this->Progress->ResetProgress();
    }
  mws::RestXMLParser parser;
  mws::WebAPI::Instance()->GetRestAPI()->SetXMLParser(&parser);
  bool ok = mws::WebAPI::Instance()->UploadFile(fields.str().c_str(),
                                     record.Path.c_str());

  if(ok)
    {
    // Clear dirty flag on the resource
    this->DatabaseProxy->ClearDirtyResource(uuid);
    std::stringstream text;
    text << "Pushed bitstream " << name << std::endl;
    Log->Message(text.str());
    }
  else
    {
    std::stringstream text;
    text << "Failed to push bitstream " << name << ": " <<
      mws::WebAPI::Instance()->GetErrorMessage() << std::endl;
    Log->Error(text.str());
    }
  return ok;
}

//-------------------------------------------------------------------
bool midasSynchronizer::PushCollection(int id)
{
  if(this->ShouldCancel)
    {
    return false;
    }
  std::string uuid = this->DatabaseProxy->GetUuid(
    midasResourceType::COLLECTION, id);
  std::string name = this->DatabaseProxy->GetName(
    midasResourceType::COLLECTION, id);

  midasResourceRecord record = this->DatabaseProxy->GetRecordByUuid(uuid);
  if(record.Parent == 0)
    {
    record.Parent = this->GetServerParentId(midasResourceType::COMMUNITY,
      this->DatabaseProxy->GetParentId(midasResourceType::COLLECTION, id));
    }
  if(record.Parent == 0)
    {
    std::stringstream text;
    text << "The parent of collection \"" << name <<
      "\" could not be resolved." << std::endl;
    Log->Error(text.str());
    return false;
    }

  this->Progress->SetIndeterminate();
  std::stringstream status;
  status << "Uploading collection " << name << "...";
  this->Log->Status(status.str());

  std::stringstream fields;
  fields << "midas.collection.create?uuid=" << uuid << "&name=" <<
    midasUtils::EscapeForURL(name) << "&parentid=" << record.Parent;

  mws::RestXMLParser parser;
  mws::WebAPI::Instance()->SetPostData("");
  mws::WebAPI::Instance()->GetRestAPI()->SetXMLParser(&parser);
  bool success = mws::WebAPI::Instance()->Execute(fields.str().c_str());
  if(success)
    {
    // Clear dirty flag on the resource
    this->DatabaseProxy->ClearDirtyResource(uuid);
    std::stringstream text;
    text << "Pushed collection " << name << std::endl;
    Log->Message(text.str());
    }
  else
    {
    std::stringstream text;
    text << "Failed to push collection " << name << ": " <<
    mws::WebAPI::Instance()->GetErrorMessage() << std::endl;
    Log->Error(text.str());
    }
  return success;
}

//-------------------------------------------------------------------
bool midasSynchronizer::PushCommunity(int id)
{
  if(this->ShouldCancel)
    {
    return false;
    }
  std::string uuid = this->DatabaseProxy->GetUuid(
    midasResourceType::COMMUNITY, id);
  std::string name = this->DatabaseProxy->GetName(
    midasResourceType::COMMUNITY, id);

  midasResourceRecord record = this->DatabaseProxy->GetRecordByUuid(uuid);
  if(record.Parent == 0)
    {
    record.Parent = this->GetServerParentId(midasResourceType::COMMUNITY,
      this->DatabaseProxy->GetParentId(midasResourceType::COMMUNITY, id));
    }

  this->Progress->SetIndeterminate();
  std::stringstream status;
  status << "Uploading community " << name << "...";
  this->Log->Status(status.str());

  // Create new community on server
  std::stringstream fields;
  fields << "midas.community.create?uuid=" << uuid << "&name=" << 
    midasUtils::EscapeForURL(name) << "&parentid=" << record.Parent;

  mws::RestXMLParser parser;
  mws::WebAPI::Instance()->SetPostData("");
  mws::WebAPI::Instance()->GetRestAPI()->SetXMLParser(&parser);
  bool success = mws::WebAPI::Instance()->Execute(fields.str().c_str());
  if(success)
    {
    // Clear dirty flag on the resource
    this->DatabaseProxy->ClearDirtyResource(uuid);
    std::stringstream text;
    text << "Pushed community " << name << std::endl;
    Log->Message(text.str());
    }
  else
    {
    std::stringstream text; 
    text << "Failed to push community " << name << ": " <<
    mws::WebAPI::Instance()->GetErrorMessage() << std::endl;
    Log->Error(text.str());
    }
  return success;
}

//-------------------------------------------------------------------
bool midasSynchronizer::PushItem(int id)
{
  if(this->ShouldCancel)
    {
    return false;
    }
  std::string uuid = this->DatabaseProxy->GetUuid(
    midasResourceType::ITEM, id);
  std::string name = this->DatabaseProxy->GetName(
    midasResourceType::ITEM, id);

  midasResourceRecord record = this->DatabaseProxy->GetRecordByUuid(uuid);
  if(record.Parent == 0)
    {
    record.Parent = this->GetServerParentId(midasResourceType::COLLECTION,
      this->DatabaseProxy->GetParentId(midasResourceType::ITEM, id));
    }
  if(record.Parent == 0)
    {
    std::stringstream text;
    text << "The parent of item \"" << name <<
      "\" could not be resolved." << std::endl;
    Log->Error(text.str());
    return false;
    }

  this->Progress->SetIndeterminate();
  std::stringstream status;
  status << "Uploading item " << name << "...";
  this->Log->Status(status.str());
  
  std::stringstream fields;
  fields << "midas.item.create?uuid=" << uuid << "&name=" <<
    midasUtils::EscapeForURL(name) << "&parentid=" << record.Parent;

  mws::RestXMLParser parser;
  mws::WebAPI::Instance()->GetRestAPI()->SetXMLParser(&parser);
  mws::WebAPI::Instance()->SetPostData("");
  bool success = mws::WebAPI::Instance()->Execute(fields.str().c_str());
  if(success)
    {
    // Clear dirty flag on the resource
    this->DatabaseProxy->ClearDirtyResource(uuid);
    std::stringstream text;
    text << "Pushed item " << name << std::endl;
    Log->Message(text.str());
    }
  else
    {
    std::stringstream text;
    text << "Failed to push item " << name << ": " <<
    mws::WebAPI::Instance()->GetErrorMessage() << std::endl;
    Log->Error(text.str());
    }
  return success;
}

//-------------------------------------------------------------------
void midasSynchronizer::Reset()
{
  this->ShouldCancel = false;
  this->SetOperation(midasSynchronizer::OPERATION_NONE);
  this->SetResourceHandle("");
  this->SetResourceType(midasResourceType::NONE);
  this->SetParentId(0);
  this->Log->Status("");
}

//-------------------------------------------------------------------
void midasSynchronizer::Cancel()
{
  this->ShouldCancel = true;
}
