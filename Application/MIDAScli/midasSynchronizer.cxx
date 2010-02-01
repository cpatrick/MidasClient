
#include <mdsSQLiteDatabase.h>
#include <mwsWebAPI.h>
#include <mwsCommunity.h>
#include <mdoCommunity.h>
#include <mwsCollection.h>
#include <mwsBitstream.h>
#include <mwsItem.h>
#include "midasSynchronizer.h"
#include "midasCLI.h"

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
  std::cout << "PROGRESS: " << dlnow << " / " << dltotal << std::endl;
  return 0;
}

//-------------------------------------------------------------------
int midasSynchronizer::Pull()
{
  if(this->PullType == TYPE_NONE || this->ServerURL == "")
    {
    return -1;
    }
 
  switch(this->PullType)
    {
    case TYPE_BITSTREAM:
      return this->PullBitstream();
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
int midasSynchronizer::PullBitstream()
{
  mws::WebAPI remote;
  mds::SQLiteDatabase local;
//TODO figure out (via uuid lookup table) what type of resource we are getting so we know which web service API to call.
//TODO implement web service to lookup resource_uuid information based on uuid.

  std::stringstream fields;
  fields << "midas.bitstream.download?id=" << this->GetResourceHandle();
  //TODO call remote.login() based on config options (profiles?)
  remote.SetServerUrl(this->ServerURL.c_str());
  std::string* progressData = new std::string;
  remote.GetRestAPI()->SetProgressCallback(DownloadProgress, progressData);
  remote.DownloadFile(fields.str().c_str(), "C:\\testFile.jpg");
  delete progressData;
  
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
  std::stringstream fields;
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

  std::string topLevelDir = kwsys::SystemTools::GetCurrentWorkingDirectory();
  //reverse from root to selected community, writing directories
  for(std::vector<mdo::Community*>::reverse_iterator i = path.rbegin();
    i != path.rend(); ++i)
    {
    const char* name = (*i)->GetName().c_str();
    if(!kwsys::SystemTools::FileIsDirectory(name))
      {
      kwsys::SystemTools::MakeDirectory(name);
      }
    kwsys::SystemTools::ChangeDirectory(name);
    }

  if(!kwsys::SystemTools::FileIsDirectory(community->GetName().c_str()))
      {
      kwsys::SystemTools::MakeDirectory(community->GetName().c_str());
      }

  //revert working dir to top level
  kwsys::SystemTools::ChangeDirectory(topLevelDir.c_str());
  delete community;

  //TODO check against local database
  mds::SQLiteDatabase local;
  local.Open(this->CLI->GetDatabaseLocation().c_str());
  //db.ExecuteQuery(query.str().c_str());
  local.Close();
  return 0;
}

//-------------------------------------------------------------------
int midasSynchronizer::PullItem()
{
  return 0;
}

//-------------------------------------------------------------------
int midasSynchronizer::Push()
{
  return 0;
}
