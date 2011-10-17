/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mwsCollection.h"
#include "mwsRestResponseParser.h"
#include "mwsCommunity.h"
#include "mdoCollection.h"
#include "mdoCommunity.h"
#include "mdoItem.h"

#include <QScriptEngine>
#include <QScriptValueIterator>

namespace mws
{

/** Custom Response parser */
class CollectionResponseParser : public RestResponseParser
{
public:

  CollectionResponseParser()
  {
    m_Collection = NULL;
  }

  ~CollectionResponseParser()
  {
  }

  virtual bool Parse(const QString& response)
  {
    if( !RestResponseParser::Parse(response) )
      {
      return false;
      }

    QScriptEngine engine;
    QScriptValue  data = engine.evaluate(response).property("data");
    if( data.property("items").isArray() )
      {
      QScriptValueIterator items(data.property("items") );
      while( items.hasNext() )
        {
        items.next();
        if( items.value().property("id").toInt32() <= 0 )
          {
          break;
          }
        mdo::Item* item = new mdo::Item();
        item->SetId(items.value().property("id").toInt32() );
        item->SetUuid(items.value().property("uuid").toString().toStdString().c_str() );
        item->SetTitle(items.value().property("title").toString().toStdString().c_str() );
        item->SetParentCollection(m_Collection);

        m_Collection->AddItem(item);
        }
      }
    return true;
  }

  /** Set the collection object */
  void SetCollection(mdo::Collection* collection)
  {
    m_Collection = collection;
  }
protected:

  mdo::Collection* m_Collection;
};

/** Constructor */
Collection::Collection()
{
  m_Collection = 0;
}

/** Destructor */
Collection::~Collection()
{
}

/** Add the object */
void Collection::SetObject(mdo::Object* object)
{
  m_Collection = static_cast<mdo::Collection *>(object);
}

/** Fetch */
bool Collection::Fetch()
{
  if( !m_Collection )
    {
    std::cerr << "Collection::Fetch() : Collection not set" << std::endl;
    return false;
    }

  if( m_Collection->IsFetched() )
    {
    return true;
    }

  if( !m_Collection->GetId() )
    {
    std::cerr << "Collection::Fetch() : Collection id not set" << std::endl;
    return false;
    }

  CollectionResponseParser parser;
  parser.SetCollection(m_Collection);
  m_Collection->Clear();
  parser.AddTag("name", m_Collection->GetName() );
  parser.AddTag("description", m_Collection->GetDescription() );
  parser.AddTag("copyright", m_Collection->GetCopyright() );
  parser.AddTag("introductory", m_Collection->GetIntroductoryText() );
  parser.AddTag("uuid", m_Collection->GetUuid() );
  parser.AddTag("parent", m_Collection->GetParentStr() );
  parser.AddTag("hasAgreement", m_Collection->RefAgreement() );
  parser.AddTag("size", m_Collection->GetSize() );

  std::stringstream url;
  url << "midas.collection.get&id=" << m_Collection->GetId();
  if( !WebAPI::Instance()->Execute(url.str().c_str(), &parser) )
    {
    return false;
    }
  m_Collection->SetFetched(true);
  return true;
}

bool Collection::FetchParent()
{
  mdo::Community* parent = new mdo::Community;

  m_Collection->SetParentCommunity(parent);
  parent->SetId(m_Collection->GetParentId() );

  Community remote;
  remote.SetObject(parent);
  return remote.Fetch();
}

bool Collection::Delete()
{
  if( !m_Collection )
    {
    std::cerr << "Collection::Delete() : Collection not set" << std::endl;
    return false;
    }

  if( !m_Collection->GetId() )
    {
    std::cerr << "Collection::Delete() : Collection id not set" << std::endl;
    return false;
    }

  std::stringstream url;
  url << "midas.collection.delete&id=" << m_Collection->GetId();
  if( !WebAPI::Instance()->Execute(url.str().c_str() ) )
    {
    return false;
    }
  return true;
}

bool Collection::Commit()
{
  std::stringstream postData;

  postData << "uuid=" << m_Collection->GetUuid()
           << "&parentid=" << m_Collection->GetParentId()
           << "&name=" << m_Collection->GetName()
           << "&description=" << m_Collection->GetDescription()
           << "&introductorytext=" << m_Collection->GetIntroductoryText()
           << "&copyright=" << m_Collection->GetCopyright();

  return WebAPI::Instance()->Execute("midas.collection.create", NULL, postData.str().c_str() );
}

} // end namespace
