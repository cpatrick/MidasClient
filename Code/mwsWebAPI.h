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

  bool GetIdByUuid(const std::string& uuid, std::string& id);
  // TODO This should be a method of mws::Object
  bool CountBitstreams(int type, int id,
                       std::string& count, std::string& size);
  bool GetIdFromPath(const std::string& path, std::string& type,
                     std::string& id, std::string& uuid);
  bool DeleteResource(const std::string& typeName, int id);
  bool CheckUserAgreement(int type, int id, std::string& hasAgreed);

  // Set the REST API URL
  void SetServerUrl(const char* baseurl);
  const char* GetServerUrl();
  
  // Set verbose mode
  void SetVerbose(bool verbose);

  // Set the authenticator
  void SetAuthenticator(midasAuthenticator* auth);

  // Set the default progress handler
  void SetProgressCallback(curl_progress_callback fprogress,
                           void* progressData);
  
  // After calling Login, use this to get the API token
  std::string GetAPIToken();

  // Login to MIDAS
  bool Login(const char* applicationname,
             const char* email,
             const char* apikey);
 
  // Check the connection to the MIDAS server
  bool CheckConnection();

  // Cancel the upload or download
  void Cancel();

protected:
  // Execute a web API command
  bool Execute(const char* url, RestXMLParser* parser = NULL,
               const char* postData = NULL,
               bool retry = true, bool lock = true);

  // Download a file
  bool DownloadFile(const char* url, const char* filename,
                    curl_progress_callback fprogress = NULL,
                    void* progressData = NULL);
  
  // Upload a file
  bool UploadFile(const char* url, const char* filename,
                  curl_progress_callback fprogress = NULL,
                  void* progressData = NULL);

  RestAPI*               m_RestAPI;
  std::string            m_APIToken;
  midasAuthenticator*    m_Authenticator;
  QMutex*                m_Mutex;
  curl_progress_callback m_Fprogress;
  void*                  m_ProgressData;

  // constructor
  WebAPI();
  ~WebAPI();
private:
  static WebAPI * m_Instance; 

};

} // end namespace

#endif //__mwsRestAPI_H
