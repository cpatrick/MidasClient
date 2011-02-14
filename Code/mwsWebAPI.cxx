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
bool WebAPI::Execute(const char* url, midasAuthenticator* auth)
{
  m_RestAPI->SetProgressCallback(NULL, NULL);
  std::stringstream fullUrl;

  fullUrl << url;

  if(!m_APIToken.empty())
    {
    fullUrl << "&token=" << m_APIToken;
    }
  bool success = m_RestAPI->Execute(fullUrl.str().c_str(), m_PostData);

  if((!success || m_RestAPI->GetXMLParser()->GetErrorCode() != 0) && !m_APIToken.empty() && auth && !m_RestAPI->ShouldCancel())
    {
    mws::RestXMLParser parser = *m_RestAPI->GetXMLParser(); //copy the parser or expat will crash...
    auth->GetLog()->Message("Operation failed. Refreshing login token and retrying...");
    if(!auth->Login(this))
      {
      auth->GetLog()->Error("Attempt to get new tokens failed.");
      return false;
      }
    fullUrl.str(std::string());
    fullUrl << url << "&token=" << m_APIToken;
    m_RestAPI->SetXMLParser(&parser);
    success = m_RestAPI->Execute(fullUrl.str().c_str(), m_PostData);
    }

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
  if(!this->Execute(url.c_str(), NULL))
    {
    std::cout << this->GetErrorMessage() << std::endl;
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
bool WebAPI::DownloadFile(const char* url, const char* filename,
                          midasAuthenticator* auth)
{
  std::string fullUrl(url);
  if(!m_APIToken.empty())
    {
    fullUrl += "&token=" + m_APIToken;
    }
  m_RestAPI->SetXMLParser(NULL);
  bool success = m_RestAPI->Download(filename,fullUrl,RestAPI::FILE);

  if(!success && !m_APIToken.empty() && auth && !m_RestAPI->ShouldCancel())
    {
    auth->GetLog()->Message("Operation failed. Refreshing login token and retrying...");
    if(!auth->Login(this))
      {
      auth->GetLog()->Error("Attempt to get new tokens failed.");
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
bool WebAPI::UploadFile(const char* url, const char* filename,
                        midasAuthenticator* auth)
{
  if(m_APIToken == "")
    {
    std::cerr << "Token should be defined to upload to MIDAS." << std::endl;
    std::cerr << "Please use the Login() function to get a token." << std::endl;
    this->GetRestXMLParser()->SetErrorMessage("Cannot push using anonymous access.");
    return false;
    }

  std::string completeUrl = url;
  completeUrl += "&token=";
  completeUrl += m_APIToken;
  m_RestAPI->SetInputMode(mws::RestAPI::FILE);
  bool ok = m_RestAPI->Upload(filename,completeUrl);
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
    std::cout << this->GetErrorMessage() << std::endl;
    return false;
    }
  
  if(m_APIToken.size() < 40)
    {
    return false;
    }    
  return true;
}

} // end namespace
