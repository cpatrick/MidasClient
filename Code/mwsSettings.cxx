/*=========================================================================

  Program:   Dionysus
  Module:    mwsSettings.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mwsSettings.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "mdsSQLiteDatabase.h"
  
namespace mws {  
  
Settings* Settings::m_Instance = NULL; 

/** Constructor */
Settings::Settings()
{
  m_ReadUploadAgreement = false;
  m_SearchInOptions = SEARCHIN_NOTSET;
  m_SearchOptionsVisible = false;
}

/** Destructor */
Settings::~Settings()
{
} 

Settings * Settings::GetInstance()
{
  if (Settings::m_Instance == NULL)
    {
    Settings::m_Instance = new Settings; 
    }
  return Settings::m_Instance; 
}

void Settings::SetPassword(const char* password)
{
  m_Password = password;
}

std::string Settings::GetPassword()
{ 
  return m_Password;
}

/** Load the setting from the SQLite database */
bool Settings::Load(const char* filename)
{
  mds::SQLiteDatabase database;
  if(!database.Open(filename))
    {
    std::cerr << database.GetErrorMessage() << std::endl;
    return false;
    }

  if(!database.SetQuery("SELECT * FROM config"))
    {
    std::cerr << database.GetErrorMessage() << std::endl;
    return false;
    }
    
  if(!database.Execute())
    {
    std::cerr << database.GetErrorMessage() << std::endl;
    return false;
    }
  
  if(database.GetNextRow())
    {
    m_ServerURL = database.GetValueAsString(0);
    m_Email = database.GetValueAsString(1);
    m_Password = database.GetValueAsString(2);
    }
    
  if(!database.Close())
    {
    std::cerr << database.GetErrorMessage() << std::endl;
    return false;
    }
  return true;
}

/** Write the settings to the SQLite database */
bool Settings::Write(const char* filename)
{
  mds::SQLiteDatabase database;
  if(!database.Open("midasdesktop.db"))
    {
    std::cerr << database.GetErrorMessage() << std::endl;
    return false;
    }

  if(!database.ExecuteQuery("SELECT count(*) FROM config"))
    {
    std::cerr << database.GetErrorMessage() << std::endl;
    return false;
    }
 
  if(database.GetNextRow())
    {
    int numrows = database.GetValueAsInt(0);
    if(numrows == 0)
      {
      std::stringstream query;
      query << "INSERT INTO config (url,email,password) VALUES ('";
      query << m_ServerURL.c_str() << "','";
      query << m_Email.c_str() << "','";
      query << m_Password.c_str() << "')";
      if(!database.ExecuteQuery(query.str().c_str()))
        {
        std::cerr << database.GetErrorMessage() << std::endl;
        }
      }
    else
      {
      std::stringstream query;
      query << "UPDATE config SET url='";
      query << m_ServerURL.c_str() << "',email='";
      query << m_Email.c_str() << "',password='";
      query << m_Password.c_str() << "'";
      if(!database.ExecuteQuery(query.str().c_str()))
        {
        std::cerr << database.GetErrorMessage() << std::endl;
        }
      }   
    }
    
  if(!database.Close())
    {
    std::cerr << database.GetErrorMessage() << std::endl;
    return false;
    }
  return true;
}

} // end namespace