/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <mwsWebAPI.h>
#include <mwsCommunity.h>
#include <mdoCommunity.h>
#include <mwsCollection.h>
#include <mdoCollection.h>
#include <mwsBitstream.h>
#include <mwsItem.h>
#include <mwsRestXMLParser.h>
#include <mdoItem.h>
#include "midasSynchronizer.h"
#include "midasProgressReporter.h"
#include "midasDatabaseProxy.h"

#define WORKING_DIR kwsys::SystemTools::GetCurrentWorkingDirectory
#define CHANGE_DIR kwsys::SystemTools::ChangeDirectory
#define MKDIR kwsys::SystemTools::MakeDirectory

#define NO_PARENT -1

midasSynchronizer::midasSynchronizer()
{
  this->Recursive = false;
  this->Operation = OPERATION_NONE;
  this->ResourceType = midasResourceType::NONE;
  this->ServerURL = "";
  this->Progress = NULL;
  this->Database = "";
  this->DatabaseProxy = NULL;
  this->Authenticator = new midasAuthenticator;
  this->WebAPI = new mws::WebAPI;
}

midasSynchronizer::~midasSynchronizer()
{
  if(this->DatabaseProxy)
    {
    delete this->DatabaseProxy;
    }
  delete this->Authenticator;
  delete this->WebAPI;
}

midasAuthenticator* midasSynchronizer::GetAuthenticator()
{
  return this->Authenticator;
}

void midasSynchronizer::SetDatabase(std::string path)
{
  this->Database = path;
  if(this->DatabaseProxy)
    {
    delete this->DatabaseProxy;
    }
  this->DatabaseProxy = new midasDatabaseProxy(path);
  this->Authenticator->SetDatabase(path);
}

void midasSynchronizer::SetProgressReporter(midasProgressReporter* progress)
{
  this->Progress = progress;
}

midasProgressReporter* midasSynchronizer::GetProgressReporter()
{
  return this->Progress;
}

void midasSynchronizer::DeleteProgressReporter()
{
  if(this->Progress)
    {
    delete this->Progress;
    }
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
  return this->ServerURL;
}

void midasSynchronizer::SetServerURL(std::string url)
{
  this->WebAPI->SetServerUrl(url.c_str());
  this->Authenticator->SetServerURL(url.c_str());
  this->ServerURL = url;
}

//-------------------------------------------------------------------
int midasSynchronizer::Perform()
{
  if(!this->Authenticator->Login(this->WebAPI))
    {
    std::cerr << "Login failed." << std::endl;
    return -1;
    }

  switch(this->Operation)
    {
    case OPERATION_ADD:
      return this->Add();
    case OPERATION_CLEAN:
      return this->Clean();
    case OPERATION_CLONE:
      return this->Clone();
    case OPERATION_PULL:
      return this->Pull();
    case OPERATION_PUSH:
      return this->Push();
    default:
      return -1;
    }
}

//-------------------------------------------------------------------
int midasSynchronizer::Add()
{
  std::string path = 
    kwsys::SystemTools::FileIsFullPath(this->ResourceHandle.c_str()) ?
    this->ResourceHandle : WORKING_DIR() + "/" + this->ResourceHandle;

  if(!kwsys::SystemTools::FileExists(path.c_str()))
    {
    std::cerr << "Error: \"" << this->ResourceHandle << "\" does not refer"
      " to a valid absolute or relative path." << std::endl;
    return -1;
    }
  if(kwsys::SystemTools::FileIsDirectory(path.c_str()) &&
     this->ResourceType == midasResourceType::BITSTREAM)
    {
    std::cerr << "Error: \"" << path << "\" is a directory. A bitstream "
      "refers to a file, not a directory." << std::endl;
    return -1;
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
    std::cerr << "Error: \"" << path << "\" is already in the database."
      << std::endl;
    this->DatabaseProxy->Close();
    return -1;
    }
  std::string parentUuid = this->DatabaseProxy->GetUuidFromPath(parentDir);

  int id = this->DatabaseProxy->AddResource(this->ResourceType, uuid, 
    path, name, parentUuid);
  this->DatabaseProxy->MarkDirtyResource(uuid, midasDirtyAction::ADDED);
  
  //TODO propagate last modified stamp up the local tree. (perhaps in MarkDirtyResource())
  
  this->DatabaseProxy->Close();
  return 0;
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
  mws::Community remote;
  mdo::Community* community = new mdo::Community;
  remote.SetWebAPI(this->WebAPI);
  remote.SetObject(community);
  
  if(!remote.FetchTree())
    {
    std::cerr << "Unable to fetch resource tree via the Web API" << std::endl;
    return -1;
    }

  this->RecurseCommunities(NO_PARENT, community);
  delete community;

  return 0;
}

