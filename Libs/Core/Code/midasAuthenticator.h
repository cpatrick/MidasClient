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


#ifndef MIDASAUTHENTICATOR_H
#define MIDASAUTHENTICATOR_H

#include "midasStandardIncludes.h"
#include "midasLogAware.h"
#include "mwsWebAPI.h"

class midasAuthenticator : public midasLogAware
{
public:
  midasAuthenticator();
  ~midasAuthenticator();

  bool AddAuthProfile(const std::string& user, const std::string& appName,
                      const std::string& password, const std::string& rootDir,
                      const std::string& profileName, bool isKey = false);

  void ClearToken();

  void SetProfile(const std::string& profile);

  std::string GetProfile();

  bool Login();

  bool IsAnonymous();

protected:
  std::string Token;
  std::string Profile;
};

#endif
