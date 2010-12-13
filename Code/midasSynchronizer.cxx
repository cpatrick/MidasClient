/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "midasSynchronizer.h"

#include "mwsWebAPI.h"
#include "mdoCommunity.h"
#include "mdsCommunity.h"
#include "mwsCommunity.h"
#include "mdoCollection.h"
#include "mdsCollection.h"
#include "mwsCollection.h"
#include "mdsBitstream.h"
#include "mwsBitstream.h"
#include "mdoItem.h"
#include "mdsItem.h"
#include "mwsItem.h"
#include "midasProgressReporter.h"
#include "midasDatabaseProxy.h"
#include "midasStdOutLog.h"
#include "mwsRestXMLParser.h"

#define WORKING_DIR kwsys::SystemTools::GetCurrentWorkingDirectory
#define CHANGE_DIR kwsys::SystemTools::ChangeDirectory
#define MKDIR kwsys::SystemTools::MakeDirectory

#define NO_PARENT -1

midasSynchronizer::midasSynchronizer()
{
  this->Recursive = false;
  this->ShouldCancel = false;
  this->PathMode = false;
  this->Operation = OPERATION_NONE;
  this->ResourceType = midasResourceType::NONE;
  this->ServerURL = "";
  this->ServerHandle = "";
  this->ClientHandle = "";
  this->Uuid = "";
  this->Progress = NULL;
  this->Log = new midasStdOutLog();
  this->Database = "";
  this->DatabaseProxy = NULL;
  this->CurrentBitstreams = 0;
  this->TotalBitstreams = 0;
  this->Authenticator = new midasAuthenticator;
  this->Authenticator->SetLog(this->Log);
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

void midasSynchronizer::SetDatabase(std::string path)
{
  if(path == this->Database)
    {
    return;
    }

  if(!midasUtils::IsDatabaseValid(path))
    {
    std::stringstream text;
    text << "No valid database found at " << path <<
        ". Creating new database..." << std::endl;
    this->GetLog()->Message(text.str());

    if(!midasUtils::CreateNewDatabase(path))
      {
      std::stringstream text;
      text << "Fatal: failed to create database at " << path << std::endl;
      this->GetLog()->Error(text.str());
      kwsys::SystemTools::RemoveFile(path.c_str());
      exit(-1);
      }
    else
      {
      std::stringstream text;
      text << "Database created successfully at " << path << std::endl;
      this->GetLog()->Message(text.str());
      }
    }
  this->Database = path;
  delete this->DatabaseProxy;
  this->DatabaseProxy = new midasDatabaseProxy(path);
  this->DatabaseProxy->SetLog(this->GetLog());
  this->Authenticator->SetDatabase(path);
  this->SetServerURL(this->GetServerURL());
}

void midasSynchronizer::SetLog(midasLog* log)
{
  delete this->Log;
  this->Log = log;
  this->Authenticator->SetLog(log);
}

midasDatabaseProxy* midasSynchronizer::GetDatabase()
{
  return this->DatabaseProxy;
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

void midasSynchronizer::SetClientHandle(std::string handle)
{
  this->ClientHandle = handle;
}

std::string midasSynchronizer::GetClientHandle()
{
  return this->ClientHandle;
}

void midasSynchronizer::SetServerHandle(std::string handle)
{
  this->ServerHandle = handle;
}

std::string midasSynchronizer::GetServerHandle()
{
  return this->ServerHandle;
}

std::string midasSynchronizer::GetServerURL()
{
  if(this->ServerURL == "")
    {
    this->ServerURL =
      this->DatabaseProxy->GetSetting(midasDatabaseProxy::LAST_URL);
    }
  return this->ServerURL;
}

void midasSynchronizer::SetServerURL(std::string url)
{
  mws::WebAPI::Instance()->SetServerUrl(url.c_str());
  this->Authenticator->SetServerURL(url.c_str());
  this->DatabaseProxy->SetSetting(midasDatabaseProxy::LAST_URL, url);
  this->ServerURL = url;
}

//-------------------------------------------------------------------
std::vector<midasStatus> midasSynchronizer::GetStatusEntries()
{
  return this->DatabaseProxy->GetStatusEntries();
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
  this->CountBitstreams();

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
    case OPERATION_UPLOAD:
      rc = this->Upload();
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
  if(this->ServerHandle != "")
    {
    int type = this->ResourceType;
    this->ResourceType = midasUtils::GetParentType(type);
    int rc = this->Pull();
    if(rc != MIDAS_OK)
      {
      return rc;
      }
    this->ResourceType = type;
    }

  std::string path = this->ResolveAddPath();

  if(path == "")
    {
    std::stringstream text;
    text << "Error: \"" << this->ClientHandle << "\" does "
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
    this->ResourceType != midasResourceType::BITSTREAM &&
    this->ServerHandle == "")
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

  std::string parentUuid = this->ServerHandle == "" ?
    this->DatabaseProxy->GetUuidFromPath(parentDir) :
    this->Uuid;

  parentDir = this->DatabaseProxy->GetRecordByUuid(parentUuid).Path;
  if(this->ServerHandle != "")
    {
    path = parentDir + "/" + name;

    if(this->ResourceType == midasResourceType::BITSTREAM)
      {
      if(!kwsys::SystemTools::CopyAFile(this->ClientHandle.c_str(),
         parentDir.c_str()))
        {
        std::stringstream text;
        text << "Error: failed to copy file into item directory" << std::endl;
        this->Log->Error(text.str());
        return MIDAS_FAILURE;
        }
      this->ClientHandle = parentDir + "/" +
        kwsys::SystemTools::GetFilenameName(this->ClientHandle);
      }
    else
      {
      if(!kwsys::SystemTools::MakeDirectory(path.c_str()))
        {
        std::stringstream text;
        text << "Error: Failed to create directory " << path << std::endl;
        this->Log->Error(text.str());
        return MIDAS_FAILURE;
        }
      }
    }

  if(this->DatabaseProxy->GetUuidFromPath(path) != "")
    {
    std::stringstream text;
    text << "Error: \"" << path << "\" is already in the "
      "database." << std::endl;
    this->Log->Error(text.str());
    return MIDAS_DUPLICATE_PATH;
    }

  if(parentUuid == "" && this->ResourceType != midasResourceType::COMMUNITY)
    {
    std::stringstream text;
    text << "The parent of this resource could not be resolved." << std::endl;
    this->Log->Error(text.str());
    return MIDAS_INVALID_PARENT;
    }

  int id = this->DatabaseProxy->AddResource(this->ResourceType, uuid, 
    path, name, parentUuid, atoi(this->ServerHandle.c_str()));

  if(id <= 0)
    {
    std::stringstream text;
    text << "AddResource failed: " << path << std::endl;
    this->Log->Error(text.str());
    return MIDAS_FAILURE;
    }

  // Size of bitstream needs to be saved automatically
  if(this->ResourceType == midasResourceType::BITSTREAM)
    {
    std::stringstream size;
    size << kwsys::SystemTools::FileLength(path.c_str());
    mdo::Bitstream bitstream;
    bitstream.SetId(id);
    bitstream.SetName(name.c_str());
    bitstream.SetSize(size.str());
    bitstream.SetLastModified(kwsys::SystemTools::ModifiedTime(path.c_str()));

    mds::Bitstream mdsBitstream;
    mdsBitstream.SetObject(&bitstream);
    mdsBitstream.SetDatabase(this->DatabaseProxy);
    mdsBitstream.Commit();
    }
  this->DatabaseProxy->MarkDirtyResource(uuid, midasDirtyAction::ADDED);

  return MIDAS_OK;
}

//-------------------------------------------------------------------
int midasSynchronizer::Clean()
{
  this->DatabaseProxy->Clean();
  return MIDAS_OK;
}

//-------------------------------------------------------------------
int midasSynchronizer::Clone()
{
  this->ChangeToRootDir();

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
  if(dltotal > 0 && dlnow >0)
    {
    out->UpdateProgress(dlnow, dltotal);
    }

  if(ultotal >0 && ulnow > 0)
    {
    out->UpdateProgress(ulnow, ultotal);
    }
  return 0;
}

//-------------------------------------------------------------------
int midasSynchronizer::Pull()
{
  this->ChangeToRootDir();

  if(this->ResourceType == midasResourceType::NONE && !this->IsPathMode())
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

  if(this->IsPathMode())
    {
    if(!this->ConvertPathToId())
      {
      std::stringstream text;
      text << "The path provided could not be resolved.  The MIDAS server "
        "might not support converting paths to IDs." << std::endl;
      Log->Error(text.str());
      return MIDAS_INVALID_SERVER_PATH;
      }
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
  bitstream->SetId(atoi(this->ServerHandle.c_str()));
  remote.SetWebAPI(mws::WebAPI::Instance());
  remote.SetObject(bitstream);
  
  this->Progress->SetIndeterminate();
  this->Log->Status("Fetching bitstream information...");
  if(!remote.Fetch())
    {
    std::stringstream text;
    text << "Unable to get bitstream via the web API." << std::endl;
    Log->Error(text.str());
    return false;
    }

  // Pull any parents we need
  if(parentId == NO_PARENT)
    {
    bool recurse = this->Recursive;
    this->Recursive = false;
    std::string handle = this->ServerHandle;
    this->ServerHandle = bitstream->GetParent();

    this->PullItem(NO_PARENT);
    CHANGE_DIR(this->LastDir.c_str());

    this->ServerHandle = handle;
    this->Recursive = recurse;
    parentId = this->LastId;
    }

  if(bitstream->GetName() == "")
    {
    std::stringstream text;
    text << "Bitstream " << this->ServerHandle <<
      " does not exist." << std::endl;
    Log->Error(text.str());
    return false;
    }

  midasResourceRecord record =
    this->DatabaseProxy->GetRecordByUuid(bitstream->GetUuid());

  this->Progress->SetMessage(bitstream->GetName());

  this->CurrentBitstreams++;
  this->Progress->UpdateOverallProgress(this->CurrentBitstreams);
  this->Progress->UpdateProgress(0, 0);

  //TODO check md5 sum of file at location against server's checksum?
  if(record.Path != "" &&
     kwsys::SystemTools::FileExists(record.Path.c_str(), true))
    {
    //we already have this bitstream, no need to download again
    return true;
    }

  std::stringstream fields;
  fields << "midas.bitstream.download?id=" << this->GetServerHandle();

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

  if(!mws::WebAPI::Instance()->DownloadFile(fields.str().c_str(),
                             bitstream->GetName().c_str()))
    {
    //delete the partial data and error out.
    kwsys::SystemTools::RemoveFile(bitstream->GetName().c_str());

    std::stringstream text;
    if(this->ShouldCancel)
      {
      text << "Download canceled by user.";
      this->Log->Message(text.str());
      }
    else
      {
      text << "Connection error during download. "
        << mws::WebAPI::Instance()->GetErrorMessage();
      this->Log->Error(text.str());
      }
    this->Log->Status(text.str());
    return false;
    }

  int id = this->DatabaseProxy->AddResource(midasResourceType::BITSTREAM,
    bitstream->GetUuid(), WORKING_DIR() + "/" + bitstream->GetName(),
    bitstream->GetName(), midasResourceType::ITEM, parentId, 0);
  bitstream->SetId(id);
  std::string path = WORKING_DIR() + "/" + bitstream->GetName();
  bitstream->SetLastModified(kwsys::SystemTools::ModifiedTime(path.c_str()));
  
  mds::Bitstream mdsBitstream;
  mdsBitstream.SetObject(bitstream);
  mdsBitstream.SetDatabase(this->DatabaseProxy);
  mdsBitstream.Commit();
  
  return true;
}

//-------------------------------------------------------------------
bool midasSynchronizer::PullCollection(int parentId)
{
  if(this->ShouldCancel)
    {
    return false;
    }

  mws::Collection remote;
  mdo::Collection* collection = new mdo::Collection;
  collection->SetId(atoi(this->GetServerHandle().c_str()));
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
    std::string handle = this->ServerHandle;
    this->ServerHandle = collection->GetParent();

    this->PullCommunity(NO_PARENT);
    CHANGE_DIR(this->LastDir.c_str());

    this->ServerHandle = handle;
    this->Recursive = recurse;
    parentId = this->LastId;
    }

  int id = this->DatabaseProxy->AddResource(midasResourceType::COLLECTION,
    collection->GetUuid(), WORKING_DIR() + "/" + collection->GetName(),
    collection->GetName(), midasResourceType::COMMUNITY, parentId, 0);
  this->LastId = id;
  collection->SetId(id);

  mds::Collection mdsColl;
  mdsColl.SetDatabase(this->DatabaseProxy);
  mdsColl.SetObject(collection);
  if(!mdsColl.Commit())
    {
    return false;
    }

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
      this->SetServerHandle(s.str());
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

  mws::Community remote;
  mdo::Community* community = new mdo::Community;
  community->SetId(atoi(this->ServerHandle.c_str()));
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
    return false;
    }

  std::stringstream status;
  status << "Pulled community " << community->GetName();
  this->Log->Status(status.str());

  community = FindInTree(community, atoi(this->ServerHandle.c_str()));
  if(!community)
    {
    std::stringstream text;
    text << "Error: Community " << this->ServerHandle 
      << " does not exist." << std::endl;
    Log->Error(text.str());
    return false;
    }
  remote.SetObject(community);
  remote.Fetch();

  // Pull any parents we need
  if(parentId == NO_PARENT && community->GetParentId())
    {
    bool recurse = this->Recursive;
    this->Recursive = false;
    std::string handle = this->ServerHandle;
    this->ServerHandle = community->GetParent();

    this->PullCommunity(NO_PARENT);
    CHANGE_DIR(this->LastDir.c_str());

    this->ServerHandle = handle;
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
  this->LastId = id;

  mds::Community mdsComm;
  mdsComm.SetDatabase(this->DatabaseProxy);
  mdsComm.SetObject(community);
  if(!mdsComm.Commit())
    {
    return false;
    }

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
    this->SetServerHandle(s.str());
    this->PullCollection(parentId);
    }
  for(std::vector<mdo::Community*>::const_iterator i =
      community->GetCommunities().begin();
      i != community->GetCommunities().end(); ++i)
    {
    std::stringstream s;
    s << (*i)->GetId();
    this->SetServerHandle(s.str());
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

  mws::Item remote;
  mdo::Item* item = new mdo::Item;
  item->SetId(atoi(this->GetServerHandle().c_str()));
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
    std::string handle = this->ServerHandle;
    this->ServerHandle = item->GetParent();

    this->PullCollection(NO_PARENT);
    CHANGE_DIR(this->LastDir.c_str());

    this->ServerHandle = handle;
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

  this->LastId = id;
  item->SetId(id);
  
  mds::Item mdsItem;
  mdsItem.SetDatabase(this->DatabaseProxy);
  mdsItem.SetObject(item);
  if(!mdsItem.Commit())
    {
    return false;
    }

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
      this->SetServerHandle(s.str());
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

  std::vector<midasStatus> dirties = this->GetStatusEntries();

  if(!dirties.size())
    {
    std::stringstream text;
    text << "There are no staged resources to push." << std::endl;
    Log->Error(text.str());
    }

  bool success = true;
  for(std::vector<midasStatus>::iterator i = dirties.begin();
      i != dirties.end(); ++i)
    {
    if(i->GetUUID() == "")
      {
      this->Log->Message("Skipping invalid dirty resource entry.\n");
      continue;
      }
    midasResourceRecord record =
      this->DatabaseProxy->GetRecordByUuid(i->GetUUID());

    switch(record.Type)
      {
      case midasResourceType::BITSTREAM:
        success &= this->PushBitstream(&record);
        break;
      case midasResourceType::COLLECTION:
        success &= this->PushCollection(&record);
        break;
      case midasResourceType::COMMUNITY:
        success &= this->PushCommunity(&record);
        break;
      case midasResourceType::ITEM:
        success &= this->PushItem(&record);
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
      return MIDAS_LOGIN_FAILED;
      }
    }
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
bool midasSynchronizer::PushBitstream(midasResourceRecord* record)
{
  if(this->ShouldCancel)
    {
    return false;
    }

  mds::Bitstream mdsBitstream;
  mdo::Bitstream* bitstream = new mdo::Bitstream;
  bitstream->SetId(record->Id);
  mdsBitstream.SetObject(bitstream);
  mdsBitstream.SetDatabase(this->DatabaseProxy);
  mdsBitstream.Fetch();

  std::string name = bitstream->GetName();
  std::string size = bitstream->GetSize();

  delete bitstream;

  this->CurrentBitstreams++;
  this->Progress->UpdateOverallProgress(this->CurrentBitstreams);
  this->Progress->SetMessage(name);

  if(midasUtils::GetFileLength(record->Path.c_str()) == 0)
    {
    std::stringstream text;
    text << "Error: \"" << record->Path << "\" is 0 bytes. You "
      "may not push an empty bitstream." << std::endl;
    Log->Error(text.str());
    return false;
    }

  if(record->Parent == 0)
    {
    record->Parent = this->GetServerParentId(midasResourceType::ITEM,
      this->DatabaseProxy->GetParentId(midasResourceType::BITSTREAM,
      record->Id));
    }
  if(record->Parent == 0)
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
  fields << "midas.upload.bitstream?uuid=" << record->Uuid << "&itemid="
    << record->Parent << "&mode=stream&filename="
    << midasUtils::EscapeForURL(name) << "&path="
    << midasUtils::EscapeForURL(name) << "&size=" << size;

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
                                     record->Path.c_str());

  if(ok)
    {
    // Clear dirty flag on the resource
    this->DatabaseProxy->ClearDirtyResource(record->Uuid);
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
bool midasSynchronizer::PushCollection(midasResourceRecord* record)
{
  if(this->ShouldCancel)
    {
    return false;
    }
  mdo::Collection coll;
  coll.SetId(record->Id);

  mds::Collection mdsColl;
  mdsColl.SetObject(&coll);
  mdsColl.SetDatabase(this->DatabaseProxy);
  mdsColl.Fetch();

  if(record->Parent == 0)
    {
    record->Parent = this->GetServerParentId(midasResourceType::COMMUNITY,
      this->DatabaseProxy->GetParentId(midasResourceType::COLLECTION,
      record->Id));
    }
  if(record->Parent == 0)
    {
    std::stringstream text;
    text << "The parent of collection \"" << coll.GetName() <<
      "\" could not be resolved." << std::endl;
    Log->Error(text.str());
    return false;
    }

  this->Progress->SetIndeterminate();
  std::stringstream status;
  status << "Uploading collection " << coll.GetName() << "...";
  this->Log->Status(status.str());

  std::stringstream fields;
  fields << "midas.collection.create?uuid=" << record->Uuid
    << "&parentid=" << record->Parent
    << "&name=" << midasUtils::EscapeForURL(coll.GetName())
    << "&description=" << midasUtils::EscapeForURL(coll.GetDescription())
    << "&introductorytext=" <<
    midasUtils::EscapeForURL(coll.GetIntroductoryText())
    << "&copyright=" << midasUtils::EscapeForURL(coll.GetCopyright());

  mws::RestXMLParser parser;
  mws::WebAPI::Instance()->SetPostData("");
  mws::WebAPI::Instance()->GetRestAPI()->SetXMLParser(&parser);
  bool success = mws::WebAPI::Instance()->Execute(fields.str().c_str());
  if(success)
    {
    // Clear dirty flag on the resource
    this->DatabaseProxy->ClearDirtyResource(record->Uuid);
    std::stringstream text;
    text << "Pushed collection " << coll.GetName() << std::endl;
    Log->Message(text.str());
    }
  else
    {
    std::stringstream text;
    text << "Failed to push collection " << coll.GetName() << ": " <<
    mws::WebAPI::Instance()->GetErrorMessage() << std::endl;
    Log->Error(text.str());
    }
  return success;
}

//-------------------------------------------------------------------
bool midasSynchronizer::PushCommunity(midasResourceRecord* record)
{
  if(this->ShouldCancel)
    {
    return false;
    }

  if(record->Parent == 0)
    {
    record->Parent = this->GetServerParentId(midasResourceType::COMMUNITY,
      this->DatabaseProxy->GetParentId(midasResourceType::COMMUNITY,
      record->Id));
    }

  mdo::Community comm;
  comm.SetId(record->Id);
  mds::Community mdsComm;
  mdsComm.SetDatabase(this->DatabaseProxy);
  mdsComm.SetObject(&comm);
  if(!mdsComm.Fetch())
    {
    return false;
    }

  this->Progress->SetIndeterminate();
  std::stringstream status;
  status << "Uploading community " << comm.GetName() << "...";
  this->Log->Status(status.str());

  // Create new community on server
  std::stringstream fields;
  fields << "midas.community.create?uuid=" << record->Uuid 
    << "&parentid=" << record->Parent
    << "&name=" << midasUtils::EscapeForURL(comm.GetName())
    << "&copyright=" << midasUtils::EscapeForURL(comm.GetCopyright())
    << "&introductorytext=" <<
    midasUtils::EscapeForURL(comm.GetIntroductoryText())
    << "&description=" << midasUtils::EscapeForURL(comm.GetDescription())
    << "&links=" << midasUtils::EscapeForURL(comm.GetLinks());

  mws::RestXMLParser parser;
  mws::WebAPI::Instance()->SetPostData("");
  mws::WebAPI::Instance()->GetRestAPI()->SetXMLParser(&parser);

  if(mws::WebAPI::Instance()->Execute(fields.str().c_str()))
    {
    // Clear dirty flag on the resource
    this->DatabaseProxy->ClearDirtyResource(record->Uuid);
    std::stringstream text;
    text << "Pushed community " << comm.GetName() << std::endl;
    Log->Message(text.str());
    Log->Status(text.str());
    return true;
    }
  else
    {
    std::stringstream text; 
    text << "Failed to push community " << comm.GetName() << ": " <<
    mws::WebAPI::Instance()->GetErrorMessage() << std::endl;
    Log->Error(text.str());
    Log->Status(text.str());
    return false;
    }
}

//-------------------------------------------------------------------
bool midasSynchronizer::PushItem(midasResourceRecord* record)
{
  if(this->ShouldCancel)
    {
    return false;
    }

  mdo::Item item;
  item.SetId(record->Id);

  mds::Item mdsItem;
  mdsItem.SetObject(&item);
  mdsItem.SetDatabase(this->DatabaseProxy);
  mdsItem.Fetch();

  if(record->Parent == 0)
    {
    record->Parent = this->GetServerParentId(midasResourceType::COLLECTION,
      this->DatabaseProxy->GetParentId(midasResourceType::ITEM,
      record->Id));
    }
  if(record->Parent == 0)
    {
    std::stringstream text;
    text << "The parent of item \"" << item.GetTitle() <<
      "\" could not be resolved." << std::endl;
    Log->Error(text.str());
    return false;
    }

  this->Progress->SetIndeterminate();
  std::stringstream status;
  status << "Uploading item " << item.GetTitle() << "...";
  this->Log->Status(status.str());
  
  std::stringstream fields;
  fields << "midas.item.create?uuid=" << record->Uuid
    << "&parentid=" << record->Parent
    << "&name=" << midasUtils::EscapeForURL(item.GetName())
    << "&abstract=" << midasUtils::EscapeForURL(item.GetAbstract())
    << "&description=" << midasUtils::EscapeForURL(item.GetDescription())
    << "&authors=" << midasUtils::EscapeForURL(item.GetAuthorsString())
    << "&keywords=" << midasUtils::EscapeForURL(item.GetKeywordsString());

  mws::RestXMLParser parser;
  mws::WebAPI::Instance()->GetRestAPI()->SetXMLParser(&parser);
  mws::WebAPI::Instance()->SetPostData("");

  if(mws::WebAPI::Instance()->Execute(fields.str().c_str()))
    {
    // Clear dirty flag on the resource
    this->DatabaseProxy->ClearDirtyResource(record->Uuid);
    std::stringstream text;
    text << "Pushed item " << item.GetTitle() << std::endl;
    Log->Message(text.str());
    return true;
    }
  else
    {
    std::stringstream text;
    text << "Failed to push item " << item.GetTitle() << ": " <<
    mws::WebAPI::Instance()->GetErrorMessage() << std::endl;
    Log->Error(text.str());
    return false;
    }
}

//-------------------------------------------------------------------
int midasSynchronizer::Upload()
{
  if(this->ClientHandle == "" && this->ServerHandle == "")
    {
    std::stringstream text;
    text << "You must set both a server and client handle "
      "in order to use the upload operation." << std::endl;
    this->Log->Error(text.str());
    return MIDAS_INVALID_PATH;
    }

  if(!kwsys::SystemTools::FileExists(this->ClientHandle.c_str(), true))
    {
    std::string originalPath = this->ClientHandle;
    this->ClientHandle = WORKING_DIR() + "/" + originalPath;
    if(!kwsys::SystemTools::FileExists(this->ClientHandle.c_str(), true))
      {
      std::stringstream text;
      text << "Error: The source file \"" << originalPath <<
        "\" does not exist." << std::endl;
      this->Log->Error(text.str());
      return MIDAS_INVALID_PATH;
      }
    }

  if(this->ServerURL == "")
    {
    std::stringstream text;
    text << "Error: server URL not set." << std::endl;
    this->Log->Error(text.str());
    return MIDAS_NO_URL;
    }

  if(!this->ConvertPathToId())
    {
    std::stringstream text;
    text << "Error: \"" << this->ServerHandle << "\" does not refer "
      "to a valid path on the MIDAS server." << std::endl;
    this->Log->Error(text.str());
    return MIDAS_INVALID_SERVER_PATH;
    }

  if(this->ResourceType != midasResourceType::ITEM)
    {
    std::stringstream text;
    text << "Error: \"" << this->ServerHandle << "\" must refer to an "
      "item on the MIDAS server. The given path is of type " <<
      midasUtils::GetTypeName(this->ResourceType) << "." << std::endl;
    this->Log->Error(text.str());
    return MIDAS_INVALID_TYPE;
    }

  if(this->GetAuthenticator()->GetProfile() == "")
    {
    std::stringstream text;
    text << "Error: you called Upload with no authentication "
      "credentials. You must authenticate in order to upload." << std::endl;
    this->Log->Error(text.str());
    return MIDAS_LOGIN_FAILED;
    }

  if(!this->GetAuthenticator()->Login(mws::WebAPI::Instance()))
    {
    std::stringstream text;
    text << "Error: Bad login credentials." << std::endl;
    this->Log->Error(text.str());
    return MIDAS_LOGIN_FAILED;
    }

  this->ResourceType = midasResourceType::BITSTREAM;

  int rc;
  if((rc = this->Add()) != MIDAS_OK)
    {
    return rc;
    }

  if((rc = this->Push()) != MIDAS_OK)
    {
    //TODO roll back the add?
    }

  return rc;
}

//-------------------------------------------------------------------
bool midasSynchronizer::ConvertPathToId()
{
  std::string type, id, uuid;
  mws::RestXMLParser parser;
  parser.AddTag("/rsp/type", type);
  parser.AddTag("/rsp/id", id);
  parser.AddTag("/rsp/uuid", uuid);

  mws::WebAPI::Instance()->SetPostData("");
  mws::WebAPI::Instance()->GetRestAPI()->SetXMLParser(&parser);

  std::stringstream fields;
  fields << "midas.convert.path.to.id?path=" <<
    midasUtils::EscapeForURL(this->ServerHandle);

  if(!mws::WebAPI::Instance()->Execute(fields.str().c_str()))
    {
    return false;
    }

  if(type == "" || id == "" || uuid == "")
    {
    return false;
    }

  this->SetResourceType(atoi(type.c_str()));
  this->SetServerHandle(id);
  this->Uuid = uuid;
  return true;
}

//-------------------------------------------------------------------
void midasSynchronizer::Reset()
{
  this->ShouldCancel = false;
  this->PathMode = false;
  this->SetOperation(midasSynchronizer::OPERATION_NONE);
  this->SetServerHandle("");
  this->SetClientHandle("");
  this->Uuid = "";
  this->SetResourceType(midasResourceType::NONE);
  this->Log->Status("");
  this->CurrentBitstreams = 0;
  this->TotalBitstreams = 0;
  this->Progress->SetMessage("");
}

//-------------------------------------------------------------------
void midasSynchronizer::Cancel()
{
  this->ShouldCancel = true;
  mws::WebAPI::Instance()->GetRestAPI()->Cancel();
}

//-------------------------------------------------------------------
void midasSynchronizer::ChangeToRootDir()
{
  std::string wdir;
  
  if(this->GetAuthenticator()->GetProfile() != "")
    {
    wdir = this->DatabaseProxy->GetAuthProfile(
      this->GetAuthenticator()->GetProfile()).RootDir;
    }

  // If no profile-specific setting exists, fall back to global root dir
  if(wdir == "")
    {
    wdir = this->DatabaseProxy->GetSetting(midasDatabaseProxy::ROOT_DIR);
    }

  if(wdir != "" && kwsys::SystemTools::FileIsDirectory(wdir.c_str()))
    {
    CHANGE_DIR(wdir.c_str());
    }
}

//-------------------------------------------------------------------
std::string midasSynchronizer::ResolveAddPath()
{
  std::string path;
  if(kwsys::SystemTools::FileIsFullPath(this->ClientHandle.c_str()))
    {
    path = this->ClientHandle;
    }
  else if(kwsys::SystemTools::FileExists(this->ClientHandle.c_str()))
    {
    path = WORKING_DIR() + "/" + this->ClientHandle;
    }
  else if(this->ResourceType != midasResourceType::BITSTREAM)
    {
    path = this->ClientHandle;
    }

  return path;
}

void midasSynchronizer::CountBitstreams()
{
  if(this->Operation == midasSynchronizer::OPERATION_PULL
     && this->Recursive)
    {
    if(this->ResourceType == midasResourceType::BITSTREAM)
      {
      this->TotalBitstreams = 1;
      return;
      }
    std::string count;
    mws::RestXMLParser parser;
    parser.AddTag("/rsp/count", count);

    mws::WebAPI::Instance()->SetPostData("");
    mws::WebAPI::Instance()->GetRestAPI()->SetXMLParser(&parser);

    std::stringstream fields;
    fields << "midas.bitstream.count?id=" << this->ServerHandle
      << "&type=" << this->ResourceType;

    this->Progress->SetIndeterminate();
    this->Log->Status("Counting total bitstreams under the object...");
    mws::WebAPI::Instance()->Execute(fields.str().c_str());
    this->Progress->ResetProgress();
    this->Log->Status("");

    this->TotalBitstreams = atoi(count.c_str());
    this->Progress->SetMaxOverall(this->TotalBitstreams);
    this->Progress->UpdateOverallProgress(0);
    }
  else if(this->Operation == midasSynchronizer::OPERATION_PUSH)
    {
    int count = 0;
    std::vector<midasStatus> status = this->DatabaseProxy->GetStatusEntries();

    for(std::vector<midasStatus>::iterator i = status.begin();
        i != status.end(); ++i)
      {
      if(i->GetType() == midasResourceType::BITSTREAM)
        {
        count++;
        }
      }
    this->TotalBitstreams = count;
    this->Progress->SetMaxOverall(count);
    this->Progress->UpdateOverallProgress(0);
    }
}
