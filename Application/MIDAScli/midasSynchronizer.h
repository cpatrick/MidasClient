#ifndef MIDASSYNCHRONIZER_H
#define MIDASSYNCHRONIZER_H

#include "midasStandardIncludes.h"
#include <mwsWebAPI.h>
#include <mdoCommunity.h>

// For resource_uuid.resource_type_id
#define MIDAS_RESOURCE_BITSTREAM  0
#define MIDAS_RESOURCE_BUNDLE     1
#define MIDAS_RESOURCE_ITEM       2
#define MIDAS_RESOURCE_COLLECTION 3
#define MIDAS_RESOURCE_COMMUNITY  4
#define MIDAS_RESOURCE_PROJECT    5

class midasCLI;

class midasSynchronizer
{
  public:
    midasSynchronizer(midasCLI* cli);
    ~midasSynchronizer();

    enum SynchOperation {
      OPERATION_NONE = 0,
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

    void SetServerURL(std::string url);
    std::string GetServerURL();

    void SetResourceHandle(std::string handle);
    std::string GetResourceHandle();

    void SetRecursive(bool recursive);
    bool GetRecursive();

    int Perform();
  protected:
    int Clone();
    int Push();
    int Pull();

    std::string GetBitstreamName();
    int PullBitstream(std::string name);
    int PullCollection();
    int PullCommunity();
    int PullItem();

    void RecurseCommunities(mdo::Community* community);

    SynchOperation Operation;
    ResourceType PullType;
    std::string ServerURL;
    std::string ResourceHandle;
    
    // pull entire subtree of items that are pulled?
    bool Recursive;
    midasCLI* CLI;
    mws::WebAPI WebAPI;
};

#endif
