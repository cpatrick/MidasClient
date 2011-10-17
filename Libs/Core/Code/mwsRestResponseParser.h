/*=========================================================================
  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

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
