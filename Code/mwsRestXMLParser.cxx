/*=========================================================================

  Program:   Dionysus
  Module:    mwsRestXMLParser.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mwsRestXMLParser.h"

namespace mws{

/** Constructor */
RestXMLParser::RestXMLParser()
{
  m_ErrorCode = 0;
  m_ErrorMessage = "";
  m_CurrentTag = "";
}
 
//----------------------------------------------------------------------------
static void itkXMLParserStartElement(void* parser, const char *name,
                                    const char **atts)
{
  // Begin element handler that is registered with the XML_Parser.
  // This just casts the user data to a itkXMLParser and calls
  // StartElement.
  static_cast<RestXMLParser*>(parser)->StartElement(name, atts);
}

//----------------------------------------------------------------------------
static void itkXMLParserEndElement(void* parser, const char *name)
{
  // End element handler that is registered with the XML_Parser.  This
  // just casts the user data to a itkXMLParser and calls EndElement.
  static_cast<RestXMLParser*>(parser)->EndElement(name);
}

//--------------------------------------------------------------------------------------------------
static void itkXMLParserCharacterDataHandler(void* parser, const char* data,
                                      int length)
  {
  // Character data handler that is registered with the XML_Parser.
  // This just casts the user data to a itkXMLParser and calls
  // CharacterDataHandler.
  static_cast<RestXMLParser*>(parser)->CharacterDataHandler(data, length);
  }

/** Initialize **/
void RestXMLParser::Initialize(void)
{
  m_Parser = XML_ParserCreate(0);

  XML_SetElementHandler(m_Parser,
                        &itkXMLParserStartElement,
                        &itkXMLParserEndElement);

  XML_SetCharacterDataHandler(m_Parser,
                              &itkXMLParserCharacterDataHandler);

  XML_SetUserData(m_Parser,this);

  this->m_ErrorMessage = "";
  this->m_CurrentTag = "";
  this->m_ErrorCode = 0;
}
  
//--------------------------------------------------------------------------------------------------
bool RestXMLParser::Parse(const char* buffer,unsigned long length)
{
  if(!m_Parser)
    {
    return false;
    }
  int result = XML_Parse(m_Parser, buffer, length, false);
  if(result==0)
    {
    m_ErrorCode = 1;
    m_ErrorMessage = "Cannot parse XML";
    return false;
    }
  return true;
}


// Add a tag to parse
void RestXMLParser::AddTag(const char* name,std::string& value)
{
  TagType tag;
  tag.name = name;
  tag.value = &value;
  m_TagsToParse.push_back(tag);
}
  
// Clear all the tags
void RestXMLParser::ClearTags()
{
  m_TagsToParse.clear();
} 
  
/** Finalize **/
void RestXMLParser::Finalize(void)
{
  XML_ParserFree(m_Parser);
}

// Callback function -- called from XML parser with start-of-element
// information.
void RestXMLParser::StartElement(const char * name,const char **atts)
{
  m_CurrentTag += "/";
  m_CurrentTag += name;
  
  // If it's an error message
  std::string val = "";
  if(!m_CurrentTag.compare("/rsp/err"))
    {
    for(int i = 0; atts[i] != 0; i++)
      {
      if(i%2==0)
        {
        val = atts[i];
        }
      else
        {
        if(!val.compare("code"))
          {
          m_ErrorCode = atoi(atts[i]);
          }
        else if(!val.compare("msg"))
          {
          m_ErrorMessage = atts[i];
          }
        }
      }
    return;   
    }
}

// Callback function -- called from XML parser when ending tag
// encountered
void RestXMLParser::EndElement(const char *name)
{
  std::string tag = "/";
  tag += name;
  unsigned int pos = m_CurrentTag.find(tag);
  if(pos != -1)
    {
    m_CurrentTag = m_CurrentTag.substr(0,pos);
    }
}

//--------------------------------------------------------------------------------------------------
// Callback function -- called from XML parser with the character data
// for an XML element
void RestXMLParser::CharacterDataHandler(const char *inData, int inLength)
{
  std::vector<TagType>::iterator it = m_TagsToParse.begin();
  while(it!=m_TagsToParse.end())
    {
    if(!(*it).name.compare(m_CurrentTag))
      {
      (*((*it).value)).append(inData,inLength);
      }
    it++;
    }
  }

} // end namespace