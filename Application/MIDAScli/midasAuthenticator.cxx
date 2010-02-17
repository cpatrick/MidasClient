/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "midasAuthenticator.h"
#include "mwsWebAPI.h"

midasAuthenticator::midasAuthenticator()
{
  this->Database = NULL;
  this->ClearToken();
}

midasAuthenticator::~midasAuthenticator()
{
  if(this->Database)
    {
    delete this->Database;
    }
}

//-------------------------------------------------------------------
bool midasAuthenticator::AddAuthProfile(std::string user, std::string appName,
      std::string apiKey, std::string profileName)
{
  this->Database->Open();
  mws::WebAPI remote;
  remote.SetServerUrl(this->ServerURL.c_str());
  if(!remote.Login(appName.c_str(), user.c_str(), apiKey.c_str()))
    {
    std::cerr << "Login credentials refused by server." << std::endl;
    return false;
    }
  bool success = 
    this->Database->AddAuthProfile(user, appName, apiKey, profileName);
  this->Database->Close();
  return success;
}

//-------------------------------------------------------------------
void midasAuthenticator::ClearToken()
{
  this->Token = "";
}

//-------------------------------------------------------------------
std::string midasAuthenticator::FetchToken(std::string profileName)
{
  if(this->Token != "")
    {
    mws::WebAPI remote;
    remote.SetServerUrl(this->ServerURL.c_str());
    std::string appName, email, apiKey;
    
    this->Database->Open();
    if(!this->Database->GetAuthProfile(profileName, email, appName, apiKey))
      {
      std::cerr << "No profile exists with that name. Use the "
        "\"create_profile\" command to add a profile." << std::endl;
      return "";
      }
    this->Database->Close();

    if(!remote.Login(appName.c_str(), email.c_str(), apiKey.c_str()))
      {
      std::cerr << "Login credentials refused by server." << std::endl;
      return "";
      }
    this->Token = remote.GetAPIToken();
    }
  return this->Token;
}

//-------------------------------------------------------------------
void midasAuthenticator::SetDatabaseProxy(midasDatabaseProxy* database)
{
  if(this->Database)
    {
    delete this->Database;
    }
  this->Database = database;
}

//-------------------------------------------------------------------
void midasAuthenticator::SetServerURL(std::string url)
{
  this->ServerURL = url;
}