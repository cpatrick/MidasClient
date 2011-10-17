/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "midasUtils.h"
#include "mdsSQLiteDatabase.h"
#include "mdsUpgradeHandler.h"
#include "mwsWebAPI.h"
#include "mwsRestResponseParser.h"
#include "mdoObject.h"
#include "mdoBitstream.h"
#include "mdoCollection.h"
#include "mdoCommunity.h"
#include "mdoItem.h"
#include "mdoVersion.h"
#include "midasStandardIncludes.h"
#include "mdsTableDefs.h"
#include "m3dsTableDefs.h"

#include <QString>
#include <QStringList>
#include <QUrl>
#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDateTime>

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#define UUID_LENGTH 45

#define DAY_LEN (60.0*60.0*24.0)
#define HR_LEN (60.0*60.0)
#define MIN_LEN (60.0)

//-------------------------------------------------------------------
std::string midasUtils::GenerateUUID()
{
  // Generate a random number for the uuid.
  std::stringstream randomchars;
  for(unsigned int i = 0; i < UUID_LENGTH; i++)
    {
    int nextDigit = rand() % 16;
    randomchars << std::hex << nextDigit;
    }
  return randomchars.str();
}

//-------------------------------------------------------------------
std::string midasUtils::EscapeForSQL(std::string s)
{
  QString string = s.c_str();
  string = string.replace(QChar('\''), "''");

  return string.toStdString();
}

//-------------------------------------------------------------------
std::string midasUtils::EscapeName(std::string name)
{
  QString string = name.c_str();
  string = string.replace(QChar(' '), "_");
  string = string.replace(QChar('\t'), "_");

  return string.toStdString();
}

int64 midasUtils::GetFileLength(const char* filename)
{
  QFileInfo fileInfo(filename);
  return fileInfo.size();
}

//-------------------------------------------------------------------
std::string midasUtils::BytesToString(double bytes, const std::string& unit)
{
  std::stringstream text;
  double amount;
  std::string order;

  if(bytes < 1024)
    {
    amount = bytes;
    order = "";
    }
  else if(bytes < 1024*1024)
    {
    amount = bytes / (1024.0);
    order = "K";
    }
  else if(bytes < 1024*1024*1024)
    {
    amount = bytes / (1024.0*1024.0);
    order = "M";
    }
  else
    {
    amount = bytes / (1024.0*1024.0*1024.0);
    order = "G";
    }

  if(amount >= 1000)
    {
    text.precision(0);
    text << std::fixed << amount;
    }
  else
    {
    text.precision(3);
    text << amount;
    }
  text << " " << order << unit;
  return text.str();
}

std::string midasUtils::FormatTimeString(double seconds)
{
  int days = static_cast<int>(seconds / DAY_LEN);
  seconds -= days * DAY_LEN;
  int hours = static_cast<int>(seconds / HR_LEN);
  seconds -= hours * HR_LEN;
  int minutes = static_cast<int>(seconds / MIN_LEN);
  seconds -= minutes * MIN_LEN;
  int sec = static_cast<int>(seconds);
  
  std::stringstream text;
  if(days > 0)
    {
    text << days << (days > 1 ? " days " : " day ");
    }
  if(hours > 0)
    {
    text << hours << (hours > 1 ? " hours " : " hour ");
    }
  if(minutes > 0)
    {
    text << minutes << (minutes > 1 ? " minutes " : " minute ");
    }
  if(sec > 0)
    {
    text << sec << (sec > 1 ? " seconds " : " second ");
    }
  return text.str();
}

//-------------------------------------------------------------------
bool midasUtils::IsDatabaseValid(std::string path,
                                 mds::UpgradeHandler* handler)
{
  bool ok = true;
  QFileInfo fileInfo(path.c_str());
  if(!fileInfo.isFile())
    {
    return false;
    }

  mds::SQLiteDatabase db;
  if(!db.Open(path.c_str()))
    {
    return false;
    }
  if(!db.ExecuteQuery("SELECT major, minor, patch FROM version "
                      "WHERE name='MIDASClient'"))
    {
    db.Close();
    return false;
    }
  mdo::Version dbVersion;
  while(db.GetNextRow())
    {
    dbVersion.Major = db.GetValueAsInt(0);
    dbVersion.Minor = db.GetValueAsInt(1);
    dbVersion.Patch = db.GetValueAsInt(2);
    }

  if(dbVersion.Major >= 3)
    {
    //TODO upgrade logic for midas 3 schema goes here
    }
  else
    {
    // Upgrade logic for midas 2 schema goes here
    if(!db.ExecuteQuery("SELECT id FROM partial_upload LIMIT 1"))
      {
      mdo::Version productVersion(MIDAS_CLIENT_VERSION_MAJOR,
                                  MIDAS_CLIENT_VERSION_MINOR,
                                  MIDAS_CLIENT_VERSION_PATCH);
      ok &= handler->Upgrade(path, dbVersion, productVersion);
      }
    }

  if(!db.Close())
    {
    return false;
    }
  return ok;
}

//-------------------------------------------------------------------
bool midasUtils::CreateNewDatabase(std::string path, bool midas3)
{
  mds::SQLiteDatabase db;
  if(!db.Open(path.c_str()))
    {
    return false;
    }

  QString tableDefs = midas3 ? m3dsUpgrade::getTableDefs() : mdsUpgrade::getTableDefs();
  QStringList lines = tableDefs.split(";");

  bool success = true;
  QString line;
  foreach(line, lines)
    {
    line = line.trimmed();
    if(line != "")
      {
      success &= db.ExecuteQuery(line.toStdString().c_str());
      }
    }

  success &= db.Close();
  return success;
}