//-------------------------------------------------------------------
int DownloadProgress(void *clientp, double dltotal, double dlnow, 
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
  if(this->ResourceType == midasResourceType::NONE || this->ServerURL == "")
    {
    return -1;
    }
 
  std::string name;
  switch(this->ResourceType)
    {
    case midasResourceType::BITSTREAM:
      name = this->GetBitstreamName();
      return this->PullBitstream(NO_PARENT, name) ? 0 : -1;
    case midasResourceType::COLLECTION:
      return this->PullCollection(NO_PARENT) ? 0 : -1;
    case midasResourceType::COMMUNITY:
      return this->PullCommunity(NO_PARENT) ? 0 : -1;
    case midasResourceType::ITEM:
      return this->PullItem(NO_PARENT) ? 0 : -1;
    default:
      return -1;
    }
}

//-------------------------------------------------------------------
bool midasSynchronizer::PullBitstream(int parentId, std::string filename)
{
  if(filename == "")
    {
    return false;
    }
  this->DatabaseProxy->Open();
  std::string uuid = this->GetUUID(midasResourceType::BITSTREAM);
  std::string path = this->DatabaseProxy->GetResourceLocation(uuid);

  //TODO check md5 sum of file at location against server's checksum?
  if(path != "" && kwsys::SystemTools::FileExists(path.c_str(), true))
    {
    //we already have this bitstream, no need to download again
    this->DatabaseProxy->Close();
    return true;
    }

  std::stringstream fields;
  fields << "midas.bitstream.download?id=" << this->GetResourceHandle();
  
  if(this->Progress)
    {
    this->WebAPI->GetRestAPI()->SetProgressCallback(
      DownloadProgress, this->Progress);
    this->Progress->SetMessage(filename);
    this->Progress->ResetProgress();
    }
  this->WebAPI->DownloadFile(fields.str().c_str(), filename.c_str());

  this->DatabaseProxy->AddResource(midasResourceType::BITSTREAM,
    uuid, WORKING_DIR() + "/" + filename, filename, midasResourceType::ITEM,
    parentId);
  this->DatabaseProxy->Close();
  
  return true;
}

