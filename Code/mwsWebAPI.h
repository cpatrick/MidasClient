/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __mwsWebAPI_H
#define __mwsWebAPI_H

#include <fstream>
#include <string>
#include <exception>
#include <sstream>

#include "mwsRestAPI.h"

namespace mws
{

class RestXMLParser;

// This class provide the functionalities needed to search, download and upload files from/to a
// MIDAS server.
class WebAPI
{
public:
     
  // constructor
  WebAPI();
  ~WebAPI();
  
  // Singleton to be used in an application
  static WebAPI* Instance();
  
  // Set the base url
  void SetServerUrl(const char* baseurl);
  const char* GetServerUrl();
  
  // Execute the command
  bool Execute(const char* url);
  
  // Return the last error code
  int GetErrorCode();
  
  // Return the last error message
  const char* GetErrorMessage();
  
  // Set verbose mode
  void SetVerbose(bool verbose);
  
  // Get the default rest XML parser
  RestXMLParser* GetRestXMLParser();
  
  // Download a file
  bool DownloadFile(const char* url, const char* filename);
  
  // Upload a file and return the bitstream id
  unsigned int UploadFile(const char* url, const char* filename);
  
  // Login to MIDAS
  bool Login(const char* applicationname,
             const char* email, 
             const char* apikey);
             
  // Login to MIDAS with password (not recommended)
  // Password should be encrypted
  bool LoginWithPassword(const char* applicationname,
                         const char* email, 
                         const char* password);

  // Return the REST API
  RestAPI* GetRestAPI() {return m_RestAPI;}
 
  // Check the connection to the MIDAS server
  bool CheckConnection();
 
protected:

  RestAPI*        m_RestAPI;
  RestXMLParser*  m_RestXMLParser;
  std::string     m_APIToken; // used when logged in to authenticate with the server

private:

  static WebAPI * m_Instance; 

};

} // end namespace

#endif //__mwsRestAPI_H
