/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mwsNewResources.h"
#include "midasUtils.h"
#include "mwsRestResponseParser.h"

#include <QScriptEngine>
#include <QScriptValueIterator>

namespace mws
{

/** Custom Response parser */
class NewResourcesResponseParser : public RestResponseParser
{
public:

  NewResourcesResponseParser()
  {
    m_NewResources = NULL;
  }

  ~NewResourcesResponseParser()
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

    if( data.property("modified").isArray() )
      {
      QScriptValueIterator resource(data.property("modified") );
      while( resource.hasNext() )
        {
        resource.next();
        m_NewResources->AddUuid(resource.value().toString().toStdString() );
        }
      }
    return true;
  }

  void SetObject(mws::NewResources* nr)
  {
    m_NewResources = nr;
  }

protected:

  mws::NewResources* m_NewResources;
  std::string        m_CurrentValue;
};

NewResources::NewResources()
{
}

NewResources::~NewResources()
{
}

void NewResources::SetObject(mdo::Object* object)
{
  (void)object;
}

bool NewResources::Fetch()
{
  NewResourcesResponseParser parser;

  parser.SetObject(this);
  this->m_Uuids.clear();
  parser.AddTag("timestamp", this->GetTimestamp() );

  std::stringstream url;
  url << "midas.newresources.get";

  if( this->m_Since != "" )
    {
    url << "&since=" << m_Since;
    }
  return WebAPI::Instance()->Execute(url.str().c_str(), &parser);
}

bool NewResources::FetchTree()
{
  return this->Fetch();
}

bool NewResources::Commit()
{
  return true;
}

} // end namespace
