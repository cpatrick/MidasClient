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


#ifndef __mwsRestResponseParser_h
#define __mwsRestResponseParser_h

#include "midasStandardIncludes.h"

#include <QString>

namespace mws
{

class RestResponseParser
{
public:

  RestResponseParser();
  ~RestResponseParser();

  // / @struct
  struct TagType
    {
    std::string name;
    std::vector<std::string> attributes;
    std::string* value;             // points to a variable specified by an
                                    // external class
    };

  // / Instantiates and invokes the JSON parser on the content provided
  virtual bool Parse(const QString& response);

  // Add a tag to parse
  void AddTag(const char* name, std::string& value);

  // Clear all the tags
  void ClearTags();

  // Return the errors messages and code
  std::string GetErrorMessage();

  void SetErrorMessage(const std::string& message);

  int GetErrorCode();

protected:

  // List of fields to parse
  std::vector<TagType> m_TagsToParse;

  // Error
  std::string m_ErrorMessage;
  int         m_ErrorCode;

  // Current tag
  std::string m_CurrentTag;
};

} // end namespace

#endif // __RestXMLParser_h
