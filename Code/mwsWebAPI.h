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

class midasAuthenticator;

namespace mws
{

class RestXMLParser;

// This class provide the functionalities needed to search, download and upload files from/to a
// MIDAS server.
class WebAPI
{
public:
  // Singleton to be used in an application
  static WebAPI* Instance();
  
  // Set the base url
  void SetServerUrl(const char* baseurl);
  const char* GetServerUrl();
  
  // Execute the command
  bool Execute(const char* url);
  void SetPostData(const char* postData);
  
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
  
  // Upload a file
  bool UploadFile(const char* url, const char* filename);

  // Set the authenticator
  void SetAuthenticator(midasAuthenticator* auth);

  // Get the authenticator
  midasAuthenticator* GetAuthenticator();
  
  // After calling Login, use this to get the API token
  std::string GetAPIToken();

  // Login to MIDAS
  bool Login(const char* applicationname,
             const char* email,
             const char* apikey);

  // Return the REST API
  RestAPI* GetRestAPI() {return m_RestAPI;}
 
  // Check the connection to the MIDAS server
  bool CheckConnection();
 
protected:

  RestAPI*            m_RestAPI;
  RestXMLParser*      m_RestXMLParser;
  midasAuthenticator* m_Auth;
  std::string         m_APIToken;
  const char*         m_PostData;

  // constructor
  WebAPI();
  ~WebAPI();
private:
  static WebAPI * m_Instance; 

};

} // end namespace

#endif //__mwsRestAPI_H