//-------------------------------------------------------------------
bool midasSynchronizer::PullCollection(int parentId)
{
  this->DatabaseProxy->Open();
  std::string uuid = this->GetUUID(midasResourceType::COLLECTION);

  mws::Collection remote;
  mdo::Collection* collection = new mdo::Collection;
  collection->SetId(atoi(this->GetResourceHandle().c_str()));
  remote.SetWebAPI(this->WebAPI);
  remote.SetObject(collection);

  if(!remote.Fetch())
    {
    std::cerr << "Unable to fetch the collection via the Web API" << std::endl;
    this->DatabaseProxy->Close();
    return false;
    }

  int id = this->DatabaseProxy->AddResource(midasResourceType::COLLECTION,
    uuid, WORKING_DIR() + "/" + collection->GetName(), collection->GetName(),
    midasResourceType::COMMUNITY, parentId);
  
  this->DatabaseProxy->Close();

  if(!kwsys::SystemTools::FileIsDirectory(collection->GetName().c_str()))
    {
    MKDIR(collection->GetName().c_str());
    }

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
  this->DatabaseProxy->Open();
  std::string uuid = this->GetUUID(midasResourceType::COMMUNITY);

  mws::Community remote;
  mdo::Community* community = new mdo::Community;
  community->SetId(atoi(this->ResourceHandle.c_str()));
  remote.SetWebAPI(this->WebAPI);
  remote.SetObject(community);

  if(!remote.FetchTree())
    {
    std::cerr << "Unable to fetch the community via the Web API" << std::endl;
    this->DatabaseProxy->Close();
    return false;
    }
  community = FindInTree(community, atoi(this->ResourceHandle.c_str()));
  if(!community)
    {
    std::cerr << "Error: Community " << this->ResourceHandle 
      << " does not exist." << std::endl;
    this->DatabaseProxy->Close();
    return false;
    }

  std::string topLevelDir = WORKING_DIR();

  if(!kwsys::SystemTools::FileIsDirectory(community->GetName().c_str()))
    {
    MKDIR(community->GetName().c_str());
    }

  int id = this->DatabaseProxy->AddResource(midasResourceType::COMMUNITY,
    uuid, WORKING_DIR() + "/" + community->GetName(), community->GetName(),
    midasResourceType::COMMUNITY, parentId);
  this->DatabaseProxy->Close();

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
std::string midasSynchronizer::GetUUID(int type)
{
  std::stringstream fields;
  fields << "midas.uuid.get?id=" << this->GetResourceHandle()
    << "&type=" << type;
  mws::WebAPI remote;
  remote.SetServerUrl(this->ServerURL.c_str());

  std::string uuid;
  remote.GetRestXMLParser()->AddTag("/rsp/uuid", uuid);
  remote.Execute(fields.str().c_str());
  return uuid;
}

//-------------------------------------------------------------------
std::string midasSynchronizer::GetBitstreamName()
{
  mws::Bitstream remote;
  mdo::Bitstream* bitstream = new mdo::Bitstream;
  bitstream->SetId(atoi(this->ResourceHandle.c_str()));
  remote.SetWebAPI(this->WebAPI);
  remote.SetObject(bitstream);
  
  if(!remote.Fetch())
    {
    std::cerr << "Unable to get bitstream via the web API." << std::endl;
    }

  if(bitstream->GetName() == "")
    {
    std::cerr << "Bitstream " << this->ResourceHandle << " does not exist."
      << std::endl;
    }
  return bitstream->GetName();
}

//-------------------------------------------------------------------
bool midasSynchronizer::PullItem(int parentId)
{
  this->DatabaseProxy->Open();
  std::string uuid = this->GetUUID(midasResourceType::ITEM);

  mws::Item remote;
  mdo::Item* item = new mdo::Item;
  item->SetId(atoi(this->GetResourceHandle().c_str()));
  remote.SetWebAPI(this->WebAPI);
  remote.SetObject(item);

  if(!remote.Fetch())
    {
    std::cerr << "Unable to fetch the item via the Web API" << std::endl;
    this->DatabaseProxy->Close();
    return false;
    }
  
  std::stringstream altTitle;
  altTitle << "item" << item->GetId();
  std::string title = item->GetTitle() == "" ? altTitle.str() :
    item->GetTitle();

  if(!kwsys::SystemTools::FileIsDirectory(title.c_str()))
    {
    MKDIR(title.c_str());
    }

  int id = this->DatabaseProxy->AddResource(midasResourceType::ITEM,
    uuid, WORKING_DIR() + "/" + title, item->GetTitle(),
    midasResourceType::COLLECTION, parentId);
  this->DatabaseProxy->Close();

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
      this->PullBitstream(id, (*i)->GetName());
      }
    CHANGE_DIR(temp.c_str());
    }

  delete item;
  return true;
}

//-------------------------------------------------------------------
int midasSynchronizer::Push()
{
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

  for(std::vector<std::string>::iterator i = uuids.begin(); i != uuids.end();
      ++i)
    {
    int id, type;
    this->DatabaseProxy->GetTypeAndIdForUuid((*i), type, id);

    switch(type)
      {
      case midasResourceType::BITSTREAM:
        success &= this->PushBitstream(id);
        break;
      case midasResourceType::COLLECTION:
        success &= this->PushCollection(id);
        break;
      case midasResourceType::COMMUNITY:
        success &= this->PushCommunity(id);
        break;
      case midasResourceType::ITEM:
        success &= this->PushItem(id);
        break;
      default:
        return -1;
      }

    if(this->WebAPI->GetErrorCode() == INVALID_POLICY
      && this->Authenticator->IsAnonymous())
      {
      std::cerr << "You are not logged in. Please specify a user profile."
        << std::endl;
      this->DatabaseProxy->Close();
      return -1;
      }
    }
  this->DatabaseProxy->Close();
  return success ? 0 : -1;
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
    this->WebAPI->GetRestXMLParser()->AddTag("/rsp/id", server_parentId);
    this->WebAPI->Execute(fields.str().c_str());
    this->WebAPI->GetRestXMLParser()->ClearTags();
    parentId = atoi(server_parentId.c_str());
    }
  return parentId;
}

