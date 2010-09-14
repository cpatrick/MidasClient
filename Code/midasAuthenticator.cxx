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
  this->Database = NULL;
  this->ClearToken();
  this->Profile = "";
  this->Log = NULL;
}

midasAuthenticator::~midasAuthenticator()
{
  delete this->Database;
}

//-------------------------------------------------------------------
bool midasAuthenticator::Login(mws::WebAPI* api)
{
  midasAuthProfile profile = this->Database->GetAuthProfile(this->Profile);
  
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
  api->GetRestAPI()->SetXMLParser(&parser);
  return api->Login(profile.AppName.c_str(), profile.User.c_str(),
                    profile.ApiKey.c_str());
}

//-------------------------------------------------------------------
bool midasAuthenticator::IsAnonymous()
{
  midasAuthProfile profile = this->Database->GetAuthProfile(this->Profile);
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
  mws::WebAPI* remote = mws::WebAPI::Instance();
  if(this->ServerURL == "")
    {
    this->ServerURL = this->Database->GetSetting(midasDatabaseProxy::LAST_URL);
    }
  remote->SetServerUrl(this->ServerURL.c_str());
  remote->GetRestAPI()->SetXMLParser(&parser);

  if(user == "")
    {
    mws::WebAPI::Instance()->SetServerUrl(this->ServerURL.c_str());
    std::stringstream text;
    if(!mws::WebAPI::Instance()->CheckConnection())
      {
      text << this->ServerURL << " is not a valid MIDAS Rest API URL";
      Log->Error(text.str());
      return false;
      }
    }
  else if(!remote->Login(appName.c_str(), user.c_str(), apiKey.c_str()))
    {
    std::stringstream text;
    text << "Login credentials refused by server." << std::endl;
    Log->Error(text.str());
    return false;
    }
  bool success = this->Database->AddAuthProfile(
    user, appName, apiKey, profileName, rootDir, this->ServerURL);

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
/*std::string midasAuthenticator::FetchToken()
{
  if(this->Token != "")
    {
    mws::RestXMLParser parser;
    mws::WebAPI* remote = mws::WebAPI::Instance();
    remote->SetServerUrl(this->ServerURL.c_str());
    remote->GetRestAPI()->SetXMLParser(&parser);
    
    midasAuthProfile profile = this->Database->GetAuthProfile(this->Profile);
    if(profile.Name == "")
      {
      std::stringstream text;
      text << "No profile exists with that name. Use the "
        "\"create_profile\" command to add a profile." << std::endl;
      Log->Error(text.str());
      return "";
      }

    if(!remote->Login(profile.AppName.c_str(), profile.User.c_str(),
                      profile.ApiKey.c_str()))
      {
      std::stringstream text;
      text << "Login credentials refused by server."
        << std::endl;
      Log->Error(text.str());
      return "";
      }
    this->Token = remote->GetAPIToken();
    }
  return this->Token;
}*/

//-------------------------------------------------------------------
void midasAuthenticator::SetDatabase(std::string database)
{
  delete this->Database;
  this->Database = new midasDatabaseProxy(database);
}

//-------------------------------------------------------------------
void midasAuthenticator::SetServerURL(std::string url)
{
  this->ServerURL = url;
}

//-------------------------------------------------------------------
void midasAuthenticator::SetProfile(std::string profile)
{
  this->Profile = profile;
  this->ClearToken();
}
