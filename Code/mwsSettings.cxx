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

/** Load the setting from the SQLite database */
bool Settings::Load(const char* filename)
{
  return true;
}

/** Write the settings to the SQLite database */
bool Settings::Write(const char* filename)
{
  return true;
}

} // end namespace
