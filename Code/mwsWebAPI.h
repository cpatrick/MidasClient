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
#include <QMutex>

#include "mwsRestAPI.h"
#include "midasLogAware.h"

class midasAuthenticator;

namespace mws
{

class RestXMLParser;

#define MWS_CONTROLLER_CLASS friend class

class WebAPI : public midasLogAware
{
  // All of the web service controllers should be able to call Execute/Upload/Download
  MWS_CONTROLLER_CLASS Community;
  MWS_CONTROLLER_CLASS Collection;
  MWS_CONTROLLER_CLASS Item;
  MWS_CONTROLLER_CLASS Bitstream;
  MWS_CONTROLLER_CLASS Search;
  MWS_CONTROLLER_CLASS TreePath;
  MWS_CONTROLLER_CLASS NewResources;
public:
  // Singleton to be used in an application
  static WebAPI* Instance();

  bool DownloadBitstream(int id, const std::string& name);
  bool UploadBitstream(
    const std::string& uuid, int parentId,
    const std::string& name, const std::string& path,
    const std::string& size);
  bool GetIdByUuid(const std::string& uuid, std::string& id);
  bool CountBitstreams(int type, int id,
                       std::string& count, std::string& size);
  bool GetIdFromPath(const std::string& path, std::string& type,
                     std::string& id, std::string& uuid);
  bool DeleteResource(const std::string& typeName, int id);
  bool CheckUserAgreement(int type, int id, std::string& hasAgreed);

  // Set the REST API URL
  void SetServerUrl(const char* baseurl);
  const char* GetServerUrl();
  
  // Return the last error code
  int GetErrorCode();
  
  // Return the last error message
  const char* GetErrorMessage();
  
  // Set verbose mode
  void SetVerbose(bool verbose);
  
  // Get the default rest XML parser
  RestXMLParser* GetRestXMLParser();

  // Set the authenticator
  void SetAuthenticator(midasAuthenticator* auth);
  
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
  // Set the post data (only if you want to POST)
  void SetPostData(const char* postData);

  // Execute a web API command
  bool Execute(const char* url, bool retry = true);

  // Download a file
  bool DownloadFile(const char* url, const char* filename);
  
  // Upload a file
  bool UploadFile(const char* url, const char* filename);

  RestAPI*            m_RestAPI;
  RestXMLParser*      m_RestXMLParser;
  std::string         m_APIToken;
  const char*         m_PostData;
  midasAuthenticator* m_Authenticator;
  QMutex              m_Mutex;

  // constructor
  WebAPI();
  ~WebAPI();
private:
  static WebAPI * m_Instance; 

};

} // end namespace

#endif //__mwsRestAPI_H
