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


#include "midasAuthenticator.h"
#include "mwsWebAPI.h"
#include "mwsRestResponseParser.h"
#include "mdsDatabaseAPI.h"
#include "midasStdOutLog.h"
#include "mdoVersion.h"

#include <QFileInfo>

midasAuthenticator::midasAuthenticator()
{
  this->ClearToken();
  this->Profile = "";
  this->Log = NULL;
}

midasAuthenticator::~midasAuthenticator()
{
}

// -------------------------------------------------------------------
bool midasAuthenticator::Login()
{
  mds::DatabaseAPI db;
  midasAuthProfile profile = db.GetAuthProfile(this->Profile);

  if( profile.IsAnonymous() )
    {
    return true;
    }

  if( profile.Name == "" )
    {
    std::stringstream text;
    text << "No profile exists with that name. Use the "
    "\"create_profile\" command to add a profile." << std::endl;
    Log->Error(text.str() );
    return false;
    }

  return mws::WebAPI::Instance()->Login(profile.AppName.c_str(),
                                        profile.User.c_str(), profile.ApiKey.c_str() );
}

// -------------------------------------------------------------------
bool midasAuthenticator::IsAnonymous()
{
  mds::DatabaseAPI db;
  midasAuthProfile profile = db.GetAuthProfile(this->Profile);

  return profile.IsAnonymous();
}

// -------------------------------------------------------------------
bool midasAuthenticator::AddAuthProfile(const std::string& user,
                                        const std::string& appName,
                                        const std::string& password,
                                        const std::string& rootDir,
                                        const std::string& profileName,
                                        bool isKey)
{
  QFileInfo fileInfo(rootDir.c_str() );

  if( rootDir != "" && !fileInfo.isDir() )
    {
    std::stringstream text;
    text << "Error: invalid root directory: " << rootDir << std::endl;
    Log->Error(text.str() );
    return false;
    }

  std::stringstream text;
  if( !mws::WebAPI::Instance()->CheckConnection() )
    {
    text << mws::WebAPI::Instance()->GetServerUrl()
         << " is not a valid MIDAS Rest API URL";
    Log->Error(text.str() );
    return false;
    }

  std::string apiKey;

  if( user != "" )
    {
    if( isKey )
      {
      apiKey = password;
      }
    else if( SERVER_IS_MIDAS3 )
      {
      if( !mws::WebAPI::Instance()->GetDefaultAPIKey(user, password, apiKey) )
        {
        std::stringstream text;
        text << "User credentials refused by server." << std::endl;
        Log->Error(text.str() );
        return false;
        }
      }
    else
      {
      apiKey = midasUtils::CreateDefaultAPIKey(user, password, "Default");
      }

    if( !mws::WebAPI::Instance()->Login(appName.c_str(), user.c_str(), apiKey.c_str() ) )
      {
      std::stringstream text;
      text << "Login credentials refused by server." << std::endl;
      Log->Error(text.str() );
      return false;
      }
    }
  mds::DatabaseAPI db;
  bool             success = db.AddAuthProfile(user, appName, apiKey, profileName, rootDir,
                                               mws::WebAPI::Instance()->GetServerUrl() );

  if( !success )
    {
    std::stringstream text;
    text << "Failed when inserting auth profile into database. "
         << "Name may already exist." << std::endl;
    this->Log->Error(text.str() );
    }
  return success;
}

// -------------------------------------------------------------------
void midasAuthenticator::ClearToken()
{
  this->Token = "";
}

// -------------------------------------------------------------------
void midasAuthenticator::SetProfile(const std::string& profile)
{
  this->Profile = profile;
  this->ClearToken();
}

// -------------------------------------------------------------------
std::string midasAuthenticator::GetProfile()
{
  return this->Profile;
}

