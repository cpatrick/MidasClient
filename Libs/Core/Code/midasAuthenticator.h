/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

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

  bool AddAuthProfile(const std::string& user, const std::string& appName, const std::string& password,
                      const std::string& rootDir, const std::string& profileName,
                      bool isKey = false);

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
