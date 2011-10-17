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


#ifndef MIDASCLI_H
#define MIDASCLI_H

#include "midasStandardIncludes.h"
#include "midasLogAware.h"

class midasSynchronizer;
struct midasAuthProfile;

class midasCLI : public midasLogAware
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

  bool ParseUpload(std::vector<std::string> args);

  int SetRootDir(std::vector<std::string> args);

  int PerformCreateProfile(std::vector<std::string> args);

  int PerformSetMetadata(std::vector<std::string> args);

  int PerformDelete(std::vector<std::string> args);

  int RunSynchronizer();

  std::string GetServerUrl();

  std::string        RootDir;
  std::string        Database;
  bool               UseTempProfile;
  midasAuthProfile*  TempProfile;
  midasSynchronizer* Synchronizer;
};

#endif
