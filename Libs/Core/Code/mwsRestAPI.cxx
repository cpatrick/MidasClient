/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mwsRestAPI.h"

#include "midasStandardIncludes.h"
#include "mwsRestResponseParser.h"
#include "midasProgressReporter.h"
#include "midasUtils.h"

#include <QUrl>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>

namespace mws {

RestAPI::RestAPI()
{
  m_Progress = NULL;
  m_Cancel = false;
}

RestAPI::~RestAPI()
{
}

void RestAPI::SetProgressReporter(midasProgressReporter* progress)
{
  m_Progress = progress;
}

const char* RestAPI::GetServerUrl()
{
  return m_ServerUrl.c_str();
}

//-------------------------------------------------------------------
void RestAPI::SetServerUrl(const char* baseurl)
{
  std::string temp(baseurl);
  // start with 'http://' or 'https://' :
  if(temp.substr(0, 7).compare("http://") != 0 &&
     temp.substr(0, 8).compare("https://") != 0)
    {
    temp.insert(0, "http://");
    }
  if(temp.substr(temp.length() - 9).compare("/api/rest") != 0 &&
     temp.substr(temp.length() - 8).compare("?method=") != 0)
    {
    temp.append("/api/rest?method=");
    }
  else if(temp.substr(temp.length() - 9).compare("/api/rest") == 0)
    {
    temp.append("?method=");
    }

  m_ServerUrl = temp;
}

//-------------------------------------------------------------------
bool RestAPI::Execute(const char* urlstr, RestResponseParser* parser,
                      const char* post_data)
{
  m_Cancel = false;

  std::string URL(urlstr);
  if(!m_ServerUrl.empty())
    {
    URL = m_ServerUrl + URL;
    }

  QNetworkRequest req;
  QUrl url(URL.c_str());
  req.setUrl(url);

  QNetworkAccessManager network;
  QNetworkReply* reply;
  if(post_data == NULL)
    {
    reply = network.get(req);
    }
  else
    {
    reply = network.post(req, QByteArray(post_data));
    }

  QString response("(");
  QEventLoop loop;
  connect(reply, SIGNAL(readyRead()), &loop, SLOT(quit()));
  connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));

  while(reply->isRunning())
    {
    loop.exec(); //wait for data from the reply
    response += reply->readAll();
    }
  response += reply->readAll();
  response += ")";

  reply->deleteLater();
  return parser->Parse(response);
}

//-------------------------------------------------------------------
bool RestAPI::Download(const std::string& filename, const std::string& urlstr,
                       int64 offset)
{
  m_Cancel = false;
  std::string URL(urlstr);
  if(!m_ServerUrl.empty())
    {
    URL = m_ServerUrl + URL; 
    }

  QFile file;
  file.setFileName(filename.c_str());
  QIODevice::OpenMode mode = offset == 0 ?
    QIODevice::WriteOnly : QIODevice::Append;
  if(!file.open(mode))
    {
    return false;
    }

  QNetworkRequest req;
  QUrl url(URL.c_str());
  req.setUrl(url);

  QNetworkAccessManager network;
  QNetworkReply* reply = network.get(req);

  QEventLoop loop;
  connect(reply, SIGNAL(readyRead()), &loop, SLOT(quit()));
  connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
  connect(this, SIGNAL(Canceled()), &loop, SLOT(quit()));

  if(m_Progress)
    {
    connect(reply, SIGNAL(downloadProgress(qint64, qint64)),
            this, SLOT(TransferProgress(qint64, qint64)));
    }

  while(reply->isRunning())
    {
    loop.exec(); //wait for data from the reply

    if(m_Cancel)
      {
      reply->abort();
      reply->deleteLater();
      file.close();
      return false;
      }
    file.write(reply->readAll());
    }
  file.write(reply->readAll());

  reply->deleteLater();
  file.close();
  return true;
 }

//-------------------------------------------------------------------
bool RestAPI::Upload(const std::string &filename, const std::string& urlstr,
                     RestResponseParser* parser, int64 offset)
{
  m_Cancel = false;
  std::string URL(urlstr);
  if(!m_ServerUrl.empty())
    {
    URL = m_ServerUrl + URL;
    }

  QFile file;
  file.setFileName(filename.c_str());
  if(!file.open(QIODevice::ReadOnly))
    {
    return false;
    }
  if(offset > 0 && !file.seek(offset)) //seek to offset
    {
    return false;
    }

  QNetworkRequest req;
  QUrl url(URL.c_str());
  req.setUrl(url);

  QNetworkAccessManager network;
  QNetworkReply* reply = network.put(req, &file);

  QEventLoop loop;
  connect(reply, SIGNAL(uploadProgress(qint64, qint64)), &loop, SLOT(quit()));
  connect(reply, SIGNAL(readyRead()), &loop, SLOT(quit()));
  connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
  if(m_Progress)
    {
    connect(reply, SIGNAL(uploadProgress(qint64, qint64)),
            this, SLOT(TransferProgress(qint64, qint64)));
    }

  QString response("(");
  while(reply->isRunning())
    {
    loop.exec(); //wait for data from the reply
    if(m_Cancel)
      {
      reply->abort();
      reply->deleteLater();
      file.close();
      return false;
      }
    response += reply->readAll();
    }
  response += reply->readAll();
  response += ")";

  reply->deleteLater();
  file.close();
  return parser->Parse(response);
}

//-------------------------------------------------------------------
void RestAPI::Cancel()
{
  m_Cancel = true;
  emit Canceled();
}

//-------------------------------------------------------------------
bool RestAPI::ShouldCancel()
{
  return m_Cancel;
}

//-------------------------------------------------------------------
void RestAPI::TransferProgress(qint64 current, qint64 total)
{
  if(m_Progress && current > 0 && total > 0)
    {
    m_Progress->UpdateProgress(
      static_cast<double>(current), static_cast<double>(total));
    }
}

} // end namespace
