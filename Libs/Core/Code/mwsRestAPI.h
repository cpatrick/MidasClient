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


#ifndef __RestAPI_H
#define __RestAPI_H

#include "midasStandardIncludes.h"

#include <QObject>

// How long to wait for an execute before dying
#define REST_EXECUTE_TIMEOUT 20

class midasProgressReporter;

namespace mws
{

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
  const char * GetServerUrl();

  // Perform a web API call using HTTP GET
  bool Execute(const char* url, RestResponseParser* parser, const char* post_data = NULL);

  // / Download a file from the server.
  // / @param dest filename (where to download the data)
  // / @param url URL of the file. This string will be appended at the end of
  // the server url that
  // / @param offset if resuming, this is the amount already completed
  // / must be previously set.
  bool Download(const std::string& filename, const std::string& url, int64 offset = 0);

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

#endif // __RestAPI_H
