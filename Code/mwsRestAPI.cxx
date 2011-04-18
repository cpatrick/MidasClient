/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mwsRestAPI.h"

#include "midasStandardIncludes.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "sys/stat.h"
#include "mwsRestXMLParser.h"
#include "midasUtils.h"

namespace mws {

/** Constructor */
RestAPI::RestAPI()
{
  m_Initialized = false; 
  m_OutputMode = BUFFER;
  m_CurlCode = CURLE_OK;
  output_filestream = NULL;
  input_stream = NULL;
  m_cURL = NULL;
  upload_unique_id = "";
  fprogress = NULL;
  m_Verbose = false;
  m_XMLParser = NULL;
  m_Cancel = false;
}

/** Destructor */
RestAPI::~RestAPI()
{
  this->Finalize();
}

/** Set cURL in verbose mode */
void RestAPI::SetVerbose(bool verbose)
{
  m_Verbose = verbose;
}

/** Initialize cURL */
bool RestAPI::Initialize()
{
  if (!m_Initialized)
    {
    //curl_global_init(CURL_GLOBAL_ALL);
    m_Initialized = true; 
    }
  if(!m_cURL)
    {
    m_cURL = curl_easy_init(); 
    }
  if(!m_cURL)
    {
    return false;
    }
  return true;
}

/** Set the XML parser */
void RestAPI::SetXMLParser(mws::RestXMLParser* parser)
{
  m_XMLParser = parser;
}

mws::RestXMLParser* RestAPI::GetXMLParser()
{
  return m_XMLParser;
}
 
/** Set the cURL options */
bool RestAPI::SetCurlOptions(const char* url, 
                             const char* authentication)
{
  if(!m_cURL)
    {
    std::cout << "SetCurlOptions m_cURL is not initialized" << std::endl;
    return false;
    }
  curl_easy_setopt(m_cURL, CURLOPT_WRITEFUNCTION, Curl_output_function);
  curl_easy_setopt(m_cURL, CURLOPT_WRITEDATA, this);
  curl_easy_setopt(m_cURL, CURLOPT_SSL_VERIFYHOST, 1);
  curl_easy_setopt(m_cURL, CURLOPT_SSL_VERIFYPEER, false);
  curl_easy_setopt(m_cURL, CURLOPT_URL, url);

  if(m_Verbose)
    {
    curl_easy_setopt(m_cURL, CURLOPT_VERBOSE, 1);
    }
    
  curl_easy_setopt(m_cURL, CURLOPT_NOPROGRESS, this->fprogress == NULL);
  curl_easy_setopt(m_cURL, CURLOPT_PROGRESSFUNCTION, this->fprogress);
  curl_easy_setopt(m_cURL, CURLOPT_PROGRESSDATA, this->fprogress_data);

  curl_easy_setopt(m_cURL, CURLOPT_FOLLOWLOCATION, true);

  std::string authStr = authentication;
  if (authStr != "")
    {
    curl_easy_setopt(m_cURL, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt(m_cURL, CURLOPT_USERPWD,authentication);
    }
  return true;  
}

//--------------------------------------------------------------------------------------------------
bool RestAPI::PerformCurl()
{
  if(m_cURL == NULL)
    {
    return false;
    }

  if(m_OutputMode == BUFFER)
    {
    this->ResetOutputBuffer();
    }
  
  m_CurlCode = curl_easy_perform(m_cURL);
  if ( m_CurlCode != CURLE_OK )
    {
    std::cout << "Cannot run cURL: "  << curl_easy_strerror(m_CurlCode) << std::endl;
    return false;
    }
  return true;
}

//--------------------------------------------------------------------------------------------------
bool RestAPI::Execute(const char*  url, 
                      const char*  post_data,
                      const char * authentication)
{
  m_Cancel = false;
  if(!m_cURL)
    {
    std::cout << "Execute: cURL not initialized" << std::endl;
    return false;
    }  
 
  std::string URL = url;
  if(!m_ServerUrl.empty())
    {
    URL = m_ServerUrl + URL; 
    }
  
  // setup
  curl_easy_reset(m_cURL);
  m_OutputMode = BUFFER;

  this->SetCurlOptions(URL.c_str(), authentication);  

  if(post_data != NULL)
    {
    curl_easy_setopt(m_cURL, CURLOPT_POST, 1);
    curl_easy_setopt(m_cURL, CURLOPT_POSTFIELDS, post_data);
    }
  else 
    {
    curl_easy_setopt(m_cURL, CURLOPT_HTTPGET, 1);
    }

  m_XMLParser->Initialize();
  if(!this->PerformCurl())
    {
    return false;
    }
  m_XMLParser->Finalize();
   
  return true;  
}

//--------------------------------------------------------------------------------------------------
bool RestAPI::Download(const std::string &filename, std::string url, IO_MODE outputMode,
                       curl_progress_callback fprogress, void * fprogress_data,
                       const char* authentication)
{
  m_Cancel = false;
  std::string URL = url;
  if(!m_ServerUrl.empty())
    {
    URL = m_ServerUrl + URL; 
    }

  m_OutputMode = outputMode;
  // prepare output
  switch(m_OutputMode)
    {
    case BUFFER :
      ResetOutputBuffer();
      break;
    case FILE :
      this->output_filestream = new std::ofstream;
      this->output_filestream->open(filename.c_str(), std::ios::binary);
      if(!this->output_filestream->is_open())
        {
        std::cerr << "Cannot open local file for writing: " << filename.c_str() << std::endl;
        return false;
        }
      break;
    }

  curl_easy_reset(m_cURL);

  this->SetCurlOptions(URL.c_str(), authentication);
  curl_easy_setopt(m_cURL, CURLOPT_HTTPGET, 1);
  curl_easy_setopt(m_cURL, CURLOPT_PROGRESSDATA, fprogress_data == NULL ?
                                                 this->fprogress_data : fprogress_data);
  curl_easy_setopt(m_cURL, CURLOPT_PROGRESSFUNCTION, fprogress == NULL ?
                                                 this->fprogress : fprogress);

  bool success = this->PerformCurl();

  switch(m_OutputMode)
    {
    case BUFFER :
      break;
    case FILE :
      this->output_filestream->close();
      this->output_filestream = NULL;
      break;
    }
  return success;  
 }

//--------------------------------------------------------------------------------------------------
bool RestAPI::UploadPost(const char* filename, std::string url, curl_progress_callback fprogress,
                         void * fprogress_data, const char* authentication)
{
  m_Cancel = false;
  std::string URL = url;
  if(!m_ServerUrl.empty())
    {
    URL = m_ServerUrl + URL;
    }

  curl_easy_reset(m_cURL);

  struct curl_httppost *formpost=NULL;
  struct curl_httppost *lastptr=NULL;
  struct curl_slist *headerlist=NULL;
  static const char buf[] = "Expect:";

  m_OutputMode = BUFFER;
  this->SetCurlOptions(URL.c_str(), authentication);
  curl_easy_setopt(m_cURL, CURLOPT_POST, 1);
  curl_formadd(&formpost,
               &lastptr,
               CURLFORM_COPYNAME, "file",
               CURLFORM_FILE, filename,
               CURLFORM_END);
  curl_easy_setopt(m_cURL, CURLOPT_HTTPPOST, formpost);
 
  /* only disable 100-continue header if explicitly requested */ 
  headerlist = curl_slist_append(headerlist, buf);
  curl_easy_setopt(m_cURL, CURLOPT_HTTPHEADER, headerlist);
  
  /* initalize custom header list (stating that Expect: 100-continue is not
     wanted */ 
  headerlist = curl_slist_append(headerlist, buf);

  m_XMLParser->Initialize();
  bool success = this->PerformCurl();
  m_XMLParser->Finalize();

  success &= (m_XMLParser->GetErrorCode() == 0);
  
  curl_formfree(formpost);
  
  /* free slist */ 
  curl_slist_free_all (headerlist);

  return success;
}

//--------------------------------------------------------------------------------------------------
bool RestAPI::Upload(const std::string &data, std::string url, curl_progress_callback fprogress,
                     void * fprogress_data, const char* authentication)
{
  m_Cancel = false;
  std::string URL = url;
  if(!m_ServerUrl.empty())
    {
    URL = m_ServerUrl + URL; 
    }
  
  int64 datasize;
  std::ifstream * input_filestream = NULL;

  if (this->upload_unique_id.empty())
    {
    //TODO: FIX throw RestAPIException(__FILE__, __LINE__, __func__,"Upload unique identifier is NOT set");
    }

  // prepare input
  switch(input_mode)
    {
    case BUFFER:
      //Logger::debug(STR("\tdata(STRING:")+kwutils::to_string<size_t>(data.size())+")");
      datasize = data.size();
      this->input_stream = new std::stringstream(data, std::stringstream::out);
      break; 
    case FILE:
      //Logger::debug("\tdata(filename:"+data+")");
      datasize = midasUtils::GetFileLength(data.c_str());
      input_filestream = new std::ifstream;
      input_filestream->open(data.c_str(), std::ios::binary);
      if(!input_filestream->is_open())
        {
        //TODO: FIX throw RestAPIException(__FILE__, __LINE__, __func__, "Failed to open file:" + data);
        } 
      this->input_stream = input_filestream; 
      break;
    }

  curl_easy_reset(m_cURL);

  m_OutputMode = BUFFER;
  this->SetCurlOptions(URL.c_str(), authentication);

  void * read_function_data = (void*) this; 
  curl_easy_setopt(m_cURL, CURLOPT_UPLOAD, true);
  curl_easy_setopt(m_cURL, CURLOPT_INFILESIZE_LARGE, datasize);
  curl_easy_setopt(m_cURL, CURLOPT_READFUNCTION, Curl_read_function);
  curl_easy_setopt(m_cURL, CURLOPT_READDATA, read_function_data);

  // remove 'Expect: 100-continue' header
  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, "Expect: ");
  curl_easy_setopt(m_cURL, CURLOPT_HTTPHEADER, headers);

  curl_easy_setopt(m_cURL, CURLOPT_PROGRESSDATA, fprogress_data == NULL ?
                                                this->fprogress_data : fprogress_data);
  curl_easy_setopt(m_cURL, CURLOPT_PROGRESSFUNCTION, fprogress == NULL ?
                                                    this->fprogress : fprogress);

  m_XMLParser->Initialize();
  bool success = this->PerformCurl();
  m_XMLParser->Finalize();

  if (input_filestream != NULL)
    {
    input_filestream->close();
    input_filestream = NULL; 
    }
  curl_slist_free_all(headers);

  return success;
}

/** Finalize */
void RestAPI::Finalize()
{
  if(!m_Initialized)
    {
    return;
    }
    
  if (m_cURL != NULL)
    {
    curl_easy_cleanup(m_cURL); 
    }
    
  if (this->input_stream != NULL )
    {
    this->input_stream->clear();
    delete this->input_stream; 
    this->input_stream = NULL; 
    }
  m_cURL = NULL;
  m_Initialized = false;  
}

/** Cancel an upload or download */
void RestAPI::Cancel()
{
  m_Cancel = true;
}

/** Whether download or upload should be aborted */
bool RestAPI::ShouldCancel()
{
  return m_Cancel;
}

//--------------------------------------------------------------------------------------------------
RestAPI::IO_MODE RestAPI::GetOutputMode()
{
  return m_OutputMode; 
}

//--------------------------------------------------------------------------------------------------
RestAPI::IO_MODE RestAPI::GetInputMode()
{
  return this->input_mode; 
}

/** Set the base URL */
void RestAPI::SetServerUrl(const char*  baseurl)
{
  std::string temp(baseurl);
  // start with 'http://' ou 'https://' :
  if(temp.substr(0, 7).compare("http://")!=0 && temp.substr(0, 8).compare("https://")!=0 )
    {
    temp.insert(0, "http://");
    }
  // end with a '/' :
  if(temp.substr(temp.length()-1).compare("/")!=0)
    {
    temp.append("/");
    }
  m_ServerUrl = temp;
}

/** Get the base URL */
const char* RestAPI::GetServerUrl()
{
  return m_ServerUrl.c_str();
}
//--------------------------------------------------------------------------------------------------
void RestAPI::SetInputMode(IO_MODE input_mode)
  {
  this->input_mode = input_mode; 
  }


//--------------------------------------------------------------------------------------------------
std::ofstream* RestAPI::GetOutputFileStream()
  {
  return this->output_filestream; 
  }

//--------------------------------------------------------------------------------------------------
std::istream* RestAPI::GetInputStream()
  {
  return this->input_stream; 
  }

//--------------------------------------------------------------------------------------------------
std::string &RestAPI::GetOutputBuffer()
  {
  return this->output_buffer; 
  }

//--------------------------------------------------------------------------------------------------
void RestAPI::ResetOutputBuffer()
  {
  output_buffer.clear();
  }

//--------------------------------------------------------------------------------------------------
void RestAPI::SetProgressCallback(curl_progress_callback fprogress, void* fprogress_data)
{
  this->fprogress = fprogress; 
  this->fprogress_data = fprogress_data; 
}

bool RestAPI::Parse(const char* buffer,unsigned long length)
{
  if(!m_XMLParser)
    {
    return false;
    }
  return m_XMLParser->Parse(buffer,length);
}

//--------------------------------------------------------------------------------------------------
static size_t Curl_output_function(void *ptr, size_t size, size_t nitems, void *userp)
{
  char *buffer = (char*)ptr;
  RestAPI * restAPI = (RestAPI*)userp;
  if(restAPI->ShouldCancel())
    {
    return 0;
    }

  size_t length = size*nitems;   
  switch (restAPI->GetOutputMode())
    {
    case RestAPI::FILE:
      restAPI->GetOutputFileStream()->write(buffer, length);
      break; 
    case RestAPI::BUFFER:
      restAPI->GetOutputBuffer().append(buffer, length);
      restAPI->Parse(buffer,length);
      break;
    }
  return length; 
}

//--------------------------------------------------------------------------------------------------
static size_t Curl_read_function(void *bufptr, size_t size, size_t nitems, void *userp)
{
  char *buffer = (char*)bufptr;
  RestAPI * restAPI = (RestAPI*)userp;
  if(restAPI->ShouldCancel())
    {
    return CURL_READFUNC_ABORT;
    }
  size_t length = size*nitems;
  restAPI->GetInputStream()->read(buffer, length);
  return restAPI->GetInputStream()->gcount();
}

} // end namespace
