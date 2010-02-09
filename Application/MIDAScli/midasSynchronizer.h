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
#include <mwsWebAPI.h>
#include <mdoCommunity.h>

class midasCLI;

class midasSynchronizer
{
  public:
    midasSynchronizer(midasCLI* cli);
    ~midasSynchronizer();

    enum SynchOperation {
      OPERATION_NONE = 0,
      OPERATION_CLEAN,
      OPERATION_CLONE,
      OPERATION_PULL,
      OPERATION_PUSH
      };

    enum ResourceType {
      TYPE_BITSTREAM,
      TYPE_ITEM,
      TYPE_COLLECTION,
      TYPE_COMMUNITY,
      TYPE_NONE
      };

    void SetOperation(SynchOperation op);
    SynchOperation GetOperation();

    void SetPullType(ResourceType type);
    ResourceType GetPullType();

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

    int Perform();
  protected:
    int Clone();
    int Push();
    int Pull();
    int Clean();

    std::string GetBitstreamName();
    std::string GetUUID(int type);
    int PullBitstream(std::string name);
    int PullCollection();
    int PullCommunity();
    int PullItem();

    void RecurseCommunities(mdo::Community* community);

    SynchOperation Operation;
    ResourceType PullType;
    std::string ServerURL;
    std::string ResourceHandle;
    
    // pull entire subtree of resources that are pulled?
    bool Recursive;
    midasCLI* CLI;
    mws::WebAPI WebAPI;
    midasProgressReporter* Progress;
    
    std::string Database;
    midasDatabaseProxy* DatabaseProxy;
};

#endif
