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

#include "mwsWebAPI.h"
#include "mwsRestResponseParser.h"
#include "mwsMirrorHandler.h"
#include "mwsBitstream.h"
#include "mdoBitstream.h"
#include "midasAuthenticator.h"
#include "mdsPartialDownload.h"
#include "mdsPartialUpload.h"

#include <QMutexLocker>
#include <QFileInfo>

namespace mws
{

/** Singleton */
WebAPI * WebAPI::m_Instance = NULL;

WebAPI * WebAPI::Instance()
{
  if( m_Instance != NULL )
    {
    return m_Instance;
    }
  else
    {
    m_Instance = new WebAPI();
    }
  return m_Instance;
}

WebAPI::WebAPI()
  : m_MirrorHandler(NULL), m_Authenticator(NULL), m_Progress(NULL), m_ServerVersion(NULL)
{
  m_RestAPI = new RestAPI();
  m_Mutex = new QMutex(QMutex::Recursive);
}

WebAPI::~WebAPI()
{
  delete m_RestAPI;
  delete m_Mutex;
  delete m_ServerVersion;
}

void WebAPI::SetServerUrl(const char* baseurl)
{
  m_RestAPI->SetServerUrl(baseurl);
}

const char * WebAPI::GetServerUrl()
{
  return m_RestAPI->GetServerUrl();
}

MirrorHandler * WebAPI::GetMirrorHandler()
{
  return m_MirrorHandler;
}

void WebAPI::SetMirrorHandler(MirrorHandler* handler)
{
  m_MirrorHandler = handler;
}

void WebAPI::SetAuthenticator(midasAuthenticator* auth)
{
  m_Authenticator = auth;
}

void WebAPI::SetProgressReporter(midasProgressReporter* progress)
{
  m_RestAPI->SetProgressReporter(progress);
  m_Progress = progress;
}

std::string WebAPI::GetAPIToken()
{
  return this->m_APIToken;
}

void WebAPI::Cancel()
{
  m_RestAPI->Cancel();
}

// -------------------------------------------------------------------
bool WebAPI::Execute(const char* url, RestResponseParser* parser,
                     const char* postData, bool retry, bool ignoreError)
{
  QMutexLocker locker(m_Mutex);
  bool         defaultParser = parser == NULL;

  if( defaultParser )
    {
    parser = new RestResponseParser;
    }

  m_RestAPI->SetProgressReporter(NULL);
  std::stringstream fullUrl;

  fullUrl << url << "&format=json";

  if( !m_APIToken.empty() )
    {
    fullUrl << "&token=" << m_APIToken;
    }
  bool success = m_RestAPI->Execute(fullUrl.str().c_str(), parser, postData);

  if( ignoreError )
    {
    return success;
    }

  if( retry && (!success || parser->GetErrorCode() != 0) && !m_APIToken.empty()
      && m_Authenticator && !m_RestAPI->ShouldCancel() )
    {
    this->Log->Message(
      "Operation failed. Refreshing login token and retrying...");

    if( !m_Authenticator->Login() )
      {
      this->Log->Error("Attempt to get new tokens failed.");
      if( defaultParser )
        {
        delete parser;
        }
      return false;
      }
    fullUrl.str(std::string() );
    fullUrl << url << "&format=json&token=" << m_APIToken;
    success = m_RestAPI->Execute(fullUrl.str().c_str(), parser, postData);
    }

  if( success && parser->GetErrorCode() == 0 )
    {
    if( defaultParser )
      {
      delete parser;
      }
    return true;
    }
  std::stringstream text;
  text << "Web API call to \"" << url << "\" failed. Response: "
       << parser->GetErrorMessage() << std::endl;
  this->Log->Error(text.str() );

  if( defaultParser )
    {
    delete parser;
    }
  return false;
}

// -------------------------------------------------------------------
bool WebAPI::DownloadFile(const char* url, const char* filename, int64 offset)
{
  QMutexLocker      locker(m_Mutex);
  std::stringstream fullUrl;

  fullUrl << url;
  if( !m_APIToken.empty() )
    {
    fullUrl << "&token=" << m_APIToken;
    }
  if( offset )
    {
    fullUrl << "&offset=" << offset;
    }
  m_RestAPI->SetProgressReporter(m_Progress);

  bool success = m_RestAPI->Download(filename, fullUrl.str(), offset);

  if( !success && !m_APIToken.empty() && m_Authenticator
      && !m_RestAPI->ShouldCancel() )
    {
    this->Log->Message("Operation failed. Refreshing login token and retrying...");
    if( !m_Authenticator->Login() )
      {
      this->Log->Error("Attempt to get new tokens failed.");
      return false;
      }
    fullUrl.str(std::string() );
    fullUrl << url;
    fullUrl << "&token=" << m_APIToken;
    if( offset )
      {
      fullUrl << "&offset=" << offset;
      }
    QFileInfo fileInfo(filename); // refresh size since it may have changed
    // Try again with the new token
    success = m_RestAPI->Download(filename, fullUrl.str(), fileInfo.size() );
    }
  if( !success )
    {
    std::stringstream text;
    text << "Web API download from \"" << url << "\" failed.";
    this->Log->Error(text.str() );
    }
  return success;
}

// -------------------------------------------------------------------
bool WebAPI::UploadFile(const char* url, const char* filename, int64 offset)
{
  QMutexLocker locker(m_Mutex);

  RestResponseParser parser;
  std::string        fullUrl = url;

  fullUrl += "&format=json";
  m_RestAPI->SetProgressReporter(m_Progress);

  bool success = m_RestAPI->Upload(filename, fullUrl, &parser, offset);
  success &= std::string(parser.GetErrorMessage() ) == "";
  success &= parser.GetErrorCode() == 0;

  if( !success )
    {
    std::stringstream text;
    text << "Web API file upload to \"" << url << "\" failed. Response: "
         << parser.GetErrorMessage();
    this->Log->Error(text.str() );
    }
  return success;
}

// -------------------------------------------------------------------
bool WebAPI::CheckConnection()
{
  std::string             version;
  mws::RestResponseParser parser;

  parser.AddTag("version", version);

  std::string url = "midas.info";
  if( !this->Execute(url.c_str(), &parser, NULL, false) )
    {
    return false;
    }
  if( version == "" )
    {
    return false;
    }
  delete m_ServerVersion;
  this->m_ServerVersion = new mdo::Version(version);
  return true;
}

// -------------------------------------------------------------------
bool WebAPI::Login(const char* appname,
                   const char* email,
                   const char* apikey)
{
  m_APIToken = "";
  RestResponseParser parser;
  parser.AddTag("token", m_APIToken);

  std::stringstream url;
  url << "midas.login&email=" << email;
  url << "&apikey=" << apikey;
  url << "&appname=" << appname;
  if( !this->Execute(url.str().c_str(), &parser, NULL, false) )
    {
    return false;
    }

  if( m_APIToken.size() < 40 )
    {
    return false;
    }
  return true;
}

// -------------------------------------------------------------------
void WebAPI::Logout()
{
  m_APIToken = "";
}

// -------------------------------------------------------------------
bool WebAPI::GetIdByUuid(const std::string& uuid, std::string& id)
{
  RestResponseParser parser;

  parser.AddTag("id", id);

  std::stringstream fields;
  fields << "midas.resource.get&folder&uuid=" << uuid;

  return this->Execute(fields.str().c_str(), &parser);
}

// -------------------------------------------------------------------
bool WebAPI::CountBitstreams(int type, int id, std::string& count,
                             std::string& size)
{
  RestResponseParser parser;

  parser.AddTag("count", count);
  parser.AddTag("size", size);

  std::stringstream fields;
  fields << "midas.bitstream.count&id=" << id << "&type=" << type;

  return this->Execute(fields.str().c_str(), &parser);
}

// -------------------------------------------------------------------
bool WebAPI::GetIdFromPath(const std::string& path, std::string& type,
                           std::string& id, std::string& uuid)
{
  RestResponseParser parser;

  parser.AddTag("type", type);
  parser.AddTag("id", id);
  parser.AddTag("uuid", uuid);

  std::stringstream fields;
  fields << "midas.convert.path.to.id&path=" << path;

  return this->Execute(fields.str().c_str(), &parser);
}

// -------------------------------------------------------------------
bool WebAPI::DeleteResource(const std::string& typeName, int id)
{
  QString           lower(typeName.c_str() );
  std::stringstream fields;

  fields << "midas." << lower.toLower().toStdString() << ".delete&id=" << id;

  return this->Execute(fields.str().c_str() );
}

// -------------------------------------------------------------------
bool WebAPI::CheckUserAgreement(int type, int id, std::string& hasAgreed)
{
  RestResponseParser parser;

  parser.AddTag("hasAgreed", hasAgreed);

  std::stringstream fields;
  fields << "midas.check.user.agreement&id=" << id
         << "&type=" << type;

  return this->Execute(fields.str().c_str(), &parser);
}

// -------------------------------------------------------------------
bool WebAPI::GetUploadOffset(const std::string& token, int64& offset)
{
  std::string        offsetStr;
  RestResponseParser parser;

  parser.AddTag("offset", offsetStr);

  std::stringstream fields;
  fields << "midas.upload.getoffset&uploadtoken=" << token;

  bool success = this->Execute(fields.str().c_str(), &parser);
  offset = QString(offsetStr.c_str() ).toLongLong();

  return success;
}

// -------------------------------------------------------------------
bool WebAPI::GetDefaultAPIKey(const std::string& email,
                              const std::string& password,
                              std::string& apiKey)
{
  if( !SERVER_IS_MIDAS3 )
    {
    return false;
    }

  RestResponseParser parser;
  parser.AddTag("apikey", apiKey);

  std::stringstream postData;
  postData << "email=" << email << "&password=" << password;

  return this->Execute("midas.user.apikey.default", &parser,
                       postData.str().c_str(), false);
}

mdo::Version * WebAPI::GetServerVersion()
{
  return m_ServerVersion;
}

} // end namespace
