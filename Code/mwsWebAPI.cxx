/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mwsWebAPI.h"
#include "mwsRestXMLParser.h"
#include "midasAuthenticator.h"
#include <iostream>

namespace mws {

/** Singleton */
WebAPI * WebAPI::m_Instance = NULL; 

/** Constructor */
WebAPI::WebAPI()
{
  m_RestAPI = new RestAPI();
  m_RestXMLParser = new RestXMLParser();
  m_RestAPI->SetXMLParser(m_RestXMLParser);
  m_RestAPI->Initialize();
  m_PostData = NULL;
  m_Authenticator = NULL;
}

/** Destructor */
WebAPI::~WebAPI()
{
  m_RestAPI->Finalize();
  delete m_RestAPI;
  delete m_RestXMLParser;
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

void WebAPI::SetAuthenticator(midasAuthenticator* auth)
{
  m_Authenticator = auth;
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

void WebAPI::SetPostData(const char* postData)
{
  m_PostData = postData;
}

/** Execute the command */
bool WebAPI::Execute(const char* url, bool retry)
{
  m_RestAPI->SetProgressCallback(NULL, NULL);
  std::stringstream fullUrl;

  fullUrl << url;

  if(!m_APIToken.empty())
    {
    fullUrl << "&token=" << m_APIToken;
    }
  bool success = m_RestAPI->Execute(fullUrl.str().c_str(), m_PostData);

  if(retry && (!success || m_RestAPI->GetXMLParser()->GetErrorCode() != 0)
     && !m_APIToken.empty() && m_Authenticator && !m_RestAPI->ShouldCancel())
    {
    mws::RestXMLParser parser = *m_RestAPI->GetXMLParser(); //copy the parser or expat will crash...
    this->Log->Message("Operation failed. Refreshing login token and retrying...");
    if(!m_Authenticator->Login())
      {
      this->Log->Error("Attempt to get new tokens failed.");
      return false;
      }
    fullUrl.str(std::string());
    fullUrl << url << "&token=" << m_APIToken;
    m_RestAPI->SetXMLParser(&parser);
    success = m_RestAPI->Execute(fullUrl.str().c_str(), m_PostData);
    }

  m_PostData = ""; //reset the post data
  if(success && m_RestAPI->GetXMLParser()->GetErrorCode() == 0)
    {
    return true;
    }
  return false;
}

/** Check the connection to the MIDAS server */
bool WebAPI::CheckConnection()
{
  std::string version;
  mws::RestXMLParser parser;
  parser.AddTag("/rsp/version", version);
  this->GetRestAPI()->SetXMLParser(&parser);
  std::string url = "midas.info";
  if(!this->Execute(url.c_str(), false))
    {
    this->Log->Error(this->GetErrorMessage());
    return false;
    }
  if(version == "")
    {
    return false;
    }

  return true;
}
    
// Return the last error code
int WebAPI::GetErrorCode()
{
  return this->GetRestAPI()->GetXMLParser()->GetErrorCode();
}
  
// Return the last error message
const char* WebAPI::GetErrorMessage()
{
  return this->GetRestAPI()->GetXMLParser()->GetErrorMessage();
}

// Set verbose mode
void WebAPI::SetVerbose(bool verbose)
{
  m_RestAPI->SetVerbose(verbose);
}

// Get the default rest XML parser
RestXMLParser* WebAPI::GetRestXMLParser()
{
  return this->GetRestAPI()->GetXMLParser();
}
 
// Download a file 
bool WebAPI::DownloadFile(const char* url, const char* filename)
{
  std::string fullUrl(url);
  if(!m_APIToken.empty())
    {
    fullUrl += "&token=" + m_APIToken;
    }
  m_RestAPI->SetXMLParser(NULL);
  bool success = m_RestAPI->Download(filename,fullUrl,RestAPI::FILE);

  if(!success && !m_APIToken.empty() && m_Authenticator && !m_RestAPI->ShouldCancel())
    {
    this->Log->Message("Operation failed. Refreshing login token and retrying...");
    if(!m_Authenticator->Login())
      {
      this->Log->Error("Attempt to get new tokens failed.");
      m_RestAPI->SetXMLParser(m_RestXMLParser);
      return false;
      }
    fullUrl = url;
    fullUrl += "&token=" + m_APIToken;
    // Try again with the new token
    success = m_RestAPI->Download(filename,fullUrl,RestAPI::FILE);
    }
  m_RestAPI->SetXMLParser(m_RestXMLParser);
  return success;
}
 
// Upload a file 
bool WebAPI::UploadFile(const char* url, const char* filename)
{
  if(m_APIToken == "")
    {
    this->Log->Error("Token should be defined to upload to MIDAS.");
    this->Log->Error("Please use the Login() function to get a token.");
    this->GetRestXMLParser()->SetErrorMessage("Cannot push using anonymous access.");
    return false;
    }

  std::string completeUrl = url;
  completeUrl += "&token=";
  completeUrl += m_APIToken;
  m_RestAPI->SetInputMode(mws::RestAPI::FILE);
  bool ok = m_RestAPI->Upload(filename, completeUrl);
  ok &= std::string(this->GetRestXMLParser()->GetErrorMessage()) == "";
  ok &= this->GetRestXMLParser()->GetErrorCode() == 0;

  return ok;
}


std::string WebAPI::GetAPIToken()
{
  return this->m_APIToken;
}

// Login to MIDAS
bool WebAPI::Login(const char* appname,
                   const char* email, 
                   const char* apikey)
{
  m_APIToken = "";
  mws::RestXMLParser parser;
  parser.AddTag("/rsp/token",m_APIToken);
  this->GetRestAPI()->SetXMLParser(&parser);
  std::stringstream url;
  url << "midas.login?email=" << email;
  url << "&apikey=" << apikey;
  url << "&appname=" << appname;
  if(!this->Execute(url.str().c_str(), NULL))
    {
    this->Log->Error(this->GetErrorMessage());
    return false;
    }
  
  if(m_APIToken.size() < 40)
    {
    return false;
    }    
  return true;
}

//-------------------------------------------------------------------
bool WebAPI::DownloadBitstream(int id, const std::string& name)
{
  std::stringstream fields;
  fields << "midas.bitstream.download?id=" << id;
  return this->DownloadFile(fields.str().c_str(), name.c_str());
}

//-------------------------------------------------------------------
bool WebAPI::UploadBitstream(const std::string& uuid, int parentId,
                             const std::string& name, const std::string& path,
                             const std::string& size)
{
  std::stringstream fields;
  fields << "midas.upload.bitstream?uuid=" << uuid << "&itemid="
    << parentId << "&mode=stream&filename=" << name << "&path="
    << name << "&size=" << size;
  return this->UploadFile(fields.str().c_str(), path.c_str());
}

//-------------------------------------------------------------------
bool WebAPI::GetIdByUuid(const std::string& uuid, std::string& id)
{
  mws::RestXMLParser parser;
  parser.AddTag("/rsp/id", id);
  this->GetRestAPI()->SetXMLParser(&parser);

  std::stringstream fields;
  fields << "midas.resource.get?uuid=" << uuid;

  return this->Execute(fields.str().c_str());
}

//-------------------------------------------------------------------
bool WebAPI::CountBitstreams(int type, int id, std::string& count,
                             std::string& size)
{
  mws::RestXMLParser parser;
  parser.AddTag("/rsp/count", count);
  parser.AddTag("/rsp/size", size);
  this->GetRestAPI()->SetXMLParser(&parser);

  std::stringstream fields;
  fields << "midas.bitstream.count?id=" << id << "&type=" << type;

  return this->Execute(fields.str().c_str());
}

//-------------------------------------------------------------------
bool WebAPI::GetIdFromPath(const std::string& path, std::string& type,
                           std::string& id, std::string& uuid)
{
  mws::RestXMLParser parser;
  parser.AddTag("/rsp/type", type);
  parser.AddTag("/rsp/id", id);
  parser.AddTag("/rsp/uuid", uuid);
  this->GetRestAPI()->SetXMLParser(&parser);

  std::stringstream fields;
  fields << "midas.convert.path.to.id?path=" << midasUtils::EscapeForURL(path);

  return this->Execute(fields.str().c_str());
}

bool WebAPI::DeleteResource(const std::string& typeName, int id)
{
  mws::RestXMLParser parser;
  this->GetRestAPI()->SetXMLParser(&parser);

  std::stringstream fields;
  fields << "midas." << typeName << ".delete?id=" << id;

  return this->Execute(fields.str().c_str());
}

bool WebAPI::CheckUserAgreement(int type, int id, std::string& hasAgreed)
{
  mws::RestXMLParser parser;
  parser.AddTag("/rsp/hasAgreed", hasAgreed);

  std::stringstream fields;
  fields << "midas.check.user.agreement?id=" << id
    << "&type=" << type;

  this->GetRestAPI()->SetXMLParser(&parser);
  return this->Execute(fields.str().c_str());
}

} // end namespace
