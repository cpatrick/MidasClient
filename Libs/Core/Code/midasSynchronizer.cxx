/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "midasSynchronizer.h"

#include <QMutexLocker>

#include "mdoVersion.h"
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
#include "m3doCommunity.h"
#include "m3doFolder.h"
#include "m3dsFolder.h"
#include "m3wsFolder.h"
#include "m3doItem.h"
#include "m3dsItem.h"
#include "m3wsItem.h"
#include "m3doBitstream.h"
#include "m3dsBitstream.h"
#include "m3wsBitstream.h"
#include "mdsDatabaseAPI.h"
#include "midasProgressReporter.h"
#include "midasStdOutLog.h"
#include "mwsRestResponseParser.h"
#include "midasAuthenticator.h"
#include "midasStatus.h"
#include "midasAgreementHandler.h"
#include "mdsResourceUpdateHandler.h"
#include "mdsPartialDownload.h"
#include "mdsPartialUpload.h"
#include "midasFileOverwriteHandler.h"

#include <QDir>
#include <QDateTime>

#define NO_PARENT -1

midasSynchronizer::midasSynchronizer()
{
  this->Recursive = false;
  this->ShouldCancel = false;
  this->PathMode = false;
  this->Operation = OPERATION_NONE;
  this->ResourceType = midasResourceType::NONE;
  this->ResourceType3 = midas3ResourceType::NONE;
  this->ServerHandle = "";
  this->ClientHandle = "";
  this->Uuid = "";
  this->Progress = NULL;
  this->Log = new midasStdOutLog();
  this->AgreementHandler = NULL;
  this->OverwriteHandler = NULL;
  this->Object = NULL;
  this->CurrentBitstreams = 0;
  this->TotalBitstreams = 0;
  this->Authenticator = new midasAuthenticator;
  this->Authenticator->SetLog(this->Log);
  this->Mutex = new QMutex;
}

midasSynchronizer::~midasSynchronizer()
{
  delete this->Authenticator;
}

midasFileOverwriteHandler* midasSynchronizer::GetOverwriteHandler()
{
  return this->OverwriteHandler;
}

void midasSynchronizer::SetOverwriteHandler(midasFileOverwriteHandler* handler)
{
  this->OverwriteHandler = handler;
}

midasAuthenticator* midasSynchronizer::GetAuthenticator()
{
  return this->Authenticator;
}

void midasSynchronizer::SetAuthenticator(midasAuthenticator* auth,
                                         bool deleteOld)
{
  if(deleteOld == true)
    {
    delete this->Authenticator;
    }
  this->Authenticator = auth;
}

void midasSynchronizer::SetLog(midasLog* log)
{
  delete this->Log;
  this->Log = log;
  this->Authenticator->SetLog(log);
}

