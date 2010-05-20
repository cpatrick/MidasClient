/*=========================================================================

  Program:   Dionysus
  Module:    MidasClientLicense.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "MidasClientLicense.h"

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

#include "MidasClientComputerID.h"
#include "Utilities/curl/curl/curl.h"
#include "DYSInit.h"

std::string httpReplyLicense = "";

static size_t
outputFunctionLicense(char *buffer,
  size_t size,
  size_t nitems,
  void *userp)
{
  httpReplyLicense += buffer;

  size *= nitems;
  return size;
}

MidasClientLicense::MidasClientLicense()
{
  m_InitFile = DYSInit::instance();
  m_UploadLeft = 0;
  m_DownloadLeft = 0;
  m_DaysRemaining = 0;
}


MidasClientLicense::~MidasClientLicense()
{
  m_InitFile = NULL;
}

bool MidasClientLicense::CanStart()
{
  const char* login = "MidasClient";
  const char* password = "M1da5#1!";
  MidasClientComputerID ID;
  m_ComputerID = ID.GetComputerId().c_str();

  //std::cout << "Computer id is: " << m_ComputerID << std::endl;

  m_Email = m_InitFile->GetUserEmail().c_str();

  curl_global_init(CURL_GLOBAL_ALL);
  CURL* curl = curl_easy_init();
  if(!curl) 
    {
    std::cout << "Cannot Initialize Curl!" << std::endl;
    return false;
    }

  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, outputFunctionLicense);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);
  
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 1);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);

  curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);

  //Check Registration
  std::string url = "https://www.insight-journal.org/midas/client/license/checklicense.php";

  url += "?computerid=";
  url += m_ComputerID;
  url += "&email=";
  url += m_Email;

  curl_easy_setopt(curl, CURLOPT_URL,url.c_str());  

  // HTTP Authentication
  std::string loginpass = login;
  loginpass += ":";
  loginpass += password;
  curl_easy_setopt(curl, CURLOPT_USERPWD,loginpass.c_str());

  // Retrieve cookie session
  curl_easy_setopt(curl,CURLOPT_COOKIEJAR,"DionysusCookie.txt");
  curl_easy_setopt(curl, CURLOPT_POST, 0);

  httpReplyLicense = "";
  CURLcode res = curl_easy_perform(curl);

  if(res != CURLE_OK)
    {
    std::cout << "Curl perform failed in CanStart: " 
              << curl_easy_strerror(res) << std::endl;
    return false;
    }
  
  curl_easy_cleanup(curl);
  long int pos = httpReplyLicense.find("[SUCCESS]");
  if(pos != -1)
    {
    long int pos2 = httpReplyLicense.find("\n",pos);
    if(pos2 != -1)
      {
      m_DaysRemaining = atoi(httpReplyLicense.substr(pos+9,pos2-pos-9).c_str());
      }
    return true;
    }

  return false;
}

bool MidasClientLicense::NewUser()
{
  const char* login = "MidasClient";
  const char* password = "M1da5#1!";
  m_Email = m_InitFile->GetUserEmail().c_str();
  m_FirstName = m_InitFile->GetFirstName().c_str();
  m_LastName = m_InitFile->GetLastName().c_str();

  curl_global_init(CURL_GLOBAL_ALL);
  CURL* curl = curl_easy_init();
  if(!curl) 
    {
    std::cout << "Cannot Initialize Curl!" << std::endl;
    return false;
    }

  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, outputFunctionLicense);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);
  
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 1);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);

  curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);

  //New Registration
  std::string url = "https://www.insight-journal.org/midas/client/license/register.php";
  url += "?computerid=";
  url += m_ComputerID;
  url += "&firstname=";
  url += m_FirstName;
  url += "&lastname=";
  url += m_LastName;
  url += "&email=";
  url += m_Email;

  curl_easy_setopt(curl, CURLOPT_URL,url.c_str());  

  // HTTP Authentication
  std::string loginpass = login;
  loginpass += ":";
  loginpass += password;
  curl_easy_setopt(curl, CURLOPT_USERPWD,loginpass.c_str());

  // Retrieve cookie session
  curl_easy_setopt(curl,CURLOPT_COOKIEJAR,"DionysusCookie.txt");
  curl_easy_setopt(curl, CURLOPT_POST, 0);

  httpReplyLicense = "";
  CURLcode res = curl_easy_perform(curl);

  if(res != CURLE_OK)
    {
    std::cout << "Curl perform failed: " 
              << curl_easy_strerror(res) << std::endl;
    return false;
    }
  
  curl_easy_cleanup(curl);
 
  if(httpReplyLicense.find("[SUCCESS]")!=-1)
    {
    return true;
    }

  if(httpReplyLicense.find("[ALREADY]")!=-1)
    {
    return true;
    }
  
  return false;
}


bool MidasClientLicense::Checkdownloads()
{
  const char* login = "MidasClient";
  const char* password = "M1da5#1!";

  curl_global_init(CURL_GLOBAL_ALL);
  CURL* curl = curl_easy_init();
  if(!curl) 
    {
    std::cout << "Cannot Initialize Curl!" << std::endl;
    return false;
    }

  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, outputFunctionLicense);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);
  
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 1);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);

  curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);

  //New Registration
  std::string url = "https://www.insight-journal.org/midas/client/license/download.php";
  url += "?computerid=";
  url += m_ComputerID;
  url += "&email=";
  url += m_Email;

  curl_easy_setopt(curl, CURLOPT_URL,url.c_str());  

  // HTTP Authentication
  std::string loginpass = login;
  loginpass += ":";
  loginpass += password;
  curl_easy_setopt(curl, CURLOPT_USERPWD,loginpass.c_str());

  // Retrieve cookie session
  curl_easy_setopt(curl,CURLOPT_COOKIEJAR,"DionysusCookie.txt");
  curl_easy_setopt(curl, CURLOPT_POST, 0);

  httpReplyLicense = "";
  CURLcode res = curl_easy_perform(curl);

  if(res != CURLE_OK)
    {
    std::cout << "Curl perform failed: " 
              << curl_easy_strerror(res) << std::endl;
    return false;
    }
  
  int dl= 0;  
  long int success = httpReplyLicense.find("[SUCCESS]");
  if(success != -1 && httpReplyLicense.size() > 10)
    {
    std::string t = httpReplyLicense.substr(10,httpReplyLicense.size()-10);
    dl = atoi(t.c_str());
    }
  
  curl_easy_cleanup(curl);
  if(dl==0)
    {
    //std::cout << "No more download available" << std::endl;
    return false;
    }

  m_DownloadLeft = dl;

  //std::cout << "You can download " << dl << " files." << std::endl;
  return true;
}

bool MidasClientLicense::Checkuploads()
{
  const char* login = "MidasClient";
  const char* password = "M1da5#1!";

  curl_global_init(CURL_GLOBAL_ALL);
  CURL* curl = curl_easy_init();
  if(!curl) 
    {
    std::cout << "Cannot Initialize Curl!" << std::endl;
    return false;
    }

  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, outputFunctionLicense);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);
  
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 1);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);

  curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);

  std::string url = "https://www.insight-journal.org/midas/client/license/upload.php";
  url += "?computerid=";
  url += m_ComputerID;
  url += "&email=";
  url += m_Email;

  curl_easy_setopt(curl, CURLOPT_URL,url.c_str());  

  // HTTP Authentication
  std::string loginpass = login;
  loginpass += ":";
  loginpass += password;
  curl_easy_setopt(curl, CURLOPT_USERPWD,loginpass.c_str());

  // Retrieve cookie session
  curl_easy_setopt(curl,CURLOPT_COOKIEJAR,"DionysusCookie.txt");
  curl_easy_setopt(curl, CURLOPT_POST, 0);


  httpReplyLicense = "";
  CURLcode res = curl_easy_perform(curl);

  if(res != CURLE_OK)
    {
    std::cout << "Curl perform failed: " << 
      curl_easy_strerror(res) << std::endl;
    return false;
    }
  
  int ul= 0;  
  long int success = httpReplyLicense.find("[SUCCESS]");
  if(success != -1 && httpReplyLicense.size() > 10)
    {
    std::string t = httpReplyLicense.substr(10,httpReplyLicense.size()-10);
    ul = atoi(t.c_str());
    }
  
  curl_easy_cleanup(curl);

  if(ul==0)
    {
    //std::cout<<"No more upload available"<<std::endl;
    return false;
    }

  //std::cout<<"You can upload "<<ul<<" files."<<std::endl;

  m_UploadLeft = ul;

  return true;
}


bool MidasClientLicense::NewUpload()
{
  const char* login = "MidasClient";
  const char* password = "M1da5#1!";

  curl_global_init(CURL_GLOBAL_ALL);
  CURL* curl = curl_easy_init();
  if(!curl) 
    {
    std::cout << "Cannot Initialize Curl!" << std::endl;
    return false;
    }

  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, outputFunctionLicense);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);
  
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 1);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);

  curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);

  std::string url = "https://www.insight-journal.org/midas/client/license/upload.php";
  url += "?computerid=";
  url += m_ComputerID;
  url += "&email=";
  url += m_Email;
  url += "&increase=1"; 

  curl_easy_setopt(curl, CURLOPT_URL,url.c_str());  

  // HTTP Authentication
  std::string loginpass = login;
  loginpass += ":";
  loginpass += password;
  curl_easy_setopt(curl, CURLOPT_USERPWD,loginpass.c_str());

  // Retrieve cookie session
  curl_easy_setopt(curl,CURLOPT_COOKIEJAR,"DionysusCookie.txt");
  curl_easy_setopt(curl, CURLOPT_POST, 0);

  httpReplyLicense = "";
  CURLcode res = curl_easy_perform(curl);

  if(res != CURLE_OK)
    {
    std::cout << "Curl perform failed: " << 
      curl_easy_strerror(res) << std::endl;
    return false;
    }
  return true;
}

bool MidasClientLicense::NewDownload()
{
  const char* login = "MidasClient";
  const char* password = "M1da5#1!";

  curl_global_init(CURL_GLOBAL_ALL);
  CURL* curl = curl_easy_init();
  if(!curl) 
    {
    std::cout << "Cannot Initialize Curl!" << std::endl;
    return false;
    }

  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, outputFunctionLicense);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);
  
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 1);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);

  curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);

  //New Registration
  std::string url = "https://www.insight-journal.org/midas/client/license/download.php";
  url += "?computerid=";
  url += m_ComputerID;
  url += "&email=";
  url += m_Email;
  url += "&increase=1"; 

  curl_easy_setopt(curl, CURLOPT_URL,url.c_str());  

  // HTTP Authentication
  std::string loginpass = login;
  loginpass += ":";
  loginpass += password;
  curl_easy_setopt(curl, CURLOPT_USERPWD,loginpass.c_str());

  // Retrieve cookie session
  curl_easy_setopt(curl,CURLOPT_COOKIEJAR,"DionysusCookie.txt");
  curl_easy_setopt(curl, CURLOPT_POST, 0);

  httpReplyLicense = "";
  CURLcode res = curl_easy_perform(curl);

  if(res != CURLE_OK)
    {
    std::cout << "Curl perform failed: " << 
    curl_easy_strerror(res) << std::endl;
    return false;
    }

  return true;
}

bool MidasClientLicense::HasFullLicense()
{
  const char* login = "MidasClient";
  const char* password = "M1da5#1!";
  MidasClientComputerID ID;
  m_ComputerID=ID.GetComputerId().c_str();

  m_Email = m_InitFile->GetUserEmail().c_str();

  curl_global_init(CURL_GLOBAL_ALL);
  CURL* curl = curl_easy_init();
  if(!curl) 
    {
    std::cout << "Cannot Initialize Curl!" << std::endl;
    return false;
    }

  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, outputFunctionLicense);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);
  
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 1);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);

  curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);

  //Check Registration
  std::string url = "https://www.insight-journal.org/midas/client/license/checklicense.php";

  url += "?computerid=";
  url += m_ComputerID;
  url += "&email=";
  url += m_Email;
  url += "&checkfulllicense=1";

  curl_easy_setopt(curl, CURLOPT_URL,url.c_str());  

  // HTTP Authentication
  std::string loginpass = login;
  loginpass += ":";
  loginpass += password;
  curl_easy_setopt(curl, CURLOPT_USERPWD,loginpass.c_str());

  // Retrieve cookie session
  curl_easy_setopt(curl,CURLOPT_COOKIEJAR,"DionysusCookie.txt");
  curl_easy_setopt(curl, CURLOPT_POST, 0);

  httpReplyLicense = "";
  CURLcode res = curl_easy_perform(curl);

  if(res != CURLE_OK)
    {
    std::cout << "Curl perform failed: " 
              << curl_easy_strerror(res) << std::endl;
    return false;
    }
  
  curl_easy_cleanup(curl);
  if(httpReplyLicense.find("[SUCCESS]")!=-1)
    {
    return true;
    }

  return false;
}
