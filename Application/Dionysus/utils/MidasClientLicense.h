/*=========================================================================

  Program:   Dionysus
  Module:    MidasClientLicense.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __MidasClientLicense_H
#define __MidasClientLicense_H

#include <stdio.h>
#include <iostream>
#include <string>

class DYSInit; 
class MidasClientComputerID; 

class MidasClientLicense
{
public:

  MidasClientLicense();
  ~MidasClientLicense();

  std::string GetEmail(){return m_Email;}
  std::string GetFirstName(){return m_FirstName;}
  std::string GetLastName(){return m_LastName;}
  std::string GetComputerID(){return m_ComputerID;}

  bool CanStart();
  bool NewUser();
  bool Checkdownloads();
  bool Checkuploads();
  bool NewDownload();
  bool NewUpload();
  bool HasFullLicense();

  int GetUploadLeft()
    {
    return m_UploadLeft;
    }

  int GetDownloadLeft()
    {
    return m_DownloadLeft;
    }

  int GetDaysRemaining()
    {
    return m_DaysRemaining;
    }

private:

  std::string        m_Email;
  std::string        m_FirstName;
  std::string        m_LastName;
  std::string        m_ComputerID;
  DYSInit *          m_InitFile;

  MidasClientComputerID * ID; 

  int                m_UploadLeft;
  int                m_DownloadLeft;
  int                m_DaysRemaining;
};

#endif //__MidasClientLicense_H
