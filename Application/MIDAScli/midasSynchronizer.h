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
#include <mwsWebAPI.h>
#include <mdoCommunity.h>

class midasSynchronizer
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

    void SetOperation(SynchOperation op);
    SynchOperation GetOperation();

    void SetResourceType(int type);
    int GetResourceType();

    void SetDatabase(std::string path);
    std::string GetDatabase();

    void SetServerURL(std::string url);
    std::string GetServerURL();

    void SetProgressReporter(midasProgressReporter* progress);
    midasProgressReporter* GetProgressReporter();
    void DeleteProgressReporter();

    void SetResourceHandle(std::string handle);
    std::string GetResourceHandle();

    void SetRecursive(bool recursive);
    bool GetRecursive();

    midasAuthenticator* GetAuthenticator();

    int Perform();
  protected:
    int Add();
    int Clone();
    int Push();
    int Pull();
    int Clean();

    std::string GetBitstreamName();
    std::string GetUUID(int type);
    bool PullBitstream(int parentId, std::string name);
    bool PullCollection(int parentId);
    bool PullCommunity(int parentId);
    bool PullItem(int parentId);

    // Helper function to convert client side parent ID to server side one
    int GetServerParentId(midasResourceType::ResourceType type, int parentId);
    bool PushBitstream(int id);
    bool PushCollection(int id);
    bool PushCommunity(int id);
    bool PushItem(int id);

    void RecurseCommunities(int parentId, mdo::Community* community);

    SynchOperation Operation;
    int ResourceType;
    std::string ServerURL;
    std::string ResourceHandle;
    
    // pull entire subtree of resources that are pulled?
    bool Recursive;
    mws::WebAPI* WebAPI;
    midasProgressReporter* Progress;
    
    std::string Database;
    midasDatabaseProxy* DatabaseProxy;
    midasAuthenticator* Authenticator;
};

#endif
