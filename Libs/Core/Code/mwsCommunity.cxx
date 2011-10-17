/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mwsCommunity.h"
#include "mdoCommunity.h"
#include "mdoCollection.h"
#include "mwsRestResponseParser.h"

#include <QScriptEngine>
#include <QScriptValueIterator>

namespace mws
{

/** Custom Response parser */
class CommunityResponseParser : public RestResponseParser
{
public:

  CommunityResponseParser()
  {
    m_Community = NULL;
  }

  ~CommunityResponseParser()
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
    this->RecurseCommunityTree(data, m_Community);
    return true;
  }

  /** Set the community object */
  void SetCommunity(mdo::Community* community)
  {
    m_Community = community;
  }

protected:

  void RecurseCommunityTree(const QScriptValue& node, mdo::Community* parent)
  {
    if( node.property("communities").isArray() )
      {
      QScriptValueIterator comms(node.property("communities") );
      while( comms.hasNext() )
        {
        comms.next();
        if( comms.value().property("id").toInt32() <= 0 )
          {
          break;
          }
        mdo::Community* comm = new mdo::Community();
        comm->SetId(comms.value().property("id").toInt32() );
        comm->SetParentId(comms.value().property("parentid").toInt32() );
        comm->SetUuid(comms.value().property("uuid").toString().toStdString().c_str() );
        comm->SetName(comms.value().property("fullname").toString().toStdString().c_str() );
        comm->SetParentCommunity(parent);
        parent->AddCommunity(comm);

        this->RecurseCommunityTree(comms.value(), comm);
        }
      }
    if( node.property("collections").isArray() )
      {
      QScriptValueIterator colls(node.property("collections") );
      while( colls.hasNext() )
        {
        colls.next();
        if( colls.value().property("id").toInt32() <= 0 )
          {
          break;
          }
        mdo::Collection* coll = new mdo::Collection();
        coll->SetId(colls.value().property("id").toInt32() );
        coll->SetParentId(colls.value().property("parentid").toInt32() );
        coll->SetUuid(colls.value().property("uuid").toString().toStdString().c_str() );
        coll->SetName(colls.value().property("fullname").toString().toStdString().c_str() );
        coll->SetParentCommunity(parent);
        parent->AddCollection(coll);
        }
      }
  }

  mdo::Community* m_Community;
};

/** Constructor */
Community::Community()
{
  m_Community = NULL;
}

/** Destructor */
Community::~Community()
{
}

// Add the object
void Community::SetObject(mdo::Object* object)
{
  m_Community = static_cast<mdo::Community *>(object);
}

/** Fetch */
bool Community::Fetch()
{
  if( !m_Community )
    {
    std::cerr << "Community::Fetch() : Community not set" << std::endl;
    return false;
    }

  if( m_Community->IsFetched() )
    {
    return true;
    }

  if( !m_Community->GetId() )
    {
    std::cerr << "Community::Fetch() : Community id not set" << std::endl;
    return false;
    }

  CommunityResponseParser parser;
  parser.SetCommunity(m_Community);
  m_Community->Clear();
  parser.AddTag("name", m_Community->GetName() );
  parser.AddTag("description", m_Community->GetDescription() );
  parser.AddTag("copyright", m_Community->GetCopyright() );
  parser.AddTag("introductory", m_Community->GetIntroductoryText() );
  parser.AddTag("uuid", m_Community->GetUuid() );
  parser.AddTag("parent", m_Community->GetParentStr() );
  parser.AddTag("hasAgreement", m_Community->RefAgreement() );
  parser.AddTag("size", m_Community->GetSize() );

  std::stringstream url;
  url << "midas.community.get&id=" << m_Community->GetId();
  if( !WebAPI::Instance()->Execute(url.str().c_str(), &parser) )
    {
    return false;
    }
  m_Community->SetFetched(true);
  return true;
}

/** Fill the full tree with community and collection */
bool Community::FetchTree()
{
  CommunityResponseParser parser;

  parser.SetCommunity(m_Community);

  std::stringstream url;
  url << "midas.community.tree&id=" << m_Community->GetId();
  if( !mws::WebAPI::Instance()->Execute(url.str().c_str(), &parser) )
    {
    return false;
    }
  return true;
}

bool Community::FetchParent()
{
  int id = m_Community->GetParentId();

  if( id )
    {
    mdo::Community* parent = new mdo::Community;
    m_Community->SetParentCommunity(parent);
    parent->SetId(m_Community->GetParentId() );

    mws::Community remote;
    remote.SetObject(parent);
    return remote.Fetch();
    }
  return true;
}

bool Community::Delete()
{
  if( !m_Community )
    {
    std::cerr << "Community::Delete() : Community not set" << std::endl;
    return false;
    }

  if( !m_Community->GetId() )
    {
    std::cerr << "Community::Delete() : Community id not set" << std::endl;
    return false;
    }

  std::stringstream url;
  url << "midas.community.delete&id=" << m_Community->GetId();
  if( !WebAPI::Instance()->Execute(url.str().c_str() ) )
    {
    return false;
    }
  return true;
}

bool Community::Commit()
{
  std::stringstream postData;

  postData << "uuid=" << m_Community->GetUuid()
           << "&parentid=" << m_Community->GetParentId()
           << "&name=" << m_Community->GetName()
           << "&copyright=" << m_Community->GetCopyright()
           << "&introductorytext=" << m_Community->GetIntroductoryText()
           << "&description=" << m_Community->GetDescription()
           << "&links=" << m_Community->GetLinks();

  return WebAPI::Instance()->Execute("midas.community.create", NULL,
                                     postData.str().c_str() );
}

} // end namespace
