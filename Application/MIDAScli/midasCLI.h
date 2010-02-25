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

    bool ParseAdd(std::vector<std::string> args);
    bool ParseClean(std::vector<std::string> args);
    bool ParseClone(std::vector<std::string> args);
    bool ParsePull(std::vector<std::string> args);
    bool ParsePush(std::vector<std::string> args);
    bool ParseStatus(std::vector<std::string> args);

    int PerformCreateProfile(std::vector<std::string> args);

    midasSynchronizer* Synchronizer;
};

#endif
