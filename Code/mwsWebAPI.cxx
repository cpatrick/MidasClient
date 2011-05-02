/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mwsWebAPI.h"
#include "mwsRestXMLParser.h"
#include "mwsMirrorHandler.h"
#include "mwsBitstream.h"
#include "mdoBitstream.h"
#include "midasAuthenticator.h"
#include <iostream>
#include <QMutexLocker>

namespace mws {

/** Singleton */
WebAPI * WebAPI::m_Instance = NULL; 

/** Constructor */
WebAPI::WebAPI()
{
  m_RestAPI = new RestAPI();
  m_RestAPI->Initialize();
  m_Mutex = new QMutex(QMutex::Recursive);
  m_Authenticator = NULL;
  m_MirrorHandler = NULL;
}

/** Destructor */
WebAPI::~WebAPI()
{
  m_RestAPI->Finalize();
  delete m_RestAPI;
}

/** Set the base url */
void WebAPI::SetServerUrl(const char* baseurl)
{
  m_RestAPI->SetServerUrl(baseurl);
}

/** Get the base url */
const char* WebAPI::GetServerUrl()
{
  return m_RestAPI->GetServerUrl();
}

MirrorHandler* WebAPI::GetMirrorHandler()
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

void WebAPI::SetProgressCallback(curl_progress_callback fprogress,
                                 void* progressData)
{
  m_Fprogress = fprogress;
  m_ProgressData = progressData;
}

/** Return the instance as a singleton */
WebAPI* WebAPI::Instance()
{
  if ( m_Instance != NULL )
    {
    return m_Instance; 
    }
  else 
    {
    m_Instance = new WebAPI();
    }
  return m_Instance;
}

// Set verbose mode
void WebAPI::SetVerbose(bool verbose)
{
  m_RestAPI->SetVerbose(verbose);
}

std::string WebAPI::GetAPIToken()
{
  return this->m_APIToken;
}

void WebAPI::Cancel()
{
  m_RestAPI->Cancel();
}

/** Execute the command */
bool WebAPI::Execute(const char* url, RestXMLParser* parser,
                     const char* postData, bool retry, bool ignoreError)
{
  QMutexLocker locker(m_Mutex);
  bool defaultParser = parser == NULL;
  if(defaultParser)
    {
    parser = new RestXMLParser;
    }
  m_RestAPI->SetXMLParser(parser);
  m_RestAPI->SetProgressCallback(NULL, NULL);
  std::stringstream fullUrl;

  fullUrl << url;

  if(!m_APIToken.empty())
    {
    fullUrl << "&token=" << m_APIToken;
    }
  bool success = m_RestAPI->Execute(fullUrl.str().c_str(), postData);

  if(ignoreError)
    {
    return success;
    }

  if(retry && (!success || m_RestAPI->GetXMLParser()->GetErrorCode() != 0)
     && !m_APIToken.empty() && m_Authenticator && !m_RestAPI->ShouldCancel())
    {
    this->Log->Message(
      "Operation failed. Refreshing login token and retrying...");

    if(!m_Authenticator->Login())
      {
      this->Log->Error("Attempt to get new tokens failed.");
      if(defaultParser)
        {
        delete parser;
        }
      return false;
      }
    fullUrl.str(std::string());
    fullUrl << url << "&token=" << m_APIToken;
    m_RestAPI->SetXMLParser(parser);
    success = m_RestAPI->Execute(fullUrl.str().c_str(), postData);
    }

  if(success && m_RestAPI->GetXMLParser()->GetErrorCode() == 0)
    {
    if(defaultParser)
      {
      delete parser;
      }
    return true;
    }
  std::stringstream text;
  text << "Web API call to \"" << url << "\" failed. Response: " <<
    m_RestAPI->GetXMLParser()->GetErrorMessage() << std::endl;
  this->Log->Error(text.str());

  if(defaultParser)
    {
    delete parser;
    }
  return false;
}

// Download a file 
bool WebAPI::DownloadFile(const char* url, const char* filename,
                          curl_progress_callback fprogress,
                          void* progressData)
{
  QMutexLocker locker(m_Mutex);
  std::string fullUrl(url);
  if(!m_APIToken.empty())
    {
    fullUrl += "&token=" + m_APIToken;
    }
  m_RestAPI->SetXMLParser(NULL);
  if(fprogress == NULL)
    {
    fprogress = m_Fprogress;
    progressData = m_ProgressData;
    }
  m_RestAPI->SetProgressCallback(fprogress, progressData);
  bool success = m_RestAPI->Download(filename, fullUrl, RestAPI::FILE);

  if(!success && !m_APIToken.empty() && m_Authenticator
     && !m_RestAPI->ShouldCancel())
    {
    this->Log->Message("Operation failed. Refreshing login token and retrying...");
    if(!m_Authenticator->Login())
      {
      this->Log->Error("Attempt to get new tokens failed.");
      return false;
      }
    fullUrl = url;
    fullUrl += "&token=" + m_APIToken;
    // Try again with the new token
    success = m_RestAPI->Download(filename,fullUrl,RestAPI::FILE);
    }
  if(!success)
    {
    std::stringstream text;
    text << "Web API download from \"" << url << "\" failed.";
    this->Log->Error(text.str());
    }
  return success;
}
 
// Upload a file 
bool WebAPI::UploadFile(const char* url, const char* filename,
                        curl_progress_callback fprogress, void* progressData)
{
  QMutexLocker locker(m_Mutex);

  if(m_APIToken == "")
    {
    this->Log->Error("Token should be defined to upload to MIDAS. "
      "Please use the Login() function to get a token.");
    return false;
    }

  if(fprogress == NULL)
    {
    fprogress = m_Fprogress;
    progressData = m_ProgressData;
    }
  m_RestAPI->SetProgressCallback(fprogress, progressData);
  RestXMLParser parser;
  m_RestAPI->SetXMLParser(&parser);

  std::string fullUrl = url;
  fullUrl += "&token=";
  fullUrl += m_APIToken;
  m_RestAPI->SetInputMode(RestAPI::FILE);
  bool success = m_RestAPI->Upload(filename, fullUrl);
  success &= std::string(parser.GetErrorMessage()) == "";
  success &= parser.GetErrorCode() == 0;

  if(!success && !m_APIToken.empty() && m_Authenticator
     && !m_RestAPI->ShouldCancel())
    {
    this->Log->Message("Operation failed. Refreshing login token and retrying...");
    if(!m_Authenticator->Login())
      {
      this->Log->Error("Attempt to get new tokens failed.");
      return false;
      }
    fullUrl = url;
    fullUrl += "&token=" + m_APIToken;
    // Try again with the new token
    m_RestAPI->SetXMLParser(&parser);
    success = m_RestAPI->Upload(filename, fullUrl);
    }

  if(!success)
    {
    std::stringstream text;
    text << "Web API file upload to \"" << url << "\" failed. Response: " <<
      parser.GetErrorMessage();
    this->Log->Error(text.str());
    }
  return success;
}

/** Check the connection to the MIDAS server */
bool WebAPI::CheckConnection()
{
  std::string version;
  mws::RestXMLParser parser;
  parser.AddTag("/rsp/version", version);

  std::string url = "midas.info";
  if(!this->Execute(url.c_str(), &parser, NULL, false))
    {
    return false;
    }
  if(version == "")
    {
    return false;
    }
  return true;
}

// Login to MIDAS
bool WebAPI::Login(const char* appname,
                   const char* email, 
                   const char* apikey)
{
  m_APIToken = "";
  RestXMLParser parser;
  parser.AddTag("/rsp/token",m_APIToken);

  std::stringstream url;
  url << "midas.login?email=" << email;
  url << "&apikey=" << apikey;
  url << "&appname=" << appname;
  if(!this->Execute(url.str().c_str(), &parser, NULL, false))
    {
    return false;
    }

  if(m_APIToken.size() < 40)
    {
    return false;
    }    
  return true;
}

//-------------------------------------------------------------------
bool WebAPI::GetIdByUuid(const std::string& uuid, std::string& id)
{
  RestXMLParser parser;
  parser.AddTag("/rsp/id", id);

  std::stringstream fields;
  fields << "midas.resource.get?uuid=" << uuid;

  return this->Execute(fields.str().c_str(), &parser);
}

//-------------------------------------------------------------------
bool WebAPI::CountBitstreams(int type, int id, std::string& count,
                             std::string& size)
{
  RestXMLParser parser;
  parser.AddTag("/rsp/count", count);
  parser.AddTag("/rsp/size", size);

  std::stringstream fields;
  fields << "midas.bitstream.count?id=" << id << "&type=" << type;

  return this->Execute(fields.str().c_str(), &parser);
}

//-------------------------------------------------------------------
bool WebAPI::GetIdFromPath(const std::string& path, std::string& type,
                           std::string& id, std::string& uuid)
{
  RestXMLParser parser;
  parser.AddTag("/rsp/type", type);
  parser.AddTag("/rsp/id", id);
  parser.AddTag("/rsp/uuid", uuid);

  std::stringstream fields;
  fields << "midas.convert.path.to.id?path=" << midasUtils::EscapeForURL(path);

  return this->Execute(fields.str().c_str(), &parser);
}

bool WebAPI::DeleteResource(const std::string& typeName, int id)
{
  std::stringstream fields;
  fields << "midas." << typeName << ".delete?id=" << id;

  return this->Execute(fields.str().c_str());
}

bool WebAPI::CheckUserAgreement(int type, int id, std::string& hasAgreed)
{
  RestXMLParser parser;
  parser.AddTag("/rsp/hasAgreed", hasAgreed);

  std::stringstream fields;
  fields << "midas.check.user.agreement?id=" << id
    << "&type=" << type;

  return this->Execute(fields.str().c_str(), &parser);
}

} // end namespace
