/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "mwsTreePath.h"
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <iostream>
#include "mwsWebAPI.h"
#include "mwsRestXMLParser.h"

namespace mws
{
/** Custom XML parser */
class TreePathXMLParser : public RestXMLParser
{
public:
   
  TreePathXMLParser()
    {
    m_Results = NULL;
    };
    
  ~TreePathXMLParser() 
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
    if(!strcmp(name,"data"))
      {
      m_Results->push_back(m_CurrentValue);
      }
    RestXMLParser::EndElement(name);
    }
    
  // Callback function -- called from XML parser with the character data
  // for an XML element
  virtual void CharacterDataHandler(const char *inData, int inLength)
    {
    RestXMLParser::CharacterDataHandler(inData,inLength);
    m_CurrentValue.append(inData,inLength);
    }
  
  /** Set the results object */
  void SetObject(std::vector<std::string>* results)
    {
    m_Results = results;
    }
  
protected:
  std::string               m_CurrentValue;
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
  TreePathXMLParser parser;
  parser.SetObject(&results);
  mws::WebAPI::Instance()->GetRestAPI()->SetXMLParser(&parser);
  
  std::stringstream fields;
  fields << url << "?uuid=" << uuid;

  mws::WebAPI::Instance()->Execute(fields.str().c_str(), NULL);
  return results;
}

} //end namespace
