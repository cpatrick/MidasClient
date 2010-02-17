/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mwsWebAPI.h"
#include "mwsRestXMLParser.h"
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

/** Constructor */
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
bool WebAPI::Execute(const char* url)
{  
  std::stringstream fullUrl;
  
  fullUrl << url;
  if(!m_APIToken.empty())
    {  
    fullUrl << "&token=" << m_APIToken;
    }

  bool success = m_RestAPI->Execute(fullUrl.str().c_str(), m_PostData);

  if(success && m_RestXMLParser->GetErrorCode() == 0)
    {
    return true;
    }
  return false;
}

/** Check the connection to the MIDAS server */
bool WebAPI::CheckConnection()
{
  std::string url = "midas.info";
  if(!this->Execute(url.c_str()))
    {
    std::cout << this->GetErrorMessage() << std::endl;
    return false;
    }
  return true;
}
    
// Return the last error code
int WebAPI::GetErrorCode()
{
  return m_RestXMLParser->GetErrorCode();
}
  
// Return the last error message
const char* WebAPI::GetErrorMessage()
{
  return m_RestXMLParser->GetErrorMessage();
}

// Set verbose mode
void WebAPI::SetVerbose(bool verbose)
{
  m_RestAPI->SetVerbose(verbose);
}

// Get the default rest XML parser
RestXMLParser* WebAPI::GetRestXMLParser()
{
  return m_RestXMLParser;
}
 
// Download a file 
bool WebAPI::DownloadFile(const char* url, const char* filename)
{
  m_RestAPI->SetXMLParser(NULL);
  bool success = m_RestAPI->Download(filename,url,RestAPI::FILE);
  m_RestAPI->SetXMLParser(m_RestXMLParser);
  return success;
}
 
// Upload a file 
unsigned int WebAPI::UploadFile(const char* url, const char* filename)
{
  if(m_APIToken == "")
    {
    std::cerr << "Token should be defined to upload to MIDAS.";
    std::cerr << "Please use the Login() function to get a token." << std::endl;
    return false;   
    }
  
  std::string completeUrl = url;
  completeUrl += "&token=";
  completeUrl += m_APIToken; 
  return m_RestAPI->UploadPost(filename,completeUrl);
}

  
// Login to MIDAS
bool WebAPI::Login(const char* appname,
                   const char* email, 
                   const char* apikey)
{
  RestXMLParser* parser = this->GetRestXMLParser();
  parser->AddTag("/rsp/token",m_APIToken);
  std::stringstream url;
  url << "midas.login?email=" << email;
  url << "&apikey=" << apikey;
  url << "&appname=" << appname;
  if(!this->Execute(url.str().c_str()))
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

// Login with email and password
// Password should be encrypted
bool WebAPI::LoginWithPassword(const char* appname,
                               const char* email, 
                               const char* password)
{
  RestXMLParser* parser = this->GetRestXMLParser();
  parser->AddTag("/rsp/token",m_APIToken);
  std::stringstream url;
  url << "midas.login?email=" << email;
  url << "&password=" << password;
  url << "&appname=" << appname;
  if(!this->Execute(url.str().c_str()))
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
