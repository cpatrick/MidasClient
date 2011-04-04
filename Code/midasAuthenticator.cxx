/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "midasAuthenticator.h"
#include "mwsWebAPI.h"
#include "mwsRestXMLParser.h"
#include "midasStdOutLog.h"

midasAuthenticator::midasAuthenticator()
{
  this->ClearToken();
  this->Profile = "";
  this->Log = NULL;
}

midasAuthenticator::~midasAuthenticator()
{
}

//-------------------------------------------------------------------
bool midasAuthenticator::Login()
{
  midasAuthProfile profile =
    mds::DatabaseAPI::Instance()->GetAuthProfile(this->Profile);
  
  if(profile.IsAnonymous())
    {
    return true;
    }

  if(profile.Name == "")
    {
    std::stringstream text;
    text << "No profile exists with that name. Use the "
      "\"create_profile\" command to add a profile." << std::endl;
    Log->Error(text.str());
    return false;
    }
  mws::RestXMLParser parser;
  mws::WebAPI::Instance()->GetRestAPI()->SetXMLParser(&parser);
  return mws::WebAPI::Instance()->Login(profile.AppName.c_str(), 
    profile.User.c_str(), profile.ApiKey.c_str());
}

//-------------------------------------------------------------------
bool midasAuthenticator::IsAnonymous()
{
  midasAuthProfile profile = mds::DatabaseAPI::Instance()->GetAuthProfile(this->Profile);
  return profile.IsAnonymous();
}

//-------------------------------------------------------------------
bool midasAuthenticator::AddAuthProfile(std::string user,
                                        std::string appName,
                                        std::string apiKey,
                                        std::string rootDir,
                                        std::string profileName)
{
  if(rootDir != "" && !kwsys::SystemTools::FileIsDirectory(rootDir.c_str()))
    {
    std::stringstream text;
    text << "Error: invalid root directory: " << rootDir << std::endl;
    Log->Error(text.str());
    return false;
    }

  mws::RestXMLParser parser;
  mws::WebAPI::Instance()->GetRestAPI()->SetXMLParser(&parser);

  if(user == "")
    {
    std::stringstream text;
    if(!mws::WebAPI::Instance()->CheckConnection())
      {
      text << mws::WebAPI::Instance()->GetServerUrl()
           << " is not a valid MIDAS Rest API URL";
      Log->Error(text.str());
      return false;
      }
    }
  else if(!mws::WebAPI::Instance()->Login(appName.c_str(), user.c_str(), apiKey.c_str()))
    {
    std::stringstream text;
    text << "Login credentials refused by server." << std::endl;
    Log->Error(text.str());
    return false;
    }
  bool success = mds::DatabaseAPI::Instance()->AddAuthProfile(
    user, appName, apiKey, profileName, rootDir,
    mws::WebAPI::Instance()->GetServerUrl());

  if(!success)
    {
    std::stringstream text;
    text << "Failed when inserting auth profile into database. "
      << "Name may already exist." << std::endl;
    this->Log->Error(text.str());
    }
  return success;
}

//-------------------------------------------------------------------
void midasAuthenticator::ClearToken()
{
  this->Token = "";
}

//-------------------------------------------------------------------
void midasAuthenticator::SetProfile(std::string profile)
{
  this->Profile = profile;
  this->ClearToken();
}
