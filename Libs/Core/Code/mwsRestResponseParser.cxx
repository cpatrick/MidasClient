/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mwsRestResponseParser.h"

#include <QScriptEngine>

namespace mws
{

RestResponseParser::RestResponseParser()
{
  m_ErrorCode = 0;
  m_ErrorMessage = "";
  m_CurrentTag = "";
}

RestResponseParser::~RestResponseParser()
{
}

// --------------------------------------------------------------------------------------------------
bool RestResponseParser::Parse(const QString& response)
{
  QScriptValue  json;
  QScriptEngine engine;

  json = engine.evaluate(response);

  if( engine.hasUncaughtException() )
    {
    m_ErrorCode = -1; // parse error
    m_ErrorMessage = "Invalid JSON object: " + response.toStdString();
    return false;
    }

  m_ErrorCode = json.property("code").toInt32();
  m_ErrorMessage = json.property("message").toString().toStdString();

  if( m_ErrorCode != 0 || m_ErrorMessage != "" ) // server returned error
    {
    return false;
    }

  bool ok = true;
  for( std::vector<TagType>::iterator i = m_TagsToParse.begin();
       i != m_TagsToParse.end(); ++i )
    {
    if( json.property("data").property(i->name.c_str() ).isString() ||
        json.property("data").property(i->name.c_str() ).isNumber() )
      {
      *(i->value) = json.property("data").property(
          i->name.c_str() ).toString().toStdString();
      }
    else if( json.property("data").property(i->name.c_str() ).isBool() )
      {
      if( json.property("data").property(i->name.c_str() ).toBool() )
        {
        *(i->value) = "true";
        }
      else
        {
        *(i->value) = "false";
        }
      }
    else
      {
      ok = false;
      }
    }
  return ok;
}

// Add a tag to parse
void RestResponseParser::AddTag(const char* name, std::string& value)
{
  TagType tag;

  tag.name = name;
  tag.value = &value;
  m_TagsToParse.push_back(tag);
}

// Clear all the tags
void RestResponseParser::ClearTags()
{
  m_TagsToParse.clear();
}

// Return the errors messages and code
std::string RestResponseParser::GetErrorMessage()
{
  return m_ErrorMessage;
}

void RestResponseParser::SetErrorMessage(const std::string& message)
{
  m_ErrorMessage = message;
}

int RestResponseParser::GetErrorCode()
{
  return m_ErrorCode;
}

} // end namespace
