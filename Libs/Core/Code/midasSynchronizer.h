/******************************************************************************
 * Copyright 2011 Kitware Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/


#ifndef MIDASSYNCHRONIZER_H
#define MIDASSYNCHRONIZER_H

#include "midasStandardIncludes.h"
#include "midasLogAware.h"
#include <QMutex>

struct midasResourceRecord;
class midasStatus;
class midasProgressReporter;
class midasFileOverwriteHandler;
class midasDatabaseProxy;
class midasAuthenticator;
class midasAgreementHandler;

namespace mdo
{
class Community;
class Collection;
class Item;
class Bitstream;
}

namespace m3do
{
class Folder;
class Item;
class Bitstream;
}

namespace mds
{
class ResourceUpdateHandler;
}

// The following define error codes returned by midas functions
#define MIDAS_OK                    0
#define MIDAS_INVALID_PATH         -1
#define MIDAS_BAD_FILE_TYPE        -2
#define MIDAS_DUPLICATE_PATH       -3
#define MIDAS_NO_URL               -4
#define MIDAS_LOGIN_FAILED         -5
#define MIDAS_BAD_OP               -6
#define MIDAS_WEB_API_FAILED       -7
#define MIDAS_NO_RTYPE             -8
#define MIDAS_FAILURE              -9
#define MIDAS_INVALID_PARENT      -10
#define MIDAS_EMPTY_FILE          -11
#define MIDAS_CANCELED            -12
#define MIDAS_INVALID_SERVER_PATH -13
#define MIDAS_INVALID_TYPE        -14

class midasSynchronizer : public midasLogAware
{
public:
  midasSynchronizer();
  ~midasSynchronizer();

  enum SynchOperation
    {
    OPERATION_NONE = 0,
    OPERATION_ADD,
    OPERATION_CLEAN,
    OPERATION_CLONE,
    OPERATION_PULL,
    OPERATION_PUSH,
    OPERATION_UPLOAD,
    OPERATION_RESUME_DOWNLOAD,
    OPERATION_RESUME_UPLOAD
    };

  void SetLog(midasLog* log);

  void SetParentId(int id);

  int GetParentId();

  void SetPathMode(bool val);
  bool IsPathMode();

  void SetOperation(SynchOperation op);

  SynchOperation GetOperation();

  void SetResourceType(int type);

  int GetResourceType();

  void SetResourceType3(int type);

  int GetResourceType3();

  void SetProgressReporter(midasProgressReporter* progress);

  midasProgressReporter * GetProgressReporter();

  void DeleteProgressReporter();

  void SetAgreementHandler(midasAgreementHandler* handler);

  void SetOverwriteHandler(midasFileOverwriteHandler* handler);

  midasFileOverwriteHandler * GetOverwriteHandler();

  void SetClientHandle(std::string handle);

  std::string GetClientHandle();

  void SetServerHandle(std::string handle);

  std::string GetServerHandle();

  /**
   * Use this to pass data for use in a specific function
   * that will know its type
   */
  void SetObject(void* object);

  void * GetObject();

  void SetRecursive(bool recursive);

  bool GetRecursive();

  void SetAuthenticator(midasAuthenticator* authenticator, bool deleteOld);

  midasAuthenticator * GetAuthenticator();

  int Perform();

  void Cancel();

protected:
  /* Converts a MIDAS server path into an ID and sets the appropriate type */
  bool ConvertPathToId();

  /**
   * Optionally pass a bitstream object that will be filled with the id/uuid
   * of the object that was just added.
   */
  int Add(mdo::Bitstream* result = NULL);

  int Add3();

  int Clean();

  int Clone();

  int Pull();

  int Pull3();

  int Push();

  int Push3();

  int Upload();

  bool ResumeDownload();

  bool ResumeUpload();

  bool PullBitstream(int parentId);

  bool PullCollection(int parentId);

  bool PullCommunity(int parentId);

  bool PullItem(int parentId);

  bool PullFolder3(m3do::Folder* parent);

  bool PullItem3(m3do::Folder* parent);

  bool PullBitstream3(m3do::Item* parent);

  /* Helper function to convert client side parent ID to server side one */
  int GetServerParentId(midasResourceType::ResourceType type, int parentId);

  int GetServerParentId3(const std::string& parentUuid);

  bool Push(mdo::Community *);

  bool Push(mdo::Collection *);

  bool Push(mdo::Item *);

  bool Push(mdo::Bitstream *);

  bool Push(m3do::Folder *);

  bool Push(m3do::Item *);

  bool Push(m3do::Bitstream *);

  int PushAllDirty();

  void RecurseCommunities(int parentId, mdo::Community* community);

  /* Reset the synchronizer to its starting state */
  void Reset();

  void CountBitstreams();

  void ChangeToRootDir();

  std::string ResolveAddPath();

  SynchOperation Operation;
  int            ResourceType;
  int            ResourceType3;
  int            LastId;
  void*          Object;
  std::string    ClientHandle;
  std::string    ServerHandle;
  std::string    LastDir;
  std::string    Uuid;
  int            CurrentBitstreams;
  int            TotalBitstreams;

  /* Pull entire subtree of resources that are pulled?*/
  bool Recursive;

  /* Does the provided resource ID specify a path on the MIDAS server? */
  bool PathMode;

  bool                       ShouldCancel;
  midasProgressReporter*     Progress;
  midasAuthenticator*        Authenticator;
  midasAgreementHandler*     AgreementHandler;
  midasFileOverwriteHandler* OverwriteHandler;
  QMutex*                    Mutex;
};

#endif
