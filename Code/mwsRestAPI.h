/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __RestAPI_H
#define __RestAPI_H

#include <fstream>
#include <string>
#include <exception>
#include <sstream>

#include "Utilities/cmcurl/curl/curl.h"

namespace mws{

class RestXMLParser;

// This class provide the functionalities needed to search, download and upload files from/to a
// MIDAS server.
class RestAPI
{
public:
  
  enum IO_MODE { FILE, BUFFER, };
    
  // Constructor/Destructor
  RestAPI();
  ~RestAPI();
  
  // Initialize cURL
  bool Initialize();
  
  // Cleanup    
  void Finalize();
  
  std::ofstream* GetOutputFileStream();
  std::string& GetOutputBuffer();
  IO_MODE GetInputMode();
  std::istream* GetInputStream();
  const std::string& GetUploadUniqueId();
  std::string GetSessionId();
  
  // Set the URL of the MIDAS server. eg: http://localhost/Midas/midas/
  void SetServerUrl(const char* baseurl);
  // Return the server url
  const char* GetServerUrl();
   
  // Set the input mode for the uploaded data.
  void SetInputMode(IO_MODE input_mode);
  
  void SetUploadUniqueId(const std::string &value);

  // Register the callback function to be notified of progress event during data
  // transfers.
  void SetProgressCallback(curl_progress_callback fprogress, void * fprogress_data);

  // API action
  bool Execute(const char* url, 
               const char* post_data = NULL,
               const char* authentication = "");

    /// Download a file from the server.
    /// @param data Filename of the downloaded data or string reference for direct storage.
    /// @param url URL of the file. This string will be appended at the end of the server url that
    /// must be previously set.
    /// @param output_mode Type of output: file (name) or buffer (string).
    bool Download(const std::string &data, std::string url, IO_MODE output_mode,
                  curl_progress_callback fprogress = NULL, void * fprogress_data = NULL,
                  const char* authentication = "");

    /// Upload file to the midas server
    bool UploadPost(const char* filename, std::string url, curl_progress_callback fprogress=NULL,
                void * fprogress_data=NULL,
                const char* authentication = "");
    bool Upload(const std::string &data, std::string url, curl_progress_callback fprogress=NULL,
                void * fprogress_data=NULL,
                const char* authentication = "");

    /// Search in the database items and bitstreams containing the keyword 'keyword'.
    /// The server URL must have been set using setServerUrl().
    /// '+' means AND, ' ' means OR, '-' means NOT. Extra-space or fancy characters are removed from
    /// the input string.
    /// @param keyword String containing all the desired keyword
    /// @return A string containing the XML file answered by the server.
    std::string SearchKeywords(const std::string &keywords);
    /// Ask for informations about an item: title, abstract, list of bitstreams
    /// @param itemId Identifier of the item on the server.
    /// @return A string containing the XML file answered by the server.
    std::string SearchItem(int itemId);

 /** Set cURL in verbose mode */
 void SetVerbose(bool verbose);

 /** For debug purposes */
 void PrintStatus();
 
 /** Return the file length */
 unsigned long GetFileLength(const char* filename);

 /** Set the XML parser */
 void SetXMLParser(mws::RestXMLParser* parser);
 
 /** XML parsing */
 bool Parse(const char* buffer,unsigned long length);
   
 /** Used by cURL */
 IO_MODE GetOutputMode();
     
protected:

  // memory mgt
  void ResetOutputBuffer();

  // curl wrapper
  bool PerformCurl();
  bool SetCurlOptions(const char* url,
                      const char* authentication = "");

  // API utilities functions :
  // Format the keyword string in order to be send trough an URL (quote, space, ...)
  std::string FormatKeywords(const std::string &keywords);
  
  // Curl
  bool m_Initialized;
  CURLcode m_CurlCode;
  CURL * m_cURL;
  bool m_Verbose;
  
  // server
  std::string m_ServerUrl;
  // transfer progress
  curl_progress_callback fprogress;
  void * fprogress_data;
  // upload
  std::string upload_unique_id;
  // query output
  std::string  output_buffer;
  std::ofstream* output_filestream;
  IO_MODE m_OutputMode;
  // query input
  std::istream * input_stream;
  IO_MODE input_mode;
  // Parser
  mws::RestXMLParser* m_XMLParser;
  
};


// callback function
extern "C" {
  static size_t Curl_output_function(void *ptr, size_t size, size_t nitems, void *userp);
  static size_t Curl_read_function(void *bufptr, size_t size, size_t nitems, void *userp);
}

} // end namespace



#endif //__RestAPI_H
