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


#include "mwsTreePath.h"
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
/** Custom Response parser */
class TreePathResponseParser : public RestResponseParser
{
public:

  TreePathResponseParser()
  {
    m_Results = NULL;
  }

  ~TreePathResponseParser()
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

    if( data.isArray() )
      {
      QScriptValueIterator node(data);
      while( node.hasNext() )
        {
        node.next();
        m_Results->push_back(node.value().toString().toStdString() );
        }
      }
    return true;
  }

  /** Set the results object */
  void SetObject(std::vector<std::string>* results)
  {
    m_Results = results;
  }

protected:
  std::vector<std::string>* m_Results;
};

std::vector<std::string> TreePath::PathToRoot(std::string uuid)
{
  return TreePath::PathInternal(uuid, "midas.path.to.root");
}

std::vector<std::string> TreePath::PathFromRoot(std::string uuid)
{
  return TreePath::PathInternal(uuid, "midas.path.from.root");
}

std::vector<std::string> TreePath::PathInternal(std::string uuid, std::string url)
{
  std::vector<std::string> results;
  TreePathResponseParser   parser;
  parser.SetObject(&results);

  std::stringstream fields;
  fields << url << "&uuid=" << uuid;

  WebAPI::Instance()->Execute(fields.str().c_str(), &parser);
  return results;
}

} // end namespace
