/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef MIDASSYNCHRONIZER_H
#define MIDASSYNCHRONIZER_H

#include "midasStandardIncludes.h"
#include "midasProgressReporter.h"
#include "midasDatabaseProxy.h"
#include "midasAuthenticator.h"
#include "midasStatus.h"
#include "midasLogAware.h"
#include "mwsWebAPI.h"
#include "mdoCommunity.h"

//The following define error codes returned by midas functions
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

class midasSynchronizer : public midasLogAware
{
public:
  midasSynchronizer();
  ~midasSynchronizer();

  enum SynchOperation {
    OPERATION_NONE = 0,
    OPERATION_ADD,
    OPERATION_CLEAN,
    OPERATION_CLONE,
    OPERATION_PULL,
    OPERATION_PUSH
    };

  void SetLog(midasLog* log);

  void SetParentId(int id);
  int GetParentId();

  void SetPathMode(bool val) { this->PathMode = val; }
  bool IsPathMode() { return this->PathMode; }

  void SetOperation(SynchOperation op);
  SynchOperation GetOperation();

  void SetResourceType(int type);
  int GetResourceType();

  void SetDatabase(std::string path);
  midasDatabaseProxy* GetDatabase();

  void SetServerURL(std::string url);
  std::string GetServerURL();

  void SetProgressReporter(midasProgressReporter* progress);
  midasProgressReporter* GetProgressReporter();
  void DeleteProgressReporter();

  void SetResourceHandle(std::string handle);
  std::string GetResourceHandle();

  void SetRecursive(bool recursive);
  bool GetRecursive();

  std::vector<midasStatus> GetStatusEntries();

  midasAuthenticator* GetAuthenticator();

  int Perform();

  void Cancel();

protected:
  /* Converts a MIDAS server path into an ID and sets the appropriate type */
  bool ConvertPathToId();

  int Add();
  int Clone();
  int Push();
  int Pull();
  int Clean();

  bool PullBitstream(int parentId);
  bool PullCollection(int parentId);
  bool PullCommunity(int parentId);
  bool PullItem(int parentId);

  /* Helper function to convert client side parent ID to server side one */
  int GetServerParentId(midasResourceType::ResourceType type, int parentId);
  bool PushBitstream(int id);
  bool PushCollection(int id);
  bool PushCommunity(int id);
  bool PushItem(int id);

  void RecurseCommunities(int parentId, mdo::Community* community);

  bool ValidateParentId(int parentId, midasResourceType::ResourceType type);

  /* Reset the synchronizer to its starting state */
  void Reset();

  SynchOperation Operation;
  int ResourceType;
  int ParentId;
  int LastId;
  std::string ServerURL;
  std::string ResourceHandle;
  std::string LastDir;

  /* Pull entire subtree of resources that are pulled?*/
  bool Recursive;

  /* Does the provided resource ID specify a path on the MIDAS server? */
  bool PathMode;

  bool ShouldCancel;
  midasProgressReporter* Progress;

  std::string Database;
  midasDatabaseProxy* DatabaseProxy;
  midasAuthenticator* Authenticator;
};

#endif
