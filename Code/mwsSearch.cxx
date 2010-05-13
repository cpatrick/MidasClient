/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "mwsSearch.h"
#include "mdoObject.h"
#include "mdoCommunity.h"
#include "mdoCollection.h"
#include "mdoItem.h"
#include "mdoBitstream.h"
#include "midasUtils.h"
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <iostream>
#include "mwsWebAPI.h"
#include "mwsRestXMLParser.h"

namespace mws
{
/** Custom XML parser */
class SearchXMLParser : public RestXMLParser
{
public:
   
  SearchXMLParser()
    {
    m_Results = NULL;
    m_Status = NONE;
    };
    
  ~SearchXMLParser() 
    {
    };  

  // Callback function -- called from XML parser with start-of-element
  // information.
  virtual void StartElement(const char * name,const char **atts)
    {
    RestXMLParser::StartElement(name,atts);
    m_CurrentValue = "";

    if(!strcmp(name,"bitstreams"))
      {
      m_Status = BITSTREAMS;
      }
    else if(!strcmp(name,"collections"))
      {
      m_Status = COLLECTIONS;
      }
    else if(!strcmp(name,"communities"))
      {
      m_Status = COMMUNITIES;
      }
    else if(!strcmp(name,"items"))
      {
      m_Status = ITEMS;
      }
    else if(!strcmp(name,"data"))
      {
      m_Id = 0;
      m_Uuid = "";
      m_Name = "";
      }
    }

  // Callback function -- called from XML parser when ending tag
  // encountered
  virtual void EndElement(const char *name)
    {
    if(!strcmp(name,"uuid"))
      {
      m_Uuid = m_CurrentValue;
      }
    switch(m_Status)
      {
      case COMMUNITIES:
        if(!strcmp(name,"community_id"))
          {
          m_Id = atoi(m_CurrentValue.c_str());
          }
        else if(!strcmp(name,"name"))
          {
          m_Name = m_CurrentValue;
          }
        else if(!strcmp(name,"data"))
          {
          mdo::Community* comm = new mdo::Community();
          comm->SetId(m_Id);
          comm->SetUuid(m_Uuid.c_str());
          comm->SetName(m_Name.c_str());
          m_Results->push_back(comm);
          }
        break;
      case COLLECTIONS:
        if(!strcmp(name,"collection_id"))
          {
          m_Id = atoi(m_CurrentValue.c_str());
          }
        else if(!strcmp(name,"name"))
          {
          m_Name = m_CurrentValue;
          }
        else if(!strcmp(name,"data"))
          {
          mdo::Collection* coll = new mdo::Collection();
          coll->SetId(m_Id);
          coll->SetUuid(m_Uuid.c_str());
          coll->SetName(m_Name.c_str());
          m_Results->push_back(coll);
          }
        break;
      case ITEMS:
        if(!strcmp(name,"item_id"))
          {
          m_Id = atoi(m_CurrentValue.c_str());
          }
        else if(!strcmp(name,"title"))
          {
          m_Name = m_CurrentValue;
          }
        else if(!strcmp(name,"data"))
          {
          mdo::Item* item = new mdo::Item();
          item->SetId(m_Id);
          item->SetUuid(m_Uuid.c_str());
          item->SetTitle(m_Name.c_str());
          m_Results->push_back(item);
          }
        break;
      case BITSTREAMS:
        if(!strcmp(name,"bitstream_id"))
          {
          m_Id = atoi(m_CurrentValue.c_str());
          }
        else if(!strcmp(name,"name"))
          {
          m_Name = m_CurrentValue;
          }
        else if(!strcmp(name,"data"))
          {
          mdo::Bitstream* bs = new mdo::Bitstream();
          bs->SetId(m_Id);
          bs->SetUuid(m_Uuid.c_str());
          bs->SetName(m_Name.c_str());
          m_Results->push_back(bs);
          }
        break;
      default:
        break;
      }
    RestXMLParser::EndElement(name);
    }
    
  // Callback function -- called from XML parser with the character data
  // for an XML element
  virtual void CharacterDataHandler(const char *inData, int inLength)
    {
    RestXMLParser::CharacterDataHandler(inData,inLength);
    m_CurrentValue.append(inData,inLength);
    }
  
  /** Set the NewResources object */
  void SetObject(std::vector<mdo::Object*>* results)
    {
    m_Results = results;
    }
  
protected:
  enum ParseStatus
    {
    NONE,
    BITSTREAMS,
    ITEMS,
    COLLECTIONS,
    COMMUNITIES
    };

  ParseStatus                m_Status;
  std::string                m_CurrentValue;
  int                        m_Id;
  std::string                m_Uuid;
  std::string                m_Name;
  std::vector<mdo::Object*>* m_Results;
};

std::vector<mdo::Object*> Search::SearchServer(std::vector<std::string> tokens)
{
  std::vector<mdo::Object*> results;
  SearchXMLParser parser;
  parser.SetObject(&results);
  mws::WebAPI::Instance()->GetRestAPI()->SetXMLParser(&parser);
  
  std::string fields = "midas.resources.search?search=";

  bool space = false;
  for(std::vector<std::string>::iterator i = tokens.begin();
      i != tokens.end(); ++i)
    {
    fields += midasUtils::EscapeForURL(*i);
    if(space)
      {
      fields += midasUtils::EscapeForURL(" ");
      }
    space = true;
    }
  mws::WebAPI::Instance()->Execute(fields.c_str());
  return results;
}

std::vector<mdo::Object*> Search::SearchClient(std::vector<std::string> tokens)
{
  //Not implemented yet
  std::vector<mdo::Object*> results;
  return results;
}

} //end namespace