//-------------------------------------------------------------------
void midasUtils::StringTrim(std::string& str)
{
  std::string::size_type pos = str.find_last_not_of(' ');
  if(pos != std::string::npos) 
    {
    str.erase(pos + 1);
    pos = str.find_first_not_of(' ');
    if(pos != std::string::npos) str.erase(0, pos);
    }
  else 
    {
    str.erase(str.begin(), str.end());
    }
}

//-------------------------------------------------------------------
std::string midasUtils::TrimTrailingSlash(std::string str)
{
  QString string = str.c_str();
  if(string.endsWith("/") && string.size() > 1)
    {
    str = str.substr(0, str.find_last_of("/"));
    }
  else if(string.endsWith("\\") && string.size() > 1)
    {
    str = str.substr(0, str.find_last_of("\\"));
    }
  return str;
}

//-------------------------------------------------------------------
std::string midasUtils::GetTypeName(int type)
{
  switch(type)
    {
    case midasResourceType::COMMUNITY:
      return "Community";
    case midasResourceType::COLLECTION:
      return "Collection";
    case midasResourceType::ITEM:
      return "Item";
    case midasResourceType::BITSTREAM:
      return "Bitstream";
    default:
      return "Unknown";
    }
}

//-------------------------------------------------------------------
int midasUtils::GetParentType(int type)
{
  switch(type)
    {
    case midasResourceType::COMMUNITY:
      return type;
    case midasResourceType::COLLECTION:
      return midasResourceType::COMMUNITY;
    case midasResourceType::ITEM:
      return midasResourceType::COLLECTION;
    case midasResourceType::BITSTREAM:
      return midasResourceType::ITEM;
    default:
      return midasResourceType::TYPE_ERROR;
    }
}

//-------------------------------------------------------------------
void midasUtils::Tokenize(const std::string& str,
                          std::vector<std::string>& tokens,
                          const std::string& delimiters,
                          bool trimspaces)
{
  // Skip delimiters at beginning.
  std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

  while (std::string::npos != pos || std::string::npos != lastPos)
    {
    std::string token = str.substr(lastPos, pos - lastPos); 
    if ( trimspaces ) { midasUtils::StringTrim( token ); }
    tokens.push_back( token );
    lastPos = str.find_first_not_of(delimiters, pos);
    pos = str.find_first_of(delimiters, lastPos);
    }
}

//-------------------------------------------------------------------
double midasUtils::StringToDouble(const std::string& num)
{
  std::istringstream i(num);
  double x;
  if(!(i>>x))
    {
    return 0;
    }
  return x;
}

//-------------------------------------------------------------------
std::string midasUtils::CreateDefaultAPIKey(const std::string& email,
                                            const std::string& password,
                                            const std::string& appName)
{
  std::string digest = email +
    midasUtils::ComputeStringMD5(password.c_str()) +
    appName;
  return midasUtils::ComputeStringMD5(digest.c_str());
}

//-------------------------------------------------------------------
std::string midasUtils::ComputeStringMD5(const char* input)
{
  std::string str(input);
  QCryptographicHash md5(QCryptographicHash::Md5);
  md5.addData(input, str.length());

  return md5.result().toHex().constData();
}

//-------------------------------------------------------------------
std::string midasUtils::ComputeFileChecksum(const std::string& filename,
                                            QCryptographicHash::Algorithm alg)
{
  QCryptographicHash hash(alg);
  QFile file(filename.c_str());
  if(!file.open(QIODevice::ReadOnly))
    {
    return "";
    }

  char data[8192];
  qint64 len;
  while((len = file.read(data, 8192)) > 0)
    {
    hash.addData(data, len);
    }
  file.close();
  return hash.result().toHex().constData();
}

//-------------------------------------------------------------------
std::string midasUtils::ComputeStringSHA1(const char* input)
{
  std::string str(input);
  QCryptographicHash sha1(QCryptographicHash::Sha1);
  sha1.addData(input, str.length());

  return sha1.result().constData();
}

//-------------------------------------------------------------------
bool midasUtils::RenameFile(const char* oldname, const char* newname)
{
  return QFile::rename(oldname, newname);
}

//-------------------------------------------------------------------
bool midasUtils::ValidateBitstreamName(const std::string& name)
{
  if(name == "" || name.find('/') != std::string::npos ||
     name.find('\\') != std::string::npos ||
     name.find(':') != std::string::npos ||
     name.find('*') != std::string::npos ||
     name.find('?') != std::string::npos ||
     name.find('|') != std::string::npos ||
     name.find('<') != std::string::npos ||
     name.find('>') != std::string::npos ||
     name.find('"') != std::string::npos)
    {
    return false;
    }
  return true;
}

bool midasUtils::RemoveDir(const std::string& dirName)
{
  bool result = true;
  QDir dir(dirName.c_str());
 
  if (dir.exists())
    {
    Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst))
      {
      if (info.isDir())
        {
        result = midasUtils::RemoveDir(info.absoluteFilePath().toStdString());
        }
      else 
        {
        result = QFile::remove(info.absoluteFilePath());
        }

      if (!result)
        {
        return result;
        }
      }
    result = dir.rmdir(dirName.c_str());
    }

  return result;
}

/**
 * Return the epoch time in seconds with millisecond resolution
 */
double midasUtils::CurrentTime()
{
  QDateTime now = QDateTime::currentDateTime();
  double seconds = now.toTime_t();
  
  seconds += (now.time().msec() / 1000.0);

  return seconds;
}
