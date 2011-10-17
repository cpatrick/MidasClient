/******************************************************************************
 * Copyright 2011 Kitware Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/


#ifndef __mwsWebAPI_H
#define __mwsWebAPI_H

#include <QMutex>

#include "mwsRestAPI.h"
#include "midasLogAware.h"
#include "mdoVersion.h"

#define SERVER_IS_MIDAS3 (mws::WebAPI::Instance()->GetServerVersion()->Major == 3)

class midasAuthenticator;

namespace mdo
{
class Version;
}

namespace mds
{
class PartialDownload;
class PartialUpload;
}

namespace m3ws
{
class Folder;
class Item;
class Bitstream;
}

namespace mws
{

class MirrorHandler;
class RestResponseParser;

#define MWS_CONTROLLER_CLASS friend class

class WebAPI : public midasLogAware
{
  // All of the web api controllers need to call Execute/Upload/Download
  MWS_CONTROLLER_CLASS Community;
  MWS_CONTROLLER_CLASS Collection;
  MWS_CONTROLLER_CLASS Item;
  MWS_CONTROLLER_CLASS Bitstream;
  MWS_CONTROLLER_CLASS Search;
  MWS_CONTROLLER_CLASS TreePath;
  MWS_CONTROLLER_CLASS NewResources;
  MWS_CONTROLLER_CLASS m3ws::Folder;
  MWS_CONTROLLER_CLASS m3ws::Item;
  MWS_CONTROLLER_CLASS m3ws::Bitstream;
public:
  // Singleton to be used in an application
  static WebAPI * Instance();

  bool GetIdByUuid(const std::string& uuid, std::string& id);

  bool CountBitstreams(int type, int id, std::string& count, std::string& size);

  bool GetIdFromPath(const std::string& path, std::string& type,
                     std::string& id, std::string& uuid);

  bool DeleteResource(const std::string& typeName, int id);

  bool CheckUserAgreement(int type, int id, std::string& hasAgreed);

  bool GetUploadOffset(const std::string& token, int64& offset);

  bool GetDefaultAPIKey(const std::string& email, const std::string& password,
                        std::string& apiKey);

  // Set the REST API URL
  void SetServerUrl(const char* baseurl);

  const char * GetServerUrl();

  // Set the mirror handler
  void SetMirrorHandler(MirrorHandler* handler);

  MirrorHandler * GetMirrorHandler();

  // Set the authenticator
  void SetAuthenticator(midasAuthenticator* auth);

  // Set the default progress handler
  void SetProgressReporter(midasProgressReporter* reporter);

  // After calling Login, use this to get the API token
  std::string GetAPIToken();

  // Login to MIDAS
  bool Login(const char* applicationname, const char* email, const char* apikey);

  // Clears the API token
  void Logout();

  // Check the connection to the MIDAS server
  bool CheckConnection();

  mdo::Version * GetServerVersion();

  // Cancel the upload or download
  void Cancel();

protected:
  // Execute a web API command
  bool Execute(const char* url, RestResponseParser* parser = NULL, const char* postData = NULL, bool retry = true,
               bool ignoreError = false);

  // Download a file
  bool DownloadFile(const char* url, const char* filename, int64 offset = 0);

  // Upload a file
  bool UploadFile(const char* url, const char* filename, int64 offset = 0);

  MirrorHandler*         m_MirrorHandler;
  RestAPI*               m_RestAPI;
  std::string            m_APIToken;
  midasAuthenticator*    m_Authenticator;
  midasProgressReporter* m_Progress;
  mdo::Version*          m_ServerVersion;
  QMutex*                m_Mutex;

  // constructor
  WebAPI();
  ~WebAPI();
private:
  static WebAPI * m_Instance;

};

} // end namespace

#endif // __mwsRestAPI_H
