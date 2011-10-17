/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mwsItem.h"
#include "mdoCollection.h"
#include "mwsCollection.h"
#include <sstream>
#include <iostream>
#include "mwsRestResponseParser.h"

#include "mdoItem.h"
#include "mdoBitstream.h"
#include "midasUtils.h"

#include <QScriptEngine>
#include <QScriptValueIterator>

namespace mws{

/** Custom Response parser */
class ItemResponseParser : public RestResponseParser
{
public:
   
  ItemResponseParser()
    {
    m_Item = NULL;
    }
    
  ~ItemResponseParser() 
    {
    }

  virtual bool Parse(const QString& response)
    {
    if(!RestResponseParser::Parse(response))
      {
      return false;
      }

    QScriptEngine engine;
    QScriptValue data = engine.evaluate(response).property("data");

    if(data.property("bitstreams").isArray())
      {
      QScriptValueIterator bitstreams(data.property("bitstreams"));
      while(bitstreams.hasNext())
        {
        bitstreams.next();
        if(bitstreams.value().property("id").toInt32() <= 0)
          {
          break;
          }
        mdo::Bitstream* bitstream = new mdo::Bitstream();
        bitstream->SetId(bitstreams.value().property("id").toInt32());
        bitstream->SetUuid(bitstreams.value().property("uuid").toString().toStdString().c_str());
        bitstream->SetSize(bitstreams.value().property("size").toString().toStdString());
        bitstream->SetName(bitstreams.value().property("name").toString().toStdString().c_str());
        m_Item->AddBitstream(bitstream);
        }
      }
    if(data.property("authors").isArray())
      {
      QScriptValueIterator authors(data.property("authors"));
      while(authors.hasNext())
        {
        authors.next();
        m_Item->AddAuthor(authors.value().toString().toStdString());
        }
      }
    if(data.property("keywords").isArray())
      {
      QScriptValueIterator keywords(data.property("keywords"));
      while(keywords.hasNext())
        {
        keywords.next();
        m_Item->AddKeyword(keywords.value().toString().toStdString());
        }
      }
    return true;
    }
  
  /** Set the item object */
  void SetItem(mdo::Item* item) {m_Item = item;}
  
protected:

  mdo::Item*       m_Item;
};

/** Constructor */
Item::Item()
{
  m_Item = 0;
}
  
/** Destructor */
Item::~Item()
{
}
  
/** Fecth */
bool Item::Fetch()
{
  if(!m_Item)
    {
    std::cerr << "Item::Fecth : Item not set" << std::endl;
    return false;
    }

  if(m_Item->IsFetched())
    {
    return true;
    }
    
  if(m_Item->GetId() == 0)
    {
    std::cerr << "Item::Fetch : ItemId not set" << std::endl;
    return false;
    }

  ItemResponseParser parser;
  parser.SetItem(m_Item);
  m_Item->Clear();
  parser.AddTag("title",m_Item->GetTitle());
  parser.AddTag("abstract",m_Item->GetAbstract());
  parser.AddTag("uuid",m_Item->GetUuid());
  parser.AddTag("parent",m_Item->GetParentStr());
  parser.AddTag("description",m_Item->GetDescription());
  parser.AddTag("hasAgreement",m_Item->RefAgreement());
  parser.AddTag("size",m_Item->GetSize());
  
  std::stringstream url;
  url << "midas.item.get&id=" << m_Item->GetId();
  if(!WebAPI::Instance()->Execute(url.str().c_str(), &parser))
    {
    return false;
    }
  
  m_Item->SetFetched(true);
  return true;
}

// Add the object
void Item::SetObject(mdo::Object* object)
{  
  m_Item = static_cast<mdo::Item*>(object);
}

bool Item::FetchParent()
{
  mdo::Collection* parent = new mdo::Collection;
  m_Item->SetParentCollection(parent);
  parent->SetId(m_Item->GetParentId());

  mws::Collection remote;
  remote.SetObject(parent);
  return remote.Fetch();
}

bool Item::Delete()
{
  if(!m_Item)
    {
    std::cerr << "Item::Delete() : Item not set" << std::endl;
    return false;
    }

  if(!m_Item->GetId())
    {
    std::cerr << "Item::Delete() : Item id not set" << std::endl;
    return false;
    }

  std::stringstream url;
  url << "midas.item.delete&id=" << m_Item->GetId();
  if(!WebAPI::Instance()->Execute(url.str().c_str()))
    {
    return false;
    }
  return true;
}

bool Item::Commit()
{
  std::stringstream postData;
  postData << "uuid=" << m_Item->GetUuid()
    << "&parentid=" << m_Item->GetParentId()
    << "&name=" << m_Item->GetName()
    << "&abstract=" << m_Item->GetAbstract()
    << "&description=" << m_Item->GetDescription()
    << "&authors=" << m_Item->GetAuthorsString()
    << "&keywords=" << m_Item->GetKeywordsString();

  return WebAPI::Instance()->Execute("midas.item.create", NULL,
                                     postData.str().c_str());
}

} // end namespace
