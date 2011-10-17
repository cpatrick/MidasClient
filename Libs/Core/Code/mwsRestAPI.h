/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __RestAPI_H
#define __RestAPI_H

#include "midasStandardIncludes.h"

#include <QObject>

// How long to wait for an execute before dying
#define REST_EXECUTE_TIMEOUT 20

class midasProgressReporter;

namespace mws {

class RestResponseParser;

class RestAPI : public QObject
{
  Q_OBJECT
public:
    
  // Constructor/Destructor
  RestAPI();
  ~RestAPI();
  
  // Set the URL of the MIDAS server. eg: http://localhost/Midas/midas/
  void SetServerUrl(const char* baseurl);
  // Return the server url
  const char* GetServerUrl();

  // Perform a web API call using HTTP GET
  bool Execute(const char* url, RestResponseParser* parser,
               const char* post_data = NULL);

  /// Download a file from the server.
  /// @param dest filename (where to download the data)
  /// @param url URL of the file. This string will be appended at the end of the server url that
  /// @param offset if resuming, this is the amount already completed
  /// must be previously set.
  bool Download(const std::string& filename, const std::string& url,
                int64 offset = 0);

  /**
   * Upload @filename to @url using HTTP PUT.
   * Supports files over 4GB.
   */
  bool Upload(const std::string& filename, const std::string& url,
              RestResponseParser* parser, int64 offset = 0);

  void SetProgressReporter(midasProgressReporter* progress);

  bool ShouldCancel();

signals:
  void Canceled();

public slots:
  void Cancel();

protected slots:
  void TransferProgress(qint64, qint64);

protected:
  bool                   m_Cancel;
  std::string            m_ServerUrl;
  midasProgressReporter* m_Progress;
};

} // end namespace



#endif //__RestAPI_H
