/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "mwsSearch.h"
#include "mdoObject.h"
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <iostream>
#include "mwsRestXMLParser.h"

namespace mws
{
/** Custom XML parser */
class SearchXMLParser : public RestXMLParser
{
public:
   
  SearchXMLParser()
    {
    m_Results = NULL;
    };
    
  ~SearchXMLParser() 
    {
    };  

  // Callback function -- called from XML parser with start-of-element
  // information.
  virtual void StartElement(const char * name,const char **atts)
    {
    RestXMLParser::StartElement(name,atts);
    m_CurrentValue = "";  
    }

  // Callback function -- called from XML parser when ending tag
  // encountered
  virtual void EndElement(const char *name)
    {
    /*if(!strcmp(name,"data"))
      {
      m_NewResources->AddUuid(m_CurrentValue);
      }*/
    RestXMLParser::EndElement(name);
    }
    
  // Callback function -- called from XML parser with the character data
  // for an XML element
  virtual void CharacterDataHandler(const char *inData, int inLength)
    {
    RestXMLParser::CharacterDataHandler(inData,inLength);
    m_CurrentValue.append(inData,inLength);
    }
  
  /** Set the NewResources object */
  void SetObject(std::vector<mdo::Object*>* results) 
    {
    m_Results = results;
    }
  
protected:
  enum ParseStatus
    {
    NONE,
    BITSTREAMS,
    ITEMS,
    COLLECTIONS,
    COMMUNITIES
    };
  
  ParseStatus                m_Status;
  std::string                m_CurrentValue;
  std::vector<mdo::Object*>* m_Results;
};


std::vector<mdo::Object*> Search::SearchServer(std::vector<std::string> tokens)
{
  std::vector<mdo::Object*> results;
  return results;
}

std::vector<mdo::Object*> Search::SearchClient(std::vector<std::string> tokens)
{
  std::vector<mdo::Object*> results;
  return results;
}

} //end namespace