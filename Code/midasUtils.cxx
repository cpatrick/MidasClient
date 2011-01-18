/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "midasUtils.h"
#include "mdsSQLiteDatabase.h"
#include "mwsWebAPI.h"
#include "mwsRestXMLParser.h"
#include "mdoObject.h"
#include "mdoBitstream.h"
#include "mdoCollection.h"
#include "mdoCommunity.h"
#include "mdoItem.h"
#include "midasStandardIncludes.h"
#include "midasTableDefs.h"
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <kwsys/MD5.h>

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
std::string midasUtils::EscapeForURL(std::string s)
{
  kwsys::SystemTools::ReplaceString(s, "%", "%25");
  kwsys::SystemTools::ReplaceString(s, " ", "%20");
  kwsys::SystemTools::ReplaceString(s, "&", "%26");
  kwsys::SystemTools::ReplaceString(s, "=", "%3D");
  kwsys::SystemTools::ReplaceString(s, "?", "%3F");
  kwsys::SystemTools::ReplaceString(s, ",", "%2C");
  kwsys::SystemTools::ReplaceString(s, "+", "%2B");
  kwsys::SystemTools::ReplaceString(s, "$", "%24");
  kwsys::SystemTools::ReplaceString(s, ":", "%3A");
  kwsys::SystemTools::ReplaceString(s, ";", "%3B");
  kwsys::SystemTools::ReplaceString(s, "/", "%2F");
  kwsys::SystemTools::ReplaceString(s, "@", "%40");
  kwsys::SystemTools::ReplaceString(s, "\"", "%22");
  kwsys::SystemTools::ReplaceString(s, "<", "%3C");
  kwsys::SystemTools::ReplaceString(s, ">", "%3E");
  kwsys::SystemTools::ReplaceString(s, "#", "%23");

  return s;
}

//-------------------------------------------------------------------
std::string midasUtils::EscapeForSQL(std::string s)
{
  kwsys::SystemTools::ReplaceString(s, "'", "''");

  return s;
}

//-------------------------------------------------------------------
std::string midasUtils::EscapeName(std::string name)
{
  kwsys::SystemTools::ReplaceString(name, " ", "_");
  kwsys::SystemTools::ReplaceString(name, "\t", "_");

  return name;
}

int64 midasUtils::GetFileLength(const char* filename)
{
#if defined(_MSC_VER) || defined(__BORLANDC__)
  struct __stat64 fs;
  if(_stat64(filename, &fs) != 0)
    {
    return 0;
    }
  else
    {
    return fs.st_size;
    }
#else
  struct stat64 fs;
  if (stat64(filename, &fs) != 0)
    {
    return 0;
    }
  else
    {
    return fs.st_size;
    }
#endif
}

//-------------------------------------------------------------------
std::string midasUtils::BytesToString(double bytes)
{
  std::stringstream text;
  double amount;
  std::string unit;

  if(bytes < 1024)
    {
    amount = bytes;
    unit = "B";
    }
  else if(bytes < 1024*1024)
    {
    amount = bytes / (1024.0);
    unit = "KB";
    }
  else if(bytes < 1024*1024*1024)
    {
    amount = bytes / (1024.0*1024.0);
    unit = "MB";
    }
  else
    {
    amount = bytes / (1024.0*1024.0*1024.0);
    unit = "GB";
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
  text << " " << unit;
  return text.str();
}

std::string midasUtils::FormatTimeString(double seconds)
{
  int days = (int)(seconds / DAY_LEN);
  seconds -= days * DAY_LEN;
  int hours = (int)(seconds / HR_LEN);
  seconds -= hours * HR_LEN;
  int minutes = (int)(seconds / MIN_LEN);
  seconds -= minutes * MIN_LEN;
  int sec = (int)seconds;
  
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
bool midasUtils::IsDatabaseValid(std::string path)
{
  if(!kwsys::SystemTools::FileExists(path.c_str(), true))
    {
    return false;
    }

  mds::SQLiteDatabase db;
  bool result = db.Open(path.c_str());
  result &= db.ExecuteQuery("SELECT * FROM dirty_resource");
  while(db.GetNextRow());
  result &= db.Close();
  return result;
}

//-------------------------------------------------------------------
bool midasUtils::CreateNewDatabase(std::string path)
{
  mds::SQLiteDatabase db;
  if(!db.Open(path.c_str()))
    {
    return false;
    }
  
  std::vector<std::string> lines;
  kwsys::SystemTools::Split(mds::getTableDefs(), lines, ';');

  bool success = true;
  for(std::vector<std::string>::iterator i = lines.begin();
      i != lines.end(); ++i)
    {
    std::string query = *i;
    midasUtils::StringTrim(query);
    if(query != "")
      {
      success &= db.ExecuteQuery(query.c_str());
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
  if(kwsys::SystemTools::StringEndsWith(str.c_str(), "/")
     && str.length() > 1)
    {
    str = str.substr(0, str.find_last_of("/"));
    }
  else if(kwsys::SystemTools::StringEndsWith(str.c_str(), "\\")
          && str.length() > 1)
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

void midasUtils::Tokenize(const std::string& str,
                          std::vector<std::string>& tokens,
                          const std::string& delimiters,
                          bool trimspaces)
{
  // Skip delimiters at beginning.
  std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  // Find first "non-delimiter".
  std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

  while (std::string::npos != pos || std::string::npos != lastPos)
    {
    std::string token = str.substr(lastPos, pos - lastPos); 
    if ( trimspaces ) { midasUtils::StringTrim( token ); }
    // Found a token, add it to the vector.
    tokens.push_back( token );
    // Skip delimiters.  Note the "not_of"
    lastPos = str.find_first_not_of(delimiters, pos);
    // Find next "non-delimiter"
    pos = str.find_first_of(delimiters, lastPos);
    }
}

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

std::string midasUtils::CreateDefaultAPIKey(const std::string& email,
                                            const std::string& password,
                                            const std::string& appName)
{
  std::string digest = email + password + appName;
  return midasUtils::ComputeStringMD5(digest.c_str());
}

std::string midasUtils::ComputeStringMD5(const char* input)
{
  char md5out[32];
  kwsysMD5* md5 = kwsysMD5_New();
  kwsysMD5_Initialize(md5);
  kwsysMD5_Append(md5, reinterpret_cast<unsigned char const*>(input), -1);
  kwsysMD5_FinalizeHex(md5, md5out);
  kwsysMD5_Delete(md5);
  return std::string(md5out, 32);
}
