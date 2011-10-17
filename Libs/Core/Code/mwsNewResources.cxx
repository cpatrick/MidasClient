/******************************************************************************
 * Copyright 2011 Kitware Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

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

void NewResources::ResolveParents()
{
}

std::vector<std::string> NewResources::GetUuids()
{
  return m_Uuids;
}

void NewResources::AddUuid(std::string uuid)
{
  m_Uuids.push_back(uuid);
}

std::string NewResources::GetSince()
{
  return m_Since;
}

void NewResources::SetSince(std::string since)
{
  m_Since = since;
}

std::string & NewResources::GetTimestamp()
{
  return m_Timestamp;
}

} // end namespace