void midasSynchronizer::SetAgreementHandler(midasAgreementHandler* handler)
{
  this->AgreementHandler = handler;
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

void midasSynchronizer::SetResourceType3(int type)
{
  this->ResourceType3 = type;
}

int midasSynchronizer::GetResourceType3()
{
  return this->ResourceType3;
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

void midasSynchronizer::SetObject(void* object)
{
  this->Object = object;
}

void* midasSynchronizer::GetObject()
{
  return this->Object;
}

//-------------------------------------------------------------------
int midasSynchronizer::Perform()
{
  QMutexLocker lock(this->Mutex);
  int rc = 0;
  this->ShouldCancel = false;
  if(!this->Authenticator->Login())
    {
    std::stringstream text;
    text << "Login failed." << std::endl;
    this->Log->Error(text.str());
    this->Reset();
    return MIDAS_LOGIN_FAILED;
    }

  const QString temp = QDir::current().path();
  if(this->Progress)
    {
    this->Progress->ResetOverall();
    this->Progress->SetUnit("B");
    }
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
    case OPERATION_RESUME_DOWNLOAD:
      rc = this->ResumeDownload() ? 0 : -1;
      break;
    case OPERATION_RESUME_UPLOAD:
      rc = this->ResumeUpload() ? 0 : -1;
      break;
    default:
      rc = MIDAS_BAD_OP;
      break;
    }
  QDir::setCurrent(temp);
  if(this->ShouldCancel)
    {
    rc = MIDAS_CANCELED;
    }
  this->Reset();
  return rc;
}

//-------------------------------------------------------------------
int midasSynchronizer::Add(mdo::Bitstream* result)
{
  if(DB_IS_MIDAS3)
    {
    return this->Add3();
    }

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
  QFileInfo fileInfo(path.c_str());

  if(path == "")
    {
    std::stringstream text;
    text << "Error: \"" << this->ClientHandle << "\" does "
      "not refer to a valid absolute or relative path." << std::endl;
    this->Log->Error(text.str());
    return MIDAS_INVALID_PATH;
    }
  if(fileInfo.isDir() &&
     this->ResourceType == midasResourceType::BITSTREAM)
    {
    std::stringstream text;
    text << "Error: \"" << path << "\" is a directory. A "
      "bitstream refers to a file, not a directory." << std::endl;
    this->Log->Error(text.str());
    return MIDAS_BAD_FILE_TYPE;
    }
  if(fileInfo.isFile() &&
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
    fileInfo.size() == 0)
    {
    std::stringstream text;
    text << "Error: \"" << path << "\" is 0 bytes. You may "
      "not add an empty bitstream." << std::endl;
    this->Log->Error(text.str());
    return MIDAS_EMPTY_FILE;
    }

  std::string uuid = midasUtils::GenerateUUID();

  std::string name = fileInfo.fileName().toStdString();
  std::string parentDir = fileInfo.dir().path().toStdString();

  mds::DatabaseAPI db;
  std::string parentUuid = this->ServerHandle == "" ?
    db.GetUuidFromPath(parentDir) : this->Uuid;
  parentDir = db.GetRecordByUuid(parentUuid).Path;

  if(this->ServerHandle != "")
    {
    path = parentDir + "/" + name;

    if(this->ResourceType == midasResourceType::BITSTREAM)
      {
      QFileInfo clientHandleInfo(this->ClientHandle.c_str());
      QString copyTo = QString(parentDir.c_str()) + "/" + clientHandleInfo.fileName();
      if(!QFile::copy(this->ClientHandle.c_str(), copyTo))
        {
        std::stringstream text;
        text << "Error: failed to copy file " << this->ClientHandle <<
          " into item directory " << parentDir << std::endl;
        this->Log->Error(text.str());
        return MIDAS_FAILURE;
        }
      
      this->ClientHandle = parentDir + "/" +
        clientHandleInfo.fileName().toStdString();
      }
    else
      {
      fileInfo.dir().mkdir(name.c_str());
      }
    }

  if(db.GetUuidFromPath(path) != "")
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

  int id = db.AddResource(this->ResourceType, uuid, path, name, parentUuid,
                           atoi(this->ServerHandle.c_str()));

  if(id <= 0)
    {
    std::stringstream text;
    text << "Add resource failed: " << path << std::endl;
    this->Log->Error(text.str());
    return MIDAS_FAILURE;
    }

  // Size of bitstream needs to be saved automatically
  if(this->ResourceType == midasResourceType::BITSTREAM)
    {
    std::stringstream size;
    size << fileInfo.size();
    mdo::Bitstream bitstream;
    bitstream.SetId(id);
    bitstream.SetName(name.c_str());
    bitstream.SetSize(size.str());
    bitstream.SetLastModified(fileInfo.lastModified().toTime_t());

    mds::Bitstream mdsBitstream;
    mdsBitstream.SetObject(&bitstream);

    if(!mdsBitstream.Commit())
      {
      std::stringstream text;
      text << "Commit failed for bitstream " << name << std::endl;
      return MIDAS_FAILURE;
      }

    if(result)
      {
      result->SetId(id);
      result->SetUuid(uuid.c_str());
      }

    }
  db.MarkDirtyResource(uuid, midasDirtyAction::ADDED);

  return MIDAS_OK;
}

//-------------------------------------------------------------------
int midasSynchronizer::Add3()
{
  std::string path;

  QFileInfo fileInfo(this->ClientHandle.c_str());
  if(fileInfo.exists())
    {
    path = fileInfo.isAbsolute() ? this->ClientHandle :
      QDir::currentPath().toStdString() + "/" + this->ClientHandle;
    }
  else
    {
    std::stringstream text;
    text << "Invalid path: " << this->ClientHandle << std::endl;
    this->Log->Error(text.str());
    return MIDAS_NO_URL;
    }

  std::string uuid = midasUtils::GenerateUUID();
  std::string name = fileInfo.fileName().toStdString();
  std::string parentDir = fileInfo.dir().path().toStdString();

  if(this->ResourceType3 == midas3ResourceType::COMMUNITY)
    {
    m3do::Community comm;
    comm.SetUuid(uuid.c_str());
    comm.SetName(name.c_str());

    m3ds::Folder mdsFolder;
    mdsFolder.SetObject(&comm);
    if(!mdsFolder.Create())
      {
      this->Log->Error("Failed to add the communtiy to the database");
      return MIDAS_FAILURE;
      }
    }
  else if(this->ResourceType3 == midas3ResourceType::FOLDER)
    {
    m3do::Folder folder;
    folder.SetUuid(uuid.c_str());
    folder.SetName(name.c_str());
    folder.SetParentFolder(reinterpret_cast<m3do::Folder*>(this->Object));

    m3ds::Folder mdsFolder;
    mdsFolder.SetObject(&folder);
    if(!mdsFolder.Create())
      {
      this->Log->Error("Failed to add the folder to the database");
      return MIDAS_FAILURE;
      }
    }
  else if(this->ResourceType3 == midas3ResourceType::ITEM)
    {
    m3do::Item item;
    item.SetUuid(uuid.c_str());
    item.SetName(name.c_str());
    item.SetParentFolder(reinterpret_cast<m3do::Folder*>(this->Object));

    m3ds::Item mdsItem;
    mdsItem.SetObject(&item);
    if(!mdsItem.Create())
      {
      this->Log->Error("Failed to add the item to the database");
      return MIDAS_FAILURE;
      }
    }
  /*else if(this->ResourceType3 == midas3ResourceType::BITSTREAM)
    {
    std::stringstream size;
    size << fileInfo.size();
    m3do::Bitstream bitstream;
    bitstream.SetChecksum(midasUtils::ComputeFileChecksum(path));
    bitstream.SetPath(path);
    bitstream.SetName(name.c_str());
    bitstream.SetSize(size.str());
    bitstream.SetParentItem(reinterpret_cast<m3do::Item*>(this->Object));
    }*/
  else
    {
    this->Log->Error("synchronizer::Add3(): Invalid resource type");
    return MIDAS_FAILURE;
    }
  return MIDAS_OK;
}

//-------------------------------------------------------------------
int midasSynchronizer::Clean()
{
  mds::DatabaseAPI db;
  db.Clean();
  return MIDAS_OK;
}

//-------------------------------------------------------------------
int midasSynchronizer::Clone()
{
  this->ChangeToRootDir();

  if(std::string(mws::WebAPI::Instance()->GetServerUrl()) == "")
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
  remote.SetAuthenticator(this->Authenticator);
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
int midasSynchronizer::Pull()
{
  this->ChangeToRootDir();

  if(SERVER_IS_MIDAS3)
    {
    return this->Pull3();
    }

  if(this->ResourceType == midasResourceType::NONE && !this->IsPathMode())
    {
    std::stringstream text;
    text << "You must specify a resource type." << std::endl;
    Log->Error(text.str());
    return MIDAS_NO_RTYPE;
    }
  if(std::string(mws::WebAPI::Instance()->GetServerUrl()) == "")
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
  mdo::Bitstream bitstream;
  bitstream.SetId(atoi(this->ServerHandle.c_str()));
  remote.SetAuthenticator(this->Authenticator);
  remote.SetObject(&bitstream);
  
  this->Progress->SetIndeterminate();
  this->Log->Status("Fetching bitstream information...");
  if(!remote.Fetch())
    {
    std::stringstream text;
    text << "Unable to get bitstream via the web API." << std::endl;
    Log->Error(text.str());
    return false;
    }

  if(bitstream.HasAgreement() && this->AgreementHandler)
    {
    if(!this->AgreementHandler->HandleAgreement(this))
      {
      std::stringstream text;
      text << "You have not agreed to the license. Canceling pull."
        << std::endl;
      this->Log->Error(text.str());
      return false;
      }
    }

  // Pull any parents we need
  if(parentId == NO_PARENT)
    {
    bool recurse = this->Recursive;
    this->Recursive = false;
    std::string handle = this->ServerHandle;
    this->ServerHandle = bitstream.GetParentStr();
    this->ResourceType = midasResourceType::ITEM;

    this->PullItem(NO_PARENT);
    QDir::setCurrent(this->LastDir.c_str());

    this->ResourceType = midasResourceType::BITSTREAM;
    this->ServerHandle = handle;
    this->Recursive = recurse;
    parentId = this->LastId;
    }

  if(bitstream.GetName() == "")
    {
    std::stringstream text;
    text << "Bitstream " << this->ServerHandle <<
      " does not exist." << std::endl;
    Log->Error(text.str());
    return false;
    }

  mds::DatabaseAPI db;
  midasResourceRecord record = db.GetRecordByUuid(bitstream.GetUuid());

  this->CurrentBitstreams++;

  if(this->Progress)
    {
    mws::WebAPI::Instance()->SetProgressReporter(this->Progress);
    this->Progress->SetMessage(bitstream.GetName());
    this->Progress->UpdateOverallCount(this->CurrentBitstreams);
    this->Progress->UpdateProgress(0, 0);
    this->Progress->ResetProgress();
    }

  QFileInfo fileInfo(record.Path.c_str());
  if(record.Path != "" && fileInfo.isFile())
    {
    //we already have this bitstream, no need to download again
    if(this->Progress)
      {
      mds::Bitstream mdsBitstream;
      mdo::Bitstream bitstream2;
      bitstream2.SetId(record.Id);
      mdsBitstream.SetObject(&bitstream2);
      mdsBitstream.Fetch();
      this->Progress->UpdateTotalProgress(
        midasUtils::StringToDouble(bitstream2.GetSize()));
      }
    return true;
    }
  std::stringstream status;
  status << "Downloading bitstream " << bitstream.GetName();
  this->Log->Status(status.str());

  bool download = true;
  QFileInfo bitstreamInfo(bitstream.GetName().c_str());
  bitstreamInfo.setCaching(false);
  if(bitstreamInfo.isFile())
    {
    if(this->OverwriteHandler)
      {
      download = this->OverwriteHandler->HandleConflict(
        QDir::currentPath().toStdString() + "/" + bitstream.GetName())
        == midasFileOverwriteHandler::Overwrite;
      }
    else
      {
      download = false; //if no handler was set we use existing file
      }
    }

  // Log the partial download so we can resume later if interrupted
  QString path = QDir::currentPath() + "/" + bitstream.GetName().c_str();
  mds::PartialDownload partial;
  partial.SetPath(path.toStdString());
  partial.SetUuid(bitstream.GetUuid());
  partial.SetParentItem(parentId);
  partial.Commit();

  if(download && !remote.Download())
    {
    std::stringstream text;
    if(this->ShouldCancel)
      {
      text << "Download canceled by user.";
      this->Log->Message(text.str());
      }
    else
      {
      text << "Connection error during download.";
      this->Log->Error(text.str());
      }
    this->Log->Status(text.str());
    return false;
    }

  partial.Remove(); //Download succeeded, remove incomplete download record

  int id = db.AddResource(midasResourceType::BITSTREAM, bitstream.GetUuid(),
    QDir::currentPath().toStdString() + "/" + bitstream.GetName(),
    bitstream.GetName(), midasResourceType::ITEM, parentId, 0);

  if(id <= 0)
    {
    std::stringstream text;
    text << "Failed to add resource record for bistream "
      << bitstream.GetName() << " to the database." << std::endl;
    this->Log->Error(text.str());
    return false;
    }
  bitstream.SetId(id);
  bitstream.SetLastModified(bitstreamInfo.lastModified().toTime_t());

  mds::Bitstream mdsBitstream;
  mdsBitstream.SetObject(&bitstream);
  if(!mdsBitstream.Commit())
    {
    std::stringstream text;
    text << "Failed to add bitstream " << bitstream.GetName() <<
      " to the database." << std::endl;
    this->Log->Error(text.str());
    return false;
    }
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
  remote.SetAuthenticator(this->Authenticator);
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

  if(collection->HasAgreement() && this->AgreementHandler)
    {
    if(!this->AgreementHandler->HandleAgreement(this))
      {
      std::stringstream text;
      text << "You have not agreed to the license. Canceling pull."
        << std::endl;
      this->Log->Error(text.str());
      return false;
      }
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
    this->ServerHandle = collection->GetParentStr();
    this->ResourceType = midasResourceType::COMMUNITY;

    this->PullCommunity(NO_PARENT);
    QDir::setCurrent(this->LastDir.c_str());

    this->ResourceType = midasResourceType::COLLECTION;
    this->ServerHandle = handle;
    this->Recursive = recurse;
    parentId = this->LastId;
    }

  mds::DatabaseAPI db;
  int id = db.AddResource(midasResourceType::COLLECTION,
    collection->GetUuid(), QDir::currentPath().toStdString() + "/" +
    collection->GetName(), collection->GetName(),
    midasResourceType::COMMUNITY, parentId, 0);

  if(id <= 0)
    {
    std::stringstream text;
    text << "Failed to add resource record for collection "
      << collection->GetName() << " to the database." << std::endl;
    this->Log->Error(text.str());
    return false;
    }
  this->LastId = id;
  collection->SetId(id);

  mds::Collection mdsColl;
  mdsColl.SetObject(collection);
  if(!mdsColl.Commit())
    {
    std::stringstream text;
    text << "Failed to add collection " << collection->GetName() <<
      " to the database." << std::endl;
    this->Log->Error(text.str());
    return false;
    }

  if(!QDir(collection->GetName().c_str()).exists())
    {
    QDir().mkdir(collection->GetName().c_str());
    }
  this->LastDir = QDir::currentPath().toStdString() + "/" +
    collection->GetName();

  if(this->Recursive)
    {
    QString temp = QDir::currentPath();
    QDir::setCurrent(collection->GetName().c_str());
    for(std::vector<mdo::Item*>::const_iterator i = 
        collection->GetItems().begin();
        i != collection->GetItems().end(); ++i)
      {
      std::stringstream s;
      s << (*i)->GetId();
      this->SetServerHandle(s.str());
      this->SetResourceType(midasResourceType::ITEM);
      this->PullItem(id);
      }
    QDir::setCurrent(temp);
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
  remote.SetAuthenticator(this->Authenticator);
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

  if(community->HasAgreement() && this->AgreementHandler)
    {
    if(!this->AgreementHandler->HandleAgreement(this))
      {
      std::stringstream text;
      text << "You have not agreed to the license. Canceling pull."
        << std::endl;
      this->Log->Error(text.str());
      return false;
      }
    }

  // Pull any parents we need
  if(parentId == NO_PARENT && community->GetParentId())
    {
    bool recurse = this->Recursive;
    this->Recursive = false;
    std::string handle = this->ServerHandle;
    this->ServerHandle = community->GetParentStr();

    this->PullCommunity(NO_PARENT);
    QDir::setCurrent(this->LastDir.c_str());

    this->ServerHandle = handle;
    this->Recursive = recurse;
    parentId = this->LastId;
    }

  std::string topLevelDir = QDir::currentPath().toStdString();

  if(!QDir(community->GetName().c_str()).exists())
    {
    QDir().mkdir(community->GetName().c_str());
    }
  this->LastDir = QDir::currentPath().toStdString() + "/" +
    community->GetName();

  mds::DatabaseAPI db;
  int id = db.AddResource(midasResourceType::COMMUNITY,
    community->GetUuid(), QDir::currentPath().toStdString() + "/" +
    community->GetName(), community->GetName(), midasResourceType::COMMUNITY,
    parentId, 0);

  if(id <= 0)
    {
    std::stringstream text;
    text << "Failed to add resource record for community "
      << community->GetName() << " to the database." << std::endl;
    this->Log->Error(text.str());
    return false;
    }
  community->SetId(id);
  this->LastId = id;

  mds::Community mdsComm;
  mdsComm.SetObject(community);
  if(!mdsComm.Commit())
    {
    std::stringstream text;
    text << "Failed to add community " << community->GetName() <<
      " to the database." << std::endl;
    this->Log->Error(text.str());
    return false;
    }

  if(this->Recursive)
    {
    QDir::setCurrent(community->GetName().c_str());
    // Pull everything under this community.
    this->RecurseCommunities(id, community);
    }

  // Revert working dir to top level
  QDir::setCurrent(topLevelDir.c_str());
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
    this->SetResourceType(midasResourceType::COLLECTION);
    this->PullCollection(parentId);
    }
  for(std::vector<mdo::Community*>::const_iterator i =
      community->GetCommunities().begin();
      i != community->GetCommunities().end(); ++i)
    {
    std::stringstream s;
    s << (*i)->GetId();
    this->SetServerHandle(s.str());
    this->SetResourceType(midasResourceType::COMMUNITY);
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
  remote.SetAuthenticator(this->Authenticator);
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

  if(item->HasAgreement() && this->AgreementHandler)
    {
    if(!this->AgreementHandler->HandleAgreement(this))
      {
      std::stringstream text;
      text << "You have not agreed to the license. Canceling pull."
        << std::endl;
      this->Log->Error(text.str());
      return false;
      }
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
    this->ServerHandle = item->GetParentStr();
    this->ResourceType = midasResourceType::COLLECTION;
    this->PullCollection(NO_PARENT);
    QDir::setCurrent(this->LastDir.c_str());

    this->ResourceType = midasResourceType::ITEM;
    this->ServerHandle = handle;
    this->Recursive = recurse;
    parentId = this->LastId;
    }
  
  std::stringstream altTitle;
  altTitle << "item" << item->GetId();
  std::string title = item->GetTitle() == "" ? altTitle.str() :
    item->GetTitle();

  if(!QDir(title.c_str()).exists())
    {
    QDir().mkdir(title.c_str());
    }
  this->LastDir = QDir::currentPath().toStdString() + "/" + title;

  mds::DatabaseAPI db;
  int id = db.AddResource(midasResourceType::ITEM,
    item->GetUuid(), QDir::currentPath().toStdString() + "/" + title,
    item->GetTitle(), midasResourceType::COLLECTION, parentId, 0);

  if(id <= 0)
    {
    std::stringstream text;
    text << "Failed to add resource record for item "
      << item->GetName() << " to the database." << std::endl;
    this->Log->Error(text.str());
    return false;
    }

  this->LastId = id;
  item->SetId(id);
  
  mds::Item mdsItem;
  mdsItem.SetObject(item);
  if(!mdsItem.Commit())
    {
    std::stringstream text;
    text << "Failed to add item " << item->GetName() <<
      " to the database." << std::endl;
    this->Log->Error(text.str());
    return false;
    }

  if(this->Recursive)
    {
    QString temp = QDir::currentPath();
    QDir::setCurrent(title.c_str());
    for(std::vector<mdo::Bitstream*>::const_iterator i = 
        item->GetBitstreams().begin();
        i != item->GetBitstreams().end(); ++i)
      {
      std::stringstream s;
      s << (*i)->GetId();
      this->SetServerHandle(s.str());
      this->SetResourceType(midasResourceType::BITSTREAM);
      this->PullBitstream(id);
      }
    QDir::setCurrent(temp);
    }

  delete item;
  return true;
}

//-------------------------------------------------------------------
int midasSynchronizer::Pull3()
{
  switch(this->ResourceType3)
    {
    case midas3ResourceType::COMMUNITY:
    case midas3ResourceType::FOLDER:
      return this->PullFolder3(NULL) ? MIDAS_OK : MIDAS_FAILURE;
    case midas3ResourceType::ITEM:
      return this->PullItem3(NULL) ? MIDAS_OK : MIDAS_FAILURE;
    case midas3ResourceType::BITSTREAM:
      return this->PullBitstream3(NULL) ? MIDAS_OK : MIDAS_FAILURE;
    default:
      return MIDAS_NO_RTYPE;
    }
}

//-------------------------------------------------------------------
bool midasSynchronizer::PullFolder3(m3do::Folder* parentFolder)
{
  if(this->ShouldCancel)
    {
    return false;
    }

  bool isComm = this->ResourceType3 == midas3ResourceType::COMMUNITY;
  m3ws::Folder remote;
  m3do::Folder* folder = isComm ? new m3do::Community : new m3do::Folder;
  folder->SetId(atoi(this->GetServerHandle().c_str()));
  remote.SetAuthenticator(this->Authenticator);
  remote.SetObject(folder);

  this->Progress->SetIndeterminate();
  this->Log->Status("Fetching folder information...");
  if(!remote.Fetch())
    {
    std::stringstream text;
    text << "Unable to fetch the folder via the Web API"
      << std::endl;
    Log->Error(text.str());
    delete folder;
    return false;
    }

  std::stringstream status;
  status << "Pulled " << folder->GetTypeName() << " " << folder->GetName();
  this->Log->Status(status.str());

  // Pull any parents we need
  if(!parentFolder && folder->GetParentId() > 0)
    {
    //TODO check if parent is a community or a folder,
    //set resourcetype3 accordingly
    bool recurse = this->Recursive;
    this->Recursive = false;
    std::string handle = this->ServerHandle;
    this->ServerHandle = folder->GetParentStr();

    if(!this->PullFolder3(NULL))
      {
      delete folder;
      return false;
      }
    QDir::setCurrent(this->LastDir.c_str());

    this->ServerHandle = handle;
    this->Recursive = recurse;
    }

  if(!QDir(folder->GetName().c_str()).exists())
    {
    QDir().mkpath(folder->GetName().c_str());
    }
  this->LastDir = QDir::currentPath().toStdString() + "/" + folder->GetName();
  folder->SetPath(this->LastDir);

  if(!parentFolder && folder->GetParentId() > 0)
    {
    m3do::Folder* parent = new m3do::Folder;
    parent->SetId(this->LastId);
    folder->SetParentFolder(parent);
    }
  else
    {
    folder->SetParentFolder(parentFolder);
    }

  m3ds::Folder local;
  local.SetObject(folder);
  if(!local.Create())
    {
    std::stringstream text;
    text << "Failed to add resource record for folder "
      << folder->GetName() << " to the database." << std::endl;
    this->Log->Error(text.str());
    delete folder;
    return false;
    }
  this->LastId = folder->GetId();
  if(!parentFolder && folder->GetParentId() > 0)
    {
    delete folder->GetParentFolder();
    }

  bool ok = true;
  if(this->Recursive)
    {
    folder->SetId(atoi(this->GetServerHandle().c_str()));
    remote.FetchTree();
    folder->SetId(this->LastId); //switch back to client side id
    QString temp = QDir::currentPath();
    QDir::setCurrent(folder->GetName().c_str());
    for(std::vector<m3do::Folder*>::const_iterator i = 
        folder->GetFolders().begin();
        i != folder->GetFolders().end(); ++i)
      {
      std::stringstream id;
      id << (*i)->GetId();
      this->SetServerHandle(id.str());
      this->SetResourceType3(midas3ResourceType::FOLDER);
      ok &= this->PullFolder3(folder);
      }
    for(std::vector<m3do::Item*>::const_iterator i = 
        folder->GetItems().begin();
        i != folder->GetItems().end(); ++i)
      {
      std::stringstream id;
      id << (*i)->GetId();
      this->SetServerHandle(id.str());
      this->SetResourceType3(midas3ResourceType::ITEM);
      ok &= this->PullItem3(folder);
      }
    QDir::setCurrent(temp);
    }

  delete folder;
  return ok;
}

//-------------------------------------------------------------------
bool midasSynchronizer::PullItem3(m3do::Folder* parentFolder)
{
  if(this->ShouldCancel)
    {
    return false;
    }

  m3ws::Item remote;
  m3do::Item* item = new m3do::Item;
  item->SetId(atoi(this->GetServerHandle().c_str()));
  remote.SetAuthenticator(this->Authenticator);
  remote.SetObject(item);

  this->Progress->SetIndeterminate();
  this->Log->Status("Fetching item information...");
  if(!remote.Fetch())
    {
    std::stringstream text;
    text << "Unable to fetch the item via the Web API"
      << std::endl;
    Log->Error(text.str());
    delete item;
    return false;
    }

  std::stringstream status;
  status << "Pulled item " << item->GetName();
  this->Log->Status(status.str());

  // Pull any parents we need
  if(!parentFolder)
    {
    bool recurse = this->Recursive;
    this->Recursive = false;
    std::string handle = this->ServerHandle;
    this->ServerHandle = item->GetParentStr();
    this->ResourceType = midas3ResourceType::FOLDER;
    if(!this->PullFolder3(NULL))
      {
      delete item;
      return false;
      }
    QDir::setCurrent(this->LastDir.c_str());

    this->ResourceType = midas3ResourceType::ITEM;
    this->ServerHandle = handle;
    this->Recursive = recurse;
    }

  if(!QDir(item->GetName().c_str()).exists())
    {
    QDir().mkpath(item->GetName().c_str());
    }
  this->LastDir = QDir::currentPath().toStdString() + "/" + item->GetName();
  item->SetPath(this->LastDir);

  if(!parentFolder)
    {
    m3do::Folder* parent = new m3do::Folder;
    parent->SetId(this->LastId);
    item->SetParentFolder(parent);
    }
  else
    {
    item->SetParentFolder(parentFolder);
    }

  m3ds::Item local;
  local.SetObject(item);
  if(!local.Create())
    {
    std::stringstream text;
    text << "Failed to add resource record for item "
      << item->GetName() << " to the database." << std::endl;
    this->Log->Error(text.str());
    delete item;
    return false;
    }
  this->LastId = item->GetId();
  if(!parentFolder)
    {
    delete item->GetParentFolder();
    }

  bool ok = true;
  if(this->Recursive)
    {
    QString temp = QDir::currentPath();
    QDir::setCurrent(item->GetName().c_str());
    for(std::vector<m3do::Bitstream*>::const_iterator i = 
        item->GetBitstreams().begin();
        i != item->GetBitstreams().end(); ++i)
      {
      std::stringstream id;
      id << (*i)->GetId();
      this->SetServerHandle(id.str());
      this->SetResourceType3(midas3ResourceType::BITSTREAM);
      ok &= this->PullBitstream3(item);
      }
    QDir::setCurrent(temp);
    }

  delete item;
  return ok;
}

//-------------------------------------------------------------------
bool midasSynchronizer::PullBitstream3(m3do::Item* parentItem)
{
  if(this->ShouldCancel)
    {
    return false;
    }

  m3ws::Bitstream remote;
  m3do::Bitstream* bitstream = new m3do::Bitstream;
  bitstream->SetId(atoi(this->GetServerHandle().c_str()));
  remote.SetAuthenticator(this->Authenticator);
  remote.SetObject(bitstream);

  this->Progress->SetIndeterminate();
  this->Log->Status("Fetching bitstream information...");
  if(!remote.Fetch())
    {
    std::stringstream text;
    text << "Unable to fetch the bitstream information via the Web API"
      << std::endl;
    Log->Error(text.str());
    delete bitstream;
    return false;
    }

  // Pull any parents we need
  if(!parentItem)
    {
    bool recurse = this->Recursive;
    this->Recursive = false;
    std::string handle = this->ServerHandle;
    this->ServerHandle = bitstream->GetParentStr();
    this->ResourceType = midas3ResourceType::ITEM;
    if(!this->PullItem3(NULL))
      {
      delete bitstream;
      return false;
      }
    QDir::setCurrent(this->LastDir.c_str());

    this->ResourceType = midas3ResourceType::BITSTREAM;
    this->ServerHandle = handle;
    this->Recursive = recurse;
    }


  if(this->Progress)
    {
    mws::WebAPI::Instance()->SetProgressReporter(this->Progress);
    this->Progress->SetMessage(bitstream->GetName());
    this->Progress->UpdateOverallCount(this->CurrentBitstreams);
    this->Progress->UpdateProgress(0, 0);
    this->Progress->ResetProgress();
    }

  if(!parentItem)
    {
    m3do::Item* parent = new m3do::Item;
    parent->SetId(this->LastId);
    bitstream->SetParentItem(parent);
    }
  else
    {
    bitstream->SetParentItem(parentItem);
    }

  m3ds::Bitstream local;
  local.SetObject(bitstream);
  if(local.AlreadyExistsInItem())
    {
    // We've already got this bitstream in the client tree
    delete bitstream;
    return true;
    }

  this->LastDir = QDir::currentPath().toStdString() + "/" + bitstream->GetName();
  bitstream->SetPath(this->LastDir);

  // Skip download if we already have a bitstream with this checksum
  if(!local.CopyContentIfExists())
    {
    if(!remote.Download())
      {
      std::stringstream text;
      text << "Failed to download bitstream " << bitstream->GetName()
        << std::endl;
      this->Log->Error(text.str());
      delete bitstream;
      return false;
      }
    }
  QFileInfo fileInfo(this->LastDir.c_str());
  bitstream->SetLastModified(fileInfo.lastModified().toTime_t());
  bitstream->SetId(0); //create, not update

  if(!local.Commit())
    {
    std::stringstream text;
    text << "Failed to add resource record for item "
      << bitstream->GetName() << " to the database." << std::endl;
    this->Log->Error(text.str());
    delete bitstream;
    return false;
    }
  if(!parentItem)
    {
    delete bitstream->GetParentItem();
    }

  delete bitstream;
  return true;
}

//-------------------------------------------------------------------
int midasSynchronizer::Push()
{
  if(std::string(mws::WebAPI::Instance()->GetServerUrl()) == "")
    {
    std::stringstream text;
    text << "You must specify a server url. No last used URL "
      "exists in the database." << std::endl;
    Log->Error(text.str());
    return MIDAS_NO_URL;
    }

  if(SERVER_IS_MIDAS3)
    {
    return this->Push3();
    }

  bool ok = true;
  mdo::Community* comm = NULL;
  mdo::Collection* coll = NULL;
  mdo::Item* item = NULL;
  mdo::Bitstream* bitstream = NULL;

  switch(this->ResourceType)
    {
    case midasResourceType::COMMUNITY:
      comm = new mdo::Community;
      comm->SetId(atoi(this->ClientHandle.c_str()));
      comm->SetUuid(this->ServerHandle.c_str());
      ok = this->Push(comm);
      delete comm;
      return ok ? MIDAS_OK : MIDAS_FAILURE;
    case midasResourceType::COLLECTION:
      coll = new mdo::Collection;
      coll->SetId(atoi(this->ClientHandle.c_str()));
      coll->SetUuid(this->ServerHandle.c_str());
      ok = this->Push(coll);
      delete coll;
      return ok ? MIDAS_OK : MIDAS_FAILURE;
    case midasResourceType::ITEM:
      item = new mdo::Item;
      item->SetId(atoi(this->ClientHandle.c_str()));
      item->SetUuid(this->ServerHandle.c_str());
      ok = this->Push(item);
      delete item;
      return ok ? MIDAS_OK : MIDAS_FAILURE;
    case midasResourceType::BITSTREAM:
      bitstream = new mdo::Bitstream;
      bitstream->SetId(atoi(this->ClientHandle.c_str()));
      bitstream->SetUuid(this->ServerHandle.c_str());
      ok = this->Push(bitstream);
      delete bitstream;
      return ok ? MIDAS_OK : MIDAS_FAILURE;
    case midasResourceType::NONE:
    default:
      return this->PushAllDirty();
    }
}

//-------------------------------------------------------------------
int midasSynchronizer::Push3()
{
  bool ok = true;
  m3do::Community* comm = NULL;
  m3do::Folder* folder = NULL;
  m3do::Item* item = NULL;
  m3do::Bitstream* bitstream = NULL;

  switch(this->ResourceType3)
    {
    case midas3ResourceType::COMMUNITY:
      comm = new m3do::Community;
      comm->SetId(atoi(this->ClientHandle.c_str()));
      comm->SetUuid(this->ServerHandle.c_str());
      ok = this->Push(comm);
      delete comm;
      return ok ? MIDAS_OK : MIDAS_FAILURE;
    case midas3ResourceType::FOLDER:
      folder = new m3do::Folder;
      folder->SetId(atoi(this->ClientHandle.c_str()));
      folder->SetUuid(this->ServerHandle.c_str());
      ok = this->Push(folder);
      delete folder;
      return ok ? MIDAS_OK : MIDAS_FAILURE;
    case midas3ResourceType::ITEM:
      item = new m3do::Item;
      item->SetId(atoi(this->ClientHandle.c_str()));
      item->SetUuid(this->ServerHandle.c_str());
      ok = this->Push(item);
      delete item;
      return ok ? MIDAS_OK : MIDAS_FAILURE;
    case midas3ResourceType::BITSTREAM:
      bitstream = new m3do::Bitstream;
      bitstream->SetId(atoi(this->ClientHandle.c_str()));
      bitstream->SetUuid(this->ServerHandle.c_str());
      ok = this->Push(bitstream);
      delete bitstream;
      return ok ? MIDAS_OK : MIDAS_FAILURE;
    case midasResourceType::NONE:
    default:
      //TODO return this->PushAllDirty();
      return MIDAS_FAILURE;
    }
}

//-------------------------------------------------------------------
int midasSynchronizer::PushAllDirty()
{
  mds::DatabaseAPI db;
  std::vector<midasStatus> dirties = db.GetStatusEntries();

  if(!dirties.size())
    {
    std::stringstream text;
    text << "There are no staged resources to push." << std::endl;
    Log->Error(text.str());
    return MIDAS_FAILURE;
    }

  bool success = true;
  this->Recursive = false;
  for(std::vector<midasStatus>::iterator i = dirties.begin();
      i != dirties.end(); ++i)
    {
    if(i->GetUuid() == "")
      {
      this->Log->Error("Skipping invalid dirty resource entry.\n");
      continue;
      }
    midasResourceRecord record = db.GetRecordByUuid(i->GetUuid());

    if(record.Id <= 0)
      {
      std::stringstream text;
      text << "Invalid uuid " << record.Uuid << ". Cannot push" << std::endl;
      this->Log->Error(text.str());
      continue;
      }

    mdo::Community* comm = NULL;
    mdo::Collection* coll = NULL;
    mdo::Item* item = NULL;
    mdo::Bitstream* bitstream = NULL;

    switch(record.Type)
      {
      case midasResourceType::COMMUNITY:
        comm = new mdo::Community;
        comm->SetId(record.Id);
        comm->SetUuid(record.Uuid.c_str());
        success &= this->Push(comm);
        delete comm;
        break;
      case midasResourceType::COLLECTION:
        coll = new mdo::Collection;
        coll->SetId(record.Id);
        coll->SetUuid(record.Uuid.c_str());
        success &= this->Push(coll);
        delete coll;
        break;
      case midasResourceType::ITEM:
        item = new mdo::Item;
        item->SetId(record.Id);
        item->SetUuid(record.Uuid.c_str());
        success &= this->Push(item);
        delete item;
        break;
      case midasResourceType::BITSTREAM:
        bitstream = new mdo::Bitstream;
        bitstream->SetId(record.Id);
        bitstream->SetUuid(record.Uuid.c_str());
        success &= this->Push(bitstream);
        delete bitstream;
        break;
      default:
        return MIDAS_NO_RTYPE;
      }

    if(!success && this->Authenticator->IsAnonymous())
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
    std::string server_parentId;
    // Get uuid from parent id/type
    mds::DatabaseAPI db;
    std::string parentUuid = db.GetUuid(type, parentId);

    // Get server-side id of parent from the uuid
    mws::WebAPI::Instance()->GetIdByUuid(parentUuid, server_parentId);
    parentId = atoi(server_parentId.c_str());
    }
  return parentId;
}

//-------------------------------------------------------------------
int midasSynchronizer::GetServerParentId3(const std::string& parentUuid)
{
  std::string serverParentId;
  mws::WebAPI::Instance()->GetIdByUuid(parentUuid, serverParentId);
  return atoi(serverParentId.c_str());
}

//-------------------------------------------------------------------
bool midasSynchronizer::Push(mdo::Bitstream* bitstream)
{
  if(this->ShouldCancel)
    {
    return false;
    }

  mds::Bitstream mdsBitstream;
  mdsBitstream.SetObject(bitstream);
  mdsBitstream.Fetch();

  std::string name = bitstream->GetName();
  std::string size = bitstream->GetSize();

  this->CurrentBitstreams++;

  if(midasUtils::GetFileLength(bitstream->GetPath().c_str()) == 0)
    {
    std::stringstream text;
    text << "Error: \"" << bitstream->GetPath() << "\" is 0 bytes. You "
      "may not push an empty bitstream." << std::endl;
    Log->Error(text.str());
    return false;
    }

  mds::DatabaseAPI db;
  if(bitstream->GetParentId() == 0)
    {
    bitstream->SetParentId(this->GetServerParentId(midasResourceType::ITEM,
      db.GetParentId(midasResourceType::BITSTREAM, bitstream->GetId())));
    }
  if(bitstream->GetParentId() == 0)
    {
    std::stringstream text;
    text << "The parent of bitstream \"" << name <<
      "\" could not be resolved." << std::endl;
    this->Log->Error(text.str());
    return false;
    }

  std::stringstream status;
  status << "Uploading bitstream " << name << "...";
  this->Log->Status(status.str());

  if(this->Progress)
    {
    mws::WebAPI::Instance()->SetProgressReporter(this->Progress);
    this->Progress->UpdateOverallCount(this->CurrentBitstreams);
    this->Progress->SetMessage(name);
    this->Progress->ResetProgress();
    }

  mws::Bitstream mwsBitstream;
  mwsBitstream.SetObject(bitstream);

  if(mwsBitstream.Upload())
    {
    // Clear dirty flag on the resource
    db.ClearDirtyResource(bitstream->GetUuid());
    std::stringstream text;
    text << "Pushed bitstream " << name << std::endl;
    Log->Message(text.str());
    return true;
    }
  else
    {
    std::stringstream text;
    text << "Failed to push bitstream " << name << std::endl;
    Log->Error(text.str());
    return false;
    }
}

//-------------------------------------------------------------------
bool midasSynchronizer::Push(mdo::Collection* coll)
{
  if(this->ShouldCancel)
    {
    return false;
    }

  mds::Collection mdsColl;
  mdsColl.SetObject(coll);
  mdsColl.Fetch();

  mds::DatabaseAPI db;

  if(coll->GetParentId() == 0)
    {
    coll->SetParentId(this->GetServerParentId(midasResourceType::COMMUNITY,
      db.GetParentId(midasResourceType::COLLECTION, coll->GetId())));
    }
  if(coll->GetParentId() == 0)
    {
    std::stringstream text;
    text << "The parent of collection \"" << coll->GetName() <<
      "\" could not be resolved." << std::endl;
    Log->Error(text.str());
    return false;
    }

  this->Progress->SetIndeterminate();
  std::stringstream status;
  status << "Uploading collection " << coll->GetName() << "...";
  this->Log->Status(status.str());

  mws::Collection mwsColl;
  mwsColl.SetObject(coll);
  if(mwsColl.Commit())
    {
    // Clear dirty flag on the resource
    db.ClearDirtyResource(coll->GetUuid());
    std::stringstream text;
    text << "Pushed collection " << coll->GetName() << std::endl;
    Log->Message(text.str());

    if(this->Recursive)
      {
      mdsColl.SetRecursive(false); //only fetch immediate children
      mdsColl.FetchTree();
      bool ok = true;
      for(std::vector<mdo::Item*>::const_iterator i = coll->GetItems().begin();
          i != coll->GetItems().end(); ++i)
        {
        ok &= this->Push(*i);
        }
      return ok;
      }
    else
      {
      return true;
      }
    }
  else
    {
    std::stringstream text;
    text << "Failed to push collection " << coll->GetName() << std::endl;
    Log->Error(text.str());
    return false;
    }
}

//-------------------------------------------------------------------
bool midasSynchronizer::Push(mdo::Community* comm)
{
  if(this->ShouldCancel)
    {
    return false;
    }

  mds::DatabaseAPI db;

  if(comm->GetParentId() == 0)
    {
    comm->SetParentId(this->GetServerParentId(midasResourceType::COMMUNITY,
      db.GetParentId(midasResourceType::COMMUNITY, comm->GetId())));
    }

  mds::Community mdsComm;
  mdsComm.SetObject(comm);
  if(!mdsComm.Fetch())
    {
    return false;
    }

  this->Progress->SetIndeterminate();
  std::stringstream status;
  status << "Uploading community " << comm->GetName() << "...";
  this->Log->Status(status.str());

  // Create new community on server
  mws::Community mwsComm;
  mwsComm.SetObject(comm);
  if(mwsComm.Commit())
    {
    // Clear dirty flag on the resource
    db.ClearDirtyResource(comm->GetUuid());
    std::stringstream text;
    text << "Pushed community " << comm->GetName() << std::endl;
    Log->Message(text.str());
    Log->Status(text.str());

    if(this->Recursive)
      {
      mdsComm.SetRecursive(false);
      mdsComm.FetchTree();
      bool ok = true;
      for(std::vector<mdo::Community*>::const_iterator i =
          comm->GetCommunities().begin(); i != comm->GetCommunities().end();
          ++i)
        {
        ok &= this->Push(*i);
        }
      for(std::vector<mdo::Collection*>::const_iterator i =
          comm->GetCollections().begin(); i != comm->GetCollections().end();
          ++i)
        {
        ok &= this->Push(*i);
        }
      return ok;
      }
    else
      {
      return true;
      }
    }
  else
    {
    std::stringstream text; 
    text << "Failed to push community " << comm->GetName() << std::endl;
    Log->Error(text.str());
    Log->Status(text.str());
    return false;
    }
}

//-------------------------------------------------------------------
bool midasSynchronizer::Push(mdo::Item* item)
{
  if(this->ShouldCancel)
    {
    return false;
    }
  mds::Item mdsItem;
  mdsItem.SetObject(item);
  mdsItem.Fetch();

  mds::DatabaseAPI db;

  if(item->GetParentId() == 0)
    {
    item->SetParentId(this->GetServerParentId(midasResourceType::COLLECTION,
      db.GetParentId(midasResourceType::ITEM, item->GetId())));
    }
  if(item->GetParentId() == 0)
    {
    std::stringstream text;
    text << "The parent of item \"" << item->GetTitle() <<
      "\" could not be resolved." << std::endl;
    Log->Error(text.str());
    return false;
    }

  this->Progress->SetIndeterminate();
  std::stringstream status;
  status << "Uploading item " << item->GetTitle() << "...";
  this->Log->Status(status.str());
  
  mws::Item mwsItem;
  mwsItem.SetObject(item);

  if(mwsItem.Commit())
    {
    // Clear dirty flag on the resource
    db.ClearDirtyResource(item->GetUuid());
    std::stringstream text;
    text << "Pushed item " << item->GetTitle() << std::endl;
    Log->Message(text.str());
    
    if(this->Recursive)
      {
      mdsItem.FetchTree();
      bool ok = true;
      for(std::vector<mdo::Bitstream*>::const_iterator i =
          item->GetBitstreams().begin(); i != item->GetBitstreams().end();
          ++i)
        {
        ok &= this->Push(*i);
        }
      return ok;
      }
    else
      {
      return true;
      }
    }
  else
    {
    std::stringstream text;
    text << "Failed to push item " << item->GetTitle() << std::endl;
    Log->Error(text.str());
    return false;
    }
}

//-------------------------------------------------------------------
bool midasSynchronizer::Push(m3do::Folder* folder)
{
  if(this->ShouldCancel)
    {
    return false;
    }

  m3ds::Folder mdsFolder;
  mdsFolder.SetObject(folder);
  if(!mdsFolder.Fetch() || !mdsFolder.FetchParent())
    {
    return false;
    }

  if(folder->GetParentFolder())
    {
    folder->SetParentId(
      this->GetServerParentId3(folder->GetParentFolder()->GetUuid()));
    }
  else
    {
    folder->SetParentId(-1); //top level user folder
    }

  this->Progress->SetIndeterminate();
  std::stringstream status;
  status << "Uploading " << folder->GetTypeName() << " "
    << folder->GetName() << "...";
  this->Log->Status(status.str());

  // Create new folder on server
  m3ws::Folder mwsFolder;
  mwsFolder.SetObject(folder);
  if(mwsFolder.Commit())
    {
    //TODO Clear dirty flag on the resource
    //db.ClearDirtyResource(comm->GetUuid());
    std::stringstream text;
    text << "Pushed " << folder->GetTypeName() << " "
      << folder->GetName() << std::endl;
    Log->Message(text.str());
    Log->Status(text.str());

    if(this->Recursive)
      {
      mdsFolder.SetRecursive(false);
      mdsFolder.FetchTree();
      bool ok = true;
      for(std::vector<m3do::Folder*>::const_iterator i =
          folder->GetFolders().begin(); i != folder->GetFolders().end();
          ++i)
        {
        ok &= this->Push(*i);
        }
      for(std::vector<m3do::Item*>::const_iterator i =
          folder->GetItems().begin(); i != folder->GetItems().end();
          ++i)
        {
        ok &= this->Push(*i);
        }
      delete folder->GetParentFolder();
      return ok;
      }
    else
      {
      delete folder->GetParentFolder();
      return true;
      }
    }
  else
    {
    std::stringstream text; 
    text << "Failed to push " << folder->GetTypeName() << " "
      << folder->GetName() << std::endl;
    Log->Error(text.str());
    Log->Status(text.str());
    delete folder->GetParentFolder();
    return false;
    }
}

//-------------------------------------------------------------------
bool midasSynchronizer::Push(m3do::Item* item)
{
  if(this->ShouldCancel)
    {
    return false;
    }

  m3ds::Item mdsItem;
  mdsItem.SetObject(item);
  if(!mdsItem.Fetch() || !mdsItem.FetchParent())
    {
    return false;
    }

  item->SetParentId(
    this->GetServerParentId3(item->GetParentFolder()->GetUuid()));
  if(!item->GetParentId())
    {
    std::stringstream text;
    text << "The parent folder of item " << item->GetName() <<
      " could not be found on the server." << std::endl;
    this->Log->Error(text.str());
    delete item->GetParentFolder();
    return false;
    }

  this->Progress->SetIndeterminate();
  std::stringstream status;
  status << "Uploading item " << item->GetName() << "...";
  this->Log->Status(status.str());

  // Create new item on server
  m3ws::Item mwsItem;
  mwsItem.SetObject(item);
  if(mwsItem.Commit())
    {
    //TODO Clear dirty flag on the resource
    //db.ClearDirtyResource(comm->GetUuid());
    std::stringstream text;
    text << "Pushed item " << item->GetName() << std::endl;
    Log->Message(text.str());
    Log->Status(text.str());

    if(this->Recursive)
      {
      mdsItem.FetchTree();
      bool ok = true;
      for(std::vector<m3do::Bitstream*>::const_iterator i =
          item->GetBitstreams().begin(); i != item->GetBitstreams().end();
          ++i)
        {
        ok &= this->Push(*i);
        }
      delete item->GetParentFolder();
      return ok;
      }
    else
      {
      delete item->GetParentFolder();
      return true;
      }
    }
  else
    {
    std::stringstream text; 
    text << "Failed to push item " << item->GetName() << std::endl;
    this->Log->Error(text.str());
    this->Log->Status(text.str());
    delete item->GetParentFolder();
    return false;
    }
}

//-------------------------------------------------------------------
bool midasSynchronizer::Push(m3do::Bitstream* bitstream)
{
  if(this->ShouldCancel)
    {
    return false;
    }

  m3ds::Bitstream mdsBitstream;
  mdsBitstream.SetObject(bitstream);
  if(!mdsBitstream.Fetch() || !mdsBitstream.FetchParent())
    {
    return false;
    }

  bitstream->SetParentId(
    this->GetServerParentId3(bitstream->GetParentItem()->GetUuid()));
  if(!bitstream->GetParentId())
    {
    std::stringstream text;
    text << "The parent item of bitstream " << bitstream->GetName() <<
      " could not be found on the server." << std::endl;
    this->Log->Error(text.str());
    delete bitstream->GetParentItem();
    return false;
    }

  this->Progress->SetIndeterminate();
  std::stringstream status;
  status << "Uploading bitstream " << bitstream->GetName() << "...";
  this->Log->Status(status.str());

  // Create new item on server
  m3ws::Bitstream mwsBitstream;
  mwsBitstream.SetObject(bitstream);
  if(mwsBitstream.Upload())
    {
    //TODO Clear dirty flag on the resource
    //db.ClearDirtyResource(comm->GetUuid());
    std::stringstream text;
    text << "Pushed bitstream " << bitstream->GetName() << std::endl;
    Log->Message(text.str());
    Log->Status(text.str());
    delete bitstream->GetParentItem();
    return true;
    }
  else
    {
    std::stringstream text; 
    text << "Failed to push bitstream " << bitstream->GetName() << std::endl;
    this->Log->Error(text.str());
    this->Log->Status(text.str());
    delete bitstream->GetParentItem();
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

  QFileInfo fileInfo1(this->ClientHandle.c_str());
  if(!fileInfo1.isFile())
    {
    std::string originalPath = this->ClientHandle;
    this->ClientHandle = QDir::currentPath().toStdString() + "/" + originalPath;
    QFileInfo fileInfo2(this->ClientHandle.c_str());
    if(!fileInfo2.isFile())
      {
      std::stringstream text;
      text << "Error: The source file \"" << originalPath <<
        "\" does not exist." << std::endl;
      this->Log->Error(text.str());
      return MIDAS_INVALID_PATH;
      }
    }

  if(std::string(mws::WebAPI::Instance()->GetServerUrl()) == "")
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

  if(!this->GetAuthenticator()->Login())
    {
    std::stringstream text;
    text << "Error: Bad login credentials." << std::endl;
    this->Log->Error(text.str());
    return MIDAS_LOGIN_FAILED;
    }

  this->ResourceType = midasResourceType::BITSTREAM;

  mdo::Bitstream* bitstream = new mdo::Bitstream;
  
  int rc;
  if((rc = this->Add(bitstream)) != MIDAS_OK)
    {
    return rc;
    }

  rc = this->Push(bitstream) ? MIDAS_OK : MIDAS_FAILURE;

  delete bitstream;
  return rc;
}

//-------------------------------------------------------------------
bool midasSynchronizer::ResumeDownload()
{
  mds::PartialDownload* partial =
    reinterpret_cast<mds::PartialDownload*>(this->Object);

  QFileInfo info(partial->GetPath().c_str());
  info.setCaching(false);
  if(this->Progress)
    {
    mws::WebAPI::Instance()->SetProgressReporter(this->Progress);
    this->Progress->SetMessage(info.fileName().toStdString());
    this->Progress->UpdateOverallCount(1);
    this->Progress->UpdateProgress(0, 0);
    this->Progress->ResetProgress();
    }
  
  std::string serverId;
  if(!mws::WebAPI::Instance()->GetIdByUuid(partial->GetUuid(), serverId))
    {
    std::stringstream text;
    text << "The bitstream you were downloading was not found on this server."
      << std::endl;
    this->Log->Error(text.str());
    return false;
    }

  mdo::Bitstream resumeObj;
  resumeObj.SetId(atoi(serverId.c_str()));
  resumeObj.SetName(partial->GetPath().c_str());

  mws::Bitstream mwsBitstream;
  mwsBitstream.SetObject(&resumeObj);
  mwsBitstream.SetOffset(info.size());
  
  if(!mwsBitstream.Download())
    {
    std::stringstream text;
    text << "Download of " << info.fileName().toStdString() << " failed."
      << std::endl;
    this->Log->Error(text.str());
    return false;
    }

  mds::DatabaseAPI db;
  int id = db.AddResource(midasResourceType::BITSTREAM, partial->GetUuid(),
    partial->GetPath(), info.fileName().toStdString(),
    midasResourceType::ITEM, partial->GetParentItem(), 0);

  if(id <= 0)
    {
    std::stringstream text;
    text << "Failed to add resource record for bistream "
      << info.fileName().toStdString() << " to the database." << std::endl;
    this->Log->Error(text.str());
    return false;
    }
  std::stringstream stream;
  stream << info.size();
  mdo::Bitstream bitstream;
  bitstream.SetName(info.fileName().toStdString().c_str());
  bitstream.SetSize(stream.str());
  bitstream.SetId(id);
  bitstream.SetLastModified(info.lastModified().toTime_t());

  mds::Bitstream mdsBitstream;
  mdsBitstream.SetObject(&bitstream);
  if(!mdsBitstream.Commit())
    {
    std::stringstream text;
    text << "Failed to add bitstream " << bitstream.GetName() <<
      " to the database." << std::endl;
    this->Log->Error(text.str());
    return false;
    }
  partial->Remove();
  return true;
}

//-------------------------------------------------------------------
bool midasSynchronizer::ResumeUpload()
{
  mds::PartialUpload* partial =
    reinterpret_cast<mds::PartialUpload*>(this->Object);

  mdo::Bitstream bitstream;
  bitstream.SetId(partial->GetBitstreamId());
  bitstream.SetParentId(partial->GetParentItem());
  
  mds::Bitstream mdsBitstream;
  mdsBitstream.SetObject(&bitstream);
  if(!mdsBitstream.Fetch())
    {
    std::stringstream text;
    text << "The bitstream you were uploading no longer exists."
      << std::endl;
    this->Log->Error(text.str());
    return false;
    }

  QFileInfo info(bitstream.GetPath().c_str());
  if(this->Progress)
    {
    mws::WebAPI::Instance()->SetProgressReporter(this->Progress);
    this->Progress->SetMessage(info.fileName().toStdString());
    this->Progress->UpdateOverallCount(1);
    this->Progress->UpdateProgress(0, 0);
    this->Progress->ResetProgress();
    }

  int64 offset;
  if(!mws::WebAPI::Instance()->GetUploadOffset(
     partial->GetToken(), offset))
    {
    std::stringstream text;
    text << "The server has no record of your partial upload."
      << std::endl;
    this->Log->Error(text.str());
    return false;
    }

  mws::Bitstream mwsBitstream;
  mwsBitstream.SetObject(&bitstream);
  mwsBitstream.SetOffset(offset);

  if(!mwsBitstream.ResumeUpload(partial->GetToken(), partial->GetUserId()))
    {
    std::stringstream text;
    text << "Upload of " << info.fileName().toStdString() << " failed."
      << std::endl;
    this->Log->Error(text.str());
    return false;
    }
  else
    {
    mds::DatabaseAPI db;
    db.ClearDirtyResource(bitstream.GetUuid());
    std::stringstream text;
    text << "Pushed bitstream " << bitstream.GetName() << std::endl;
    Log->Message(text.str());

    partial->Remove();
    return true;
    }
}

//-------------------------------------------------------------------
bool midasSynchronizer::ConvertPathToId()
{
  std::string type, id, uuid;
  if(!mws::WebAPI::Instance()->GetIdFromPath(
      this->ServerHandle, type, id, uuid))
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
  this->Recursive = false;
  this->SetOperation(midasSynchronizer::OPERATION_NONE);
  this->SetServerHandle("");
  this->SetClientHandle("");
  this->Uuid = "";
  this->SetResourceType(midasResourceType::NONE);
  this->SetResourceType3(midas3ResourceType::NONE);
  this->Log->Status("");
  this->CurrentBitstreams = 0;
  this->TotalBitstreams = 0;
  this->Progress->SetMessage("");
  this->Progress->ResetProgress();
  this->Progress->ResetOverall();
}

//-------------------------------------------------------------------
void midasSynchronizer::Cancel()
{
  this->ShouldCancel = true;
  mws::WebAPI::Instance()->Cancel();
}

//-------------------------------------------------------------------
void midasSynchronizer::ChangeToRootDir()
{
  std::string wdir;
  mds::DatabaseAPI db;

  if(this->GetAuthenticator()->GetProfile() != "")
    {
    wdir = db.GetAuthProfile(this->Authenticator->GetProfile()).RootDir;
    }

  // If no profile-specific setting exists, fall back to global root dir
  if(wdir == "")
    {
    wdir = db.GetSetting(mds::DatabaseAPI::ROOT_DIR);
    }
  if(wdir == "")
    {
    QFileInfo dir(mds::DatabaseInfo::Instance()->GetPath().c_str());
    wdir = dir.path().toStdString();
    }

  QFileInfo fileInfo(wdir.c_str());
  if(wdir != "" && fileInfo.isDir())
    {
    QDir::setCurrent(wdir.c_str());
    }
}

//-------------------------------------------------------------------
std::string midasSynchronizer::ResolveAddPath()
{
  std::string path;

  QFileInfo fileInfo(this->ClientHandle.c_str());
  if(fileInfo.isAbsolute())
    {
    path = this->ClientHandle;
    }
  else if(fileInfo.exists())
    {
    path = QDir::currentPath().toStdString() + "/" + this->ClientHandle;
    }
  else if(this->ResourceType != midasResourceType::BITSTREAM)
    {
    path = this->ClientHandle;
    }

  return path;
}

//-------------------------------------------------------------------
void midasSynchronizer::CountBitstreams()
{
  if(SERVER_IS_MIDAS3 || DB_IS_MIDAS3)
    {
    return; //no count bitstreams behavior in midas 3 yet
    }

  if(this->Operation == midasSynchronizer::OPERATION_PULL
     && this->Recursive && !this->PathMode)
    {
    if(this->ResourceType == midasResourceType::BITSTREAM)
      {
      this->TotalBitstreams = 1;
      this->Progress->SetMaxCount(this->TotalBitstreams);
      this->Progress->SetMaxTotal(0);
      this->Progress->UpdateOverallCount(0);
      return;
      }
    std::string count;
    std::string size;

    this->Progress->SetIndeterminate();
    this->Log->Status("Counting total bitstreams under the object...");
    if(!mws::WebAPI::Instance()->CountBitstreams(this->ResourceType,
      atoi(this->ServerHandle.c_str()), count, size))
      {
      this->Log->Error("Failed to count bitstreams under the object");
      }
    this->Progress->ResetProgress();
    this->Log->Status("");

    this->TotalBitstreams = atoi(count.c_str());
    this->Progress->SetMaxCount(this->TotalBitstreams);
    this->Progress->SetMaxTotal(midasUtils::StringToDouble(size));
    this->Progress->UpdateOverallCount(0);
    }
  else if(this->Operation == midasSynchronizer::OPERATION_PUSH
          && this->ResourceType == midasResourceType::NONE)
    {
    int count = 0;
    double totalSize = 0;
    mds::DatabaseAPI db;
    std::vector<midasStatus> status = db.GetStatusEntries();

    for(std::vector<midasStatus>::iterator i = status.begin();
        i != status.end(); ++i)
      {
      if(i->GetType() == midasResourceType::BITSTREAM)
        {
        count++;
        mdo::Bitstream bitstream;
        bitstream.SetId(i->GetId());
        mds::Bitstream mdsBitstream;
        mdsBitstream.SetObject(&bitstream);
        mdsBitstream.Fetch();
        totalSize += midasUtils::StringToDouble(bitstream.GetSize());
        }
      }
    this->TotalBitstreams = count;
    this->Progress->SetMaxCount(count);
    this->Progress->SetMaxTotal(totalSize);
    this->Progress->UpdateOverallCount(0);
    }
  else if(this->Operation == midasSynchronizer::OPERATION_PUSH
          && this->ResourceType == midasResourceType::BITSTREAM)
    {
    mdo::Bitstream bitstream;
    bitstream.SetId(atoi(this->ClientHandle.c_str()));
    mds::Bitstream mdsBitstream;
    mdsBitstream.SetObject(&bitstream);
    mdsBitstream.Fetch();

    this->TotalBitstreams = 1;
    this->Progress->SetMaxCount(1);
    this->Progress->UpdateOverallCount(0);
    this->Progress->SetMaxTotal(midasUtils::StringToDouble(bitstream.GetSize()));
    }
  else if(this->Operation == midasSynchronizer::OPERATION_PUSH
          && this->ResourceType == midasResourceType::COMMUNITY
          && this->Recursive)
    {
    mdo::Community comm;
    comm.SetId(atoi(this->ClientHandle.c_str()));
    mds::Community mdsComm;
    mdsComm.SetObject(&comm);
    mdsComm.FetchTree();
    mdsComm.FetchSize();

    this->TotalBitstreams = comm.GetBitstreamCount();
    this->Progress->SetMaxCount(this->TotalBitstreams);
    this->Progress->UpdateOverallCount(0);
    this->Progress->SetMaxTotal(midasUtils::StringToDouble(comm.GetSize()));
    }
  else if(this->Operation == midasSynchronizer::OPERATION_PUSH
          && this->ResourceType == midasResourceType::COLLECTION
          && this->Recursive)
    {
    mdo::Collection coll;
    coll.SetId(atoi(this->ClientHandle.c_str()));
    mds::Collection mdsColl;
    mdsColl.SetObject(&coll);
    mdsColl.FetchTree();
    mdsColl.FetchSize();

    this->TotalBitstreams = coll.GetBitstreamCount();
    this->Progress->SetMaxCount(this->TotalBitstreams);
    this->Progress->UpdateOverallCount(0);
    this->Progress->SetMaxTotal(midasUtils::StringToDouble(coll.GetSize()));
    }
  else if(this->Operation == midasSynchronizer::OPERATION_PUSH
          && this->ResourceType == midasResourceType::ITEM
          && this->Recursive)
    {
    mdo::Item item;
    item.SetId(atoi(this->ClientHandle.c_str()));
    mds::Item mdsItem;
    mdsItem.SetObject(&item);
    mdsItem.FetchTree();
    mdsItem.FetchSize();

    this->TotalBitstreams = item.GetBitstreamCount();
    this->Progress->SetMaxCount(this->TotalBitstreams);
    this->Progress->UpdateOverallCount(0);
    this->Progress->SetMaxTotal(midasUtils::StringToDouble(item.GetSize()));
    }
}
