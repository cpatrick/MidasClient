#ifndef MIDASSYNCHRONIZER_H
#define MIDASSYNCHRONIZER_H

#include "midasStandardIncludes.h"

class midasCLI;

class midasSynchronizer
{
  public:
    midasSynchronizer(midasCLI* cli);
    ~midasSynchronizer();

    enum SynchOperation {
      OPERATION_NONE = 0,
      OPERATION_CLONE,
      OPERATION_PUSH,
      OPERATION_PULL
      };

    void SetOperation(SynchOperation op);
    SynchOperation GetOperation();

    void SetServerURL(std::string url);
    std::string GetServerURL();

    void SetResourceId(std::string id);
    std::string GetResourceId();

    int Perform();
  protected:
    SynchOperation Operation;
    std::string ServerURL;
    std::string ResourceId;
    midasCLI* CLI;
};

#endif
