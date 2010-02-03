/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <mdsSQLiteDatabase.h>
#include <mwsWebAPI.h>
#include <mwsCommunity.h>
#include <mdoCommunity.h>
#include <mwsCollection.h>
#include <mdoCollection.h>
#include <mwsBitstream.h>
#include <mwsItem.h>
#include <mdoItem.h>
#include "midasSynchronizer.h"
#include "midasProgressReporter.h"
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
}

midasSynchronizer::~midasSynchronizer()
{
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
  mws::WebAPI remote;
  mds::SQLiteDatabase local;

  std::stringstream fields;
  fields << "midas.bitstream.download?id=" << this->GetResourceHandle();
  //TODO call remote.login() based on config options (profiles?)
  remote.SetServerUrl(this->ServerURL.c_str());
  midasProgressReporter* progress = new midasProgressReporter(30);
  std::cout << std::setw(32) << filename << "  ";
  remote.GetRestAPI()->SetProgressCallback(DownloadProgress, progress);
  remote.DownloadFile(fields.str().c_str(), filename.c_str());

  std::cout << " Done" << std::endl;
  delete progress;
  
  //std::stringstream query;
  //query << "

  local.Open(this->CLI->GetDatabaseLocation().c_str());
  //db.ExecuteQuery(query.str().c_str());
  local.Close();
  return 0;
}

//-------------------------------------------------------------------
int midasSynchronizer::PullCollection()
{
  mws::Collection remote;
  mdo::Collection* collection = new mdo::Collection;
  collection->SetId(atoi(this->GetResourceHandle().c_str()));
  remote.SetWebAPI(&this->WebAPI);
  remote.SetObject(collection);

  if(!remote.Fetch())
    {
    std::cerr << "Unable to fetch the collection via the Web API" << std::endl;
    return -1;
    }

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
  mws::Community remote;
  mdo::Community* community = new mdo::Community;
  community->SetId(atoi(this->ResourceHandle.c_str()));
  remote.SetWebAPI(&this->WebAPI);
  remote.SetObject(community);
  
  if(!remote.FetchTree())
    {
    std::cerr << "Unable to fetch the community via the Web API" << std::endl;
    return -1;
    }
  community = FindInTree(community, atoi(this->ResourceHandle.c_str()));
  if(!community)
    {
    std::cerr << "Error: Community " << this->ResourceHandle 
      << " does not exist." << std::endl;
    return -1;
    }

  // Go up the tree to the root, building the path
  std::vector<mdo::Community*> path;
  mdo::Community* current = community;
  while(current->GetParent() != NULL)
    {
    current = current->GetParent();
    path.push_back(current);
    }

  std::string topLevelDir = WORKING_DIR();
  //reverse from root to selected community, writing directories
  for(std::vector<mdo::Community*>::reverse_iterator i = path.rbegin();
      i != path.rend(); ++i)
    {
    const char* name = (*i)->GetName().c_str();
    if(!kwsys::SystemTools::FileIsDirectory(name))
      {
      MKDIR(name);
      }
    CHANGE_DIR(name);
    }

  if(!kwsys::SystemTools::FileIsDirectory(community->GetName().c_str()))
    {
    MKDIR(community->GetName().c_str());
    }
  
  if(this->Recursive)
    {
    CHANGE_DIR(community->GetName().c_str());
    //Pull the everything under this community.
    RecurseCommunities(community);
    }

  //revert working dir to top level
  CHANGE_DIR(topLevelDir.c_str());
  delete community;

  //TODO check against local database
  mds::SQLiteDatabase local;
  local.Open(this->CLI->GetDatabaseLocation().c_str());
  //db.ExecuteQuery(query.str().c_str());
  local.Close();
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
    std::string temp = WORKING_DIR();
    MKDIR((*i)->GetName().c_str());
    CHANGE_DIR((*i)->GetName().c_str());
    this->RecurseCommunities(*i);
    CHANGE_DIR(temp.c_str());
    }
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
  mws::Item remote;
  mdo::Item* item = new mdo::Item;
  item->SetId(atoi(this->GetResourceHandle().c_str()));
  remote.SetWebAPI(&this->WebAPI);
  remote.SetObject(item);

  if(!remote.Fetch())
    {
    std::cerr << "Unable to fetch the item via the Web API" << std::endl;
    return -1;
    }
  
  if(!kwsys::SystemTools::FileIsDirectory(item->GetTitle().c_str()))
    {
    MKDIR(item->GetTitle().c_str());
    }

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
