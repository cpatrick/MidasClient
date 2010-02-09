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
#include "midasCLI.h"

#define WORKING_DIR kwsys::SystemTools::GetCurrentWorkingDirectory
#define CHANGE_DIR kwsys::SystemTools::ChangeDirectory
#define MKDIR kwsys::SystemTools::MakeDirectory

midasSynchronizer::midasSynchronizer(midasCLI* cli)
{
  this->CLI = cli;
  this->Recursive = false;
  this->Operation = OPERATION_NONE;
  this->PullType = TYPE_NONE;
  this->ServerURL = "";
  this->Progress = NULL;
  this->Database = "";
  this->DatabaseProxy = NULL;
}

midasSynchronizer::~midasSynchronizer()
{
  if(this->DatabaseProxy)
    {
    delete this->DatabaseProxy;
    }
}

void midasSynchronizer::SetDatabase(std::string path)
{
  this->Database = path;
  if(this->DatabaseProxy)
    {
    delete this->DatabaseProxy;
    }
  this->DatabaseProxy = new midasDatabaseProxy(path);
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

void midasSynchronizer::SetPullType(midasSynchronizer::ResourceType type)
{
  this->PullType = type;
}

midasSynchronizer::ResourceType midasSynchronizer::GetPullType()
{
  return this->PullType;
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
  this->WebAPI.SetServerUrl(url.c_str());
  this->ServerURL = url;
}

//-------------------------------------------------------------------
int midasSynchronizer::Perform()
{
  switch(this->Operation)
    {
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
int midasSynchronizer::Clone()
{
  mws::Community remote;
  mdo::Community* community = new mdo::Community;
  remote.SetWebAPI(&this->WebAPI);
  remote.SetObject(community);
  
  if(!remote.FetchTree())
    {
    std::cerr << "Unable to fetch resource tree via the Web API" << std::endl;
    return -1;
    }

  this->RecurseCommunities(community);
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
  if(this->PullType == TYPE_NONE || this->ServerURL == "")
    {
    return -1;
    }
 
  std::string name;
  switch(this->PullType)
    {
    case TYPE_BITSTREAM:
      name = this->GetBitstreamName();
      return this->PullBitstream(name);
    case TYPE_COLLECTION:
      return this->PullCollection();
    case TYPE_COMMUNITY:
      return this->PullCommunity();
    case TYPE_ITEM:
      return this->PullItem();
    default:
      return -1;
    }
}

//-------------------------------------------------------------------
int midasSynchronizer::PullBitstream(std::string filename)
{
  if(filename == "")
    {
    return -1;
    }
  this->DatabaseProxy->Open();
  std::string uuid = this->GetUUID(MIDAS_RESOURCE_BITSTREAM);
  std::string path = this->DatabaseProxy->GetResourceLocation(uuid);

  //TODO check md5 sum of file at location against server's checksum?
  if(path != "" && kwsys::SystemTools::FileExists(path.c_str(), true))
    {
    //we already have this bitstream, no need to download again
    this->DatabaseProxy->Close();
    return 0;
    }

  mws::WebAPI remote;

  std::stringstream fields;
  fields << "midas.bitstream.download?id=" << this->GetResourceHandle();
  //TODO call remote.login() based on config options (profiles?)
  remote.SetServerUrl(this->ServerURL.c_str());
  
  if(this->Progress)
    {
    remote.GetRestAPI()->SetProgressCallback(DownloadProgress, this->Progress);
    this->Progress->SetMessage(filename);
    this->Progress->ResetProgress();
    }
  remote.DownloadFile(fields.str().c_str(), filename.c_str());
  
  int id = this->DatabaseProxy->InsertBitstream(
    WORKING_DIR() + "/" + filename, filename);
  this->DatabaseProxy->InsertResourceRecord(
    MIDAS_RESOURCE_BITSTREAM, id, WORKING_DIR() + "/" + filename, uuid);
  this->DatabaseProxy->Close();
  
  return 0;
}

//-------------------------------------------------------------------
int midasSynchronizer::PullCollection()
{
  this->DatabaseProxy->Open();
  std::string uuid = this->GetUUID(MIDAS_RESOURCE_COLLECTION);

  mws::Collection remote;
  mdo::Collection* collection = new mdo::Collection;
  collection->SetId(atoi(this->GetResourceHandle().c_str()));
  remote.SetWebAPI(&this->WebAPI);
  remote.SetObject(collection);

  if(!remote.Fetch())
    {
    std::cerr << "Unable to fetch the collection via the Web API" << std::endl;
    this->DatabaseProxy->Close();
    return -1;
    }

  if(!this->DatabaseProxy->ResourceExists(uuid))
    {
    int id = this->DatabaseProxy->InsertCollection(collection->GetName());
    this->DatabaseProxy->InsertResourceRecord(MIDAS_RESOURCE_COLLECTION,
      id, WORKING_DIR() + "/" + collection->GetName(), uuid);
    }
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
      this->PullItem();
      }
    CHANGE_DIR(temp.c_str());
    }

  delete collection;
  return 0;
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
int midasSynchronizer::PullCommunity()
{
  this->DatabaseProxy->Open();
  std::string uuid = this->GetUUID(MIDAS_RESOURCE_COMMUNITY);

  mws::Community remote;
  mdo::Community* community = new mdo::Community;
  community->SetId(atoi(this->ResourceHandle.c_str()));
  remote.SetWebAPI(&this->WebAPI);
  remote.SetObject(community);
  
  if(!remote.FetchTree())
    {
    std::cerr << "Unable to fetch the community via the Web API" << std::endl;
    this->DatabaseProxy->Close();
    return -1;
    }
  community = FindInTree(community, atoi(this->ResourceHandle.c_str()));
  if(!community)
    {
    std::cerr << "Error: Community " << this->ResourceHandle 
      << " does not exist." << std::endl;
    this->DatabaseProxy->Close();
    return -1;
    }

  std::string topLevelDir = WORKING_DIR();

  if(!kwsys::SystemTools::FileIsDirectory(community->GetName().c_str()))
    {
    MKDIR(community->GetName().c_str());
    }

  if(!this->DatabaseProxy->ResourceExists(uuid))
    {
    int id = this->DatabaseProxy->InsertCommunity(community->GetName());
    this->DatabaseProxy->InsertResourceRecord(MIDAS_RESOURCE_COMMUNITY, 
      id, WORKING_DIR() + "/" + community->GetName(), uuid);
    }
  this->DatabaseProxy->Close();
  
  if(this->Recursive)
    {
    CHANGE_DIR(community->GetName().c_str());
    // Pull everything under this community.
    this->RecurseCommunities(community);
    }

  // Revert working dir to top level
  CHANGE_DIR(topLevelDir.c_str());
  delete community;
  return 0;
}

/**
 * Function to recursively pull all collections
 * underneath the given community, including in subcommunities.
 */
void midasSynchronizer::RecurseCommunities(mdo::Community* community)
{
  for(std::vector<mdo::Collection*>::const_iterator i = 
      community->GetCollections().begin();
      i != community->GetCollections().end(); ++i)
    {
    std::stringstream s;
    s << (*i)->GetId();
    this->SetResourceHandle(s.str());
    this->PullCollection();
    }
  for(std::vector<mdo::Community*>::const_iterator i =
      community->GetCommunities().begin();
      i != community->GetCommunities().end(); ++i)
    {
    std::stringstream s;
    s << (*i)->GetId();
    this->SetResourceHandle(s.str());
    this->PullCommunity();
    }
}

//-------------------------------------------------------------------
std::string midasSynchronizer::GetUUID(int type)
{
  mws::WebAPI remote;
  std::stringstream fields;
  fields << "midas.uuid.get?id=" << this->GetResourceHandle()
    << "&type=" << type;
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
  remote.SetWebAPI(&this->WebAPI);
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
int midasSynchronizer::PullItem()
{
  this->DatabaseProxy->Open();
  std::string uuid = this->GetUUID(MIDAS_RESOURCE_ITEM);

  mws::Item remote;
  mdo::Item* item = new mdo::Item;
  item->SetId(atoi(this->GetResourceHandle().c_str()));
  remote.SetWebAPI(&this->WebAPI);
  remote.SetObject(item);

  if(!remote.Fetch())
    {
    std::cerr << "Unable to fetch the item via the Web API" << std::endl;
    this->DatabaseProxy->Close();
    return -1;
    }
  
  if(!kwsys::SystemTools::FileIsDirectory(item->GetTitle().c_str()))
    {
    MKDIR(item->GetTitle().c_str());
    }

  if(!this->DatabaseProxy->ResourceExists(uuid))
    {
    int id = this->DatabaseProxy->InsertItem(item->GetTitle());
    this->DatabaseProxy->InsertResourceRecord(MIDAS_RESOURCE_ITEM, 
      id, WORKING_DIR() + "/" + item->GetTitle(), uuid);
    }
  this->DatabaseProxy->Close();

  if(this->Recursive)
    {
    std::string temp = WORKING_DIR();
    CHANGE_DIR(item->GetTitle().c_str());
    for(std::vector<mdo::Bitstream*>::const_iterator i = 
        item->GetBitstreams().begin();
        i != item->GetBitstreams().end(); ++i)
      {
      std::stringstream s;
      s << (*i)->GetId();
      this->SetResourceHandle(s.str());
      this->PullBitstream((*i)->GetName());
      }
    CHANGE_DIR(temp.c_str());
    }

  delete item;
  return 0;
}

//-------------------------------------------------------------------
int midasSynchronizer::Push()
{
  return 0;
}