//-------------------------------------------------------------------
bool midasSynchronizer::PushBitstream(int id)
{
  return true;
}

//-------------------------------------------------------------------
bool midasSynchronizer::PushCollection(int id)
{
  std::string uuid = this->DatabaseProxy->GetUuid(
    midasResourceType::COLLECTION, id);
  std::string name = this->DatabaseProxy->GetName(
    midasResourceType::COLLECTION, id);

  int parentId = this->GetServerParentId(midasResourceType::COMMUNITY,
    this->DatabaseProxy->GetParentId(midasResourceType::COLLECTION, id));
  
  std::stringstream fields;
  fields << "midas.collection.create?uuid=" << uuid << "&name=" <<
    midasUtils::EscapeForURL(name) << "&parentid=" << parentId;

  this->WebAPI->SetPostData("");
  bool success = this->WebAPI->Execute(fields.str().c_str());
  if(success)
    {
    // Clear dirty flag on the resource
    this->DatabaseProxy->ClearDirtyResource(uuid);
    std::cout << "Pushed collection " << name << std::endl;
    }
  else
    {
    std::cerr << "Failed to push collection " << name << ": " <<
    this->WebAPI->GetErrorMessage() << std::endl;
    }
  return success;
}

//-------------------------------------------------------------------
bool midasSynchronizer::PushCommunity(int id)
{
  std::string uuid = this->DatabaseProxy->GetUuid(
    midasResourceType::COMMUNITY, id);
  std::string name = this->DatabaseProxy->GetName(
    midasResourceType::COMMUNITY, id);

  int parentId = this->GetServerParentId(midasResourceType::COMMUNITY,
    this->DatabaseProxy->GetParentId(midasResourceType::COMMUNITY, id));
  while(this->DatabaseProxy->GetDatabase()->GetNextRow());

  // Create new community on server
  std::stringstream fields;
  fields << "midas.community.create?uuid=" << uuid << "&name=" << 
    midasUtils::EscapeForURL(name) << "&parentid=" << parentId;

  this->WebAPI->SetPostData("");
  bool success = this->WebAPI->Execute(fields.str().c_str());
  if(success)
    {
    // Clear dirty flag on the resource
    this->DatabaseProxy->ClearDirtyResource(uuid);
    std::cout << "Pushed community " << name << std::endl;
    }
  else
    {
    std::cerr << "Failed to push community " << name << ": " <<
    this->WebAPI->GetErrorMessage() << std::endl;
    }
  return success;
}

//-------------------------------------------------------------------
bool midasSynchronizer::PushItem(int id)
{
  std::string uuid = this->DatabaseProxy->GetUuid(
    midasResourceType::ITEM, id);
  std::string name = this->DatabaseProxy->GetName(
    midasResourceType::ITEM, id);

  int parentId = this->GetServerParentId(midasResourceType::COLLECTION,
    this->DatabaseProxy->GetParentId(midasResourceType::ITEM, id));
  
  std::stringstream fields;
  fields << "midas.item.create?uuid=" << uuid << "&name=" <<
    midasUtils::EscapeForURL(name) << "&parentid=" << parentId;

  this->WebAPI->SetPostData("");
  bool success = this->WebAPI->Execute(fields.str().c_str());
  if(success)
    {
    // Clear dirty flag on the resource
    this->DatabaseProxy->ClearDirtyResource(uuid);
    std::cout << "Pushed item " << name << std::endl;
    }
  else
    {
    std::cerr << "Failed to push item " << name << ": " <<
    this->WebAPI->GetErrorMessage() << std::endl;
    }
  return success;
}
