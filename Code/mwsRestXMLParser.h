/*=========================================================================
  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __RestXMLParser_h
#define __RestXMLParser_h
#include "expat/expat.h"

#include <fstream>
#include <iostream>
#include <vector>

namespace mws{

/// \class XMLParserBase
/// XMLParserBase encapsulates the expat library (Insight/Utilities/expat
/// and defines the methods needed in a derived class to receive the
/// contents of an XML file in a structured manner. Some functions that are 
///  generic to opening and parsing a file are implemented here.
///
class RestXMLParser
{
public:
   
  RestXMLParser();
  ~RestXMLParser() {};

  /// @struct
  struct TagType
    {
    std::string              name;
    std::vector<std::string> attributes;
    std::string*             value; // points to a variable specified by an external class
    };

  /// Callback function -- called from XML parser with start-of-element
  /// information.
  ///
  virtual void StartElement(const char * name,const char **atts);

  /// Callback function -- called from XML parser when ending tag
  /// encountered
  ///
  virtual void EndElement(const char *name);

  /// Callback function -- called from XML parser with the character data
  /// for an XML element
  ///
  virtual void CharacterDataHandler(const char *inData, int inLength);

  // Initialize the parsing
  void Initialize();

  /// Instantiates and invokes the XML parser for the file named by
  /// m_Filename.  The parser will throw an exception in the case of XML
  /// syntax errors, missing filenames, unreadable input file, etc.
  virtual bool Parse(const char * buffer,unsigned long length);

  // Add a tag to parse
  void AddTag(const char* name,std::string& value);
  
  // Clear all the tags
  void ClearTags();

  // Finalize the parsing
  void Finalize();

  // Return the errors messages and code
  const char* GetErrorMessage() {return m_ErrorMessage.c_str();}
  int         GetErrorCode() {return m_ErrorCode;}
  
protected:
 
  // List of fields to parse
  std::vector<TagType> m_TagsToParse;
  
  // Error
  std::string m_ErrorMessage;
  int         m_ErrorCode;
  
  // Current tag
  std::string m_CurrentTag;
  
  /// filename of the input file
  std::string m_Filename;
  
  XML_Parser m_Parser;
};

} // end namespace

#endif //__RestXMLParser_h
