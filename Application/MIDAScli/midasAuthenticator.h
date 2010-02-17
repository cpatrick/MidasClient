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
#include "midasDatabaseProxy.h"

class midasAuthenticator
{
  public:
    midasAuthenticator();
    ~midasAuthenticator();

    bool AddAuthProfile(std::string user, std::string appName,
      std::string apiKey, std::string profileName);
    
    // Lazy getter for an api auth token
    std::string FetchToken(std::string profileName);
    // May need this function in case the old token expires
    void ClearToken();

    void SetDatabase(std::string database);
    void SetServerURL(std::string url);
  protected:
    std::string ServerURL;
    midasDatabaseProxy* Database;
    std::string Token;
};

#endif