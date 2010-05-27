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
#include <time.h>

#define UUID_LENGTH 45

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

mdo::Object* midasUtils::FetchByUuid(std::string uuid)
{
  mdo::Object* object = NULL;
  std::stringstream fields;
  fields << "midas.resource.get?uuid=" << uuid;

  std::string idStr, typeStr;
  mws::RestXMLParser parser;
  parser.AddTag("/rsp/id", idStr);
  parser.AddTag("/rsp/type", typeStr);
  
  mws::WebAPI::Instance()->GetRestAPI()->SetXMLParser(&parser);
  if(mws::WebAPI::Instance()->Execute(fields.str().c_str()))
    {
    int id = atoi(idStr.c_str());
    int type = atoi(typeStr.c_str());

    if(id == 0) return NULL;

    mdo::Community* comm = NULL;
    mdo::Collection* coll = NULL;
    mdo::Item* item = NULL;
    mdo::Bitstream* bitstream = NULL;

    switch(type)
      {
      case midasResourceType::COMMUNITY:
        comm = new mdo::Community();
        comm->SetId(id);
        object = reinterpret_cast<mdo::Object*>(comm);
        break;
      case midasResourceType::COLLECTION:
        coll = new mdo::Collection();
        coll->SetId(id);
        object = reinterpret_cast<mdo::Object*>(coll);
        break;
      case midasResourceType::ITEM:
        item = new mdo::Item();
        item->SetId(id);
        object = reinterpret_cast<mdo::Object*>(item);
        break;
      case midasResourceType::BITSTREAM:
        bitstream = new mdo::Bitstream();
        bitstream->SetId(id);
        object = reinterpret_cast<mdo::Object*>(bitstream);
        break;
      default:
        break;
      }
    }

  return object;
}
