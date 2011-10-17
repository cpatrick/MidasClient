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
#include "mwsRestResponseParser.h"

#include <QScriptEngine>
#include <QScriptValueIterator>

namespace mws
{
/** Custom JSON parser */
class SearchResponseParser : public RestResponseParser
{
public:
   
  SearchResponseParser()
    {
    m_Results = NULL;
    };
    
  ~SearchResponseParser() 
    {
    };

  virtual bool Parse(const QString& response)
    {
    if(!RestResponseParser::Parse(response))
      {
      return false;
      }

    QScriptEngine engine;
    QScriptValue data = engine.evaluate(response).property("data");

    if(data.property("communities").isArray())
      {
      QScriptValueIterator comms(data.property("communities"));
      while(comms.hasNext())
        {
        comms.next();
        mdo::Community* comm = new mdo::Community();
        comm->SetId(comms.value().property("community_id").toInt32());
        comm->SetUuid(comms.value().property("uuid").toString().toStdString().c_str());
        comm->SetName(comms.value().property("community_name").toString().toStdString().c_str());
        m_Results->push_back(comm);
        }
      }
    if(data.property("collections").isArray())
      {
      QScriptValueIterator colls(data.property("collections"));
      while(colls.hasNext())
        {
        colls.next();
        mdo::Collection* coll = new mdo::Collection();
        coll->SetId(colls.value().property("collection_id").toInt32());
        coll->SetUuid(colls.value().property("uuid").toString().toStdString().c_str());
        coll->SetName(colls.value().property("collection_name").toString().toStdString().c_str());
        m_Results->push_back(coll);
        }
      }
    if(data.property("items").isArray())
      {
      QScriptValueIterator items(data.property("items"));
      while(items.hasNext())
        {
        items.next();
        mdo::Item* item = new mdo::Item();
        item->SetId(items.value().property("item_id").toInt32());
        item->SetUuid(items.value().property("uuid").toString().toStdString().c_str());
        item->SetTitle(items.value().property("item_name").toString().toStdString().c_str());
        m_Results->push_back(item);
        }
      }
    if(data.property("bitstreams").isArray())
      {
      QScriptValueIterator bitstreams(data.property("bitstreams"));
      while(bitstreams.hasNext())
        {
        bitstreams.next();
        mdo::Bitstream* bitstream = new mdo::Bitstream();
        bitstream->SetId(bitstreams.value().property("bitstream_id").toInt32());
        bitstream->SetUuid(bitstreams.value().property("uuid").toString().toStdString().c_str());
        bitstream->SetName(bitstreams.value().property("bitstream_name").toString().toStdString().c_str());
        m_Results->push_back(bitstream);
        }
      }
    return true;
    }
  
  /** Set the results object */
  void SetObject(std::vector<mdo::Object*>* results)
    {
    m_Results = results;
    }

protected:
  std::vector<mdo::Object*>* m_Results;
};

std::vector<mdo::Object*> Search::SearchServer(std::vector<std::string> tokens)
{
  std::vector<mdo::Object*> results;
  SearchResponseParser parser;
  parser.SetObject(&results);
  
  std::string fields = "midas.resources.search&search=";

  bool space = false;
  for(std::vector<std::string>::iterator i = tokens.begin();
      i != tokens.end(); ++i)
    {
    fields += *i;
    if(space)
      {
      fields += " ";
      }
    space = true;
    }
  mws::WebAPI::Instance()->Execute(fields.c_str(), &parser);
  return results;
}
} //end namespace
