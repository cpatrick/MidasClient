/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    mcLicenseTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include <iostream>
#include "MidasClientComputerID.h"
#include "Utilities/curl/curl/curl.h"

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

int main(int argc, char **argv)
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
  curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);

  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);

  curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);

  //Check Registration
  std::string url = "https://www.insight-journal.org/midas/client/license/checklicense.php";
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
 
  std::cout << "HTTP Reply = " << std::endl;
  std::cout << httpReplyLicense.c_str() << std::endl;

  return 1;
}
