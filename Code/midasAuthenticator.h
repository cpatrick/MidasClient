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
#include "mdsDatabaseAPI.h"
#include "midasLogAware.h"
#include "mwsWebAPI.h"

class midasAuthenticator : public midasLogAware
{
  public:
    midasAuthenticator();
    ~midasAuthenticator();

    bool AddAuthProfile(std::string user, std::string appName,
      std::string apiKey, std::string rootDir, std::string profileName);
    
    void ClearToken();

    void SetProfile(std::string profile);
    std::string GetProfile() { return Profile; }

    bool Login();

    bool IsAnonymous();
  protected:
    std::string Token;
    std::string Profile;
};

#endif
