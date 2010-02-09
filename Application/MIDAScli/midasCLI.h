/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef MIDASCLI_H
#define MIDASCLI_H

#include "midasSynchronizer.h"

class midasCLI
{
  public:
    midasCLI();
    ~midasCLI();

    int Perform(std::vector<std::string> args);
  protected:
    void PrintUsage();
    void PrintCommandHelp(std::string command);

    void ParseClean(std::vector<std::string> args);
    void ParsePull(std::vector<std::string> args);
    void ParsePush(std::vector<std::string> args);
    void ParseClone(std::vector<std::string> args);

    //the synchronizer object which performs the action
    midasSynchronizer* Synchronizer;
};

#endif
