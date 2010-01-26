#ifndef MIDASCLI_H
#define MIDASCLI_H

#include <kwsys/SystemTools.hxx>
#include "midasSynchronizer.h"

class midasCLI
{
  public:
    midasCLI();
    ~midasCLI();

    int Perform(std::vector<std::string> args);

    void SetDatabaseLocation(std::string location);
    std::string GetDatabaseLocation();
  protected:
    void PrintUsage();
    void PrintCommandHelp(std::string command);
  
    //the path to the local database file
    std::string DatabaseLocation;

    //the synchronizer object which performs the action
    midasSynchronizer* Synchronizer;
};

#endif
