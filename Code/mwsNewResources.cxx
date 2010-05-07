/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mwsNewResources.h"
#include "midasUtils.h"
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <iostream>
#include "mwsRestXMLParser.h"

namespace mws{

/** Custom XML parser */
class NewResourcesXMLParser : public RestXMLParser
{
public:
   
  NewResourcesXMLParser()
    {
    m_NewResources;
    };
    
  ~NewResourcesXMLParser() 
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
      m_NewResources->AddUuid(m_CurrentValue);
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
  
  /** Set the NewResources object */
  void SetObject(mws::NewResources* nr) 
    {
    m_NewResources = nr;
    }
  
protected:

  mws::NewResources*  m_NewResources;
  std::string         m_CurrentValue;
};


/** Constructor */
NewResources::NewResources()
{
}
  
/** Destructor */
NewResources::~NewResources()
{
}

// Add the object
void NewResources::SetObject(mdo::Object* object)
{
}

/** Fetch the list of new resources */
bool NewResources::Fetch()
{
  NewResourcesXMLParser parser;
  parser.SetObject(this);
  this->m_Uuids.clear();
  parser.AddTag("/rsp/timestamp",this->GetTimestamp());
  
  mws::WebAPI::Instance()->GetRestAPI()->SetXMLParser(&parser);
  
  std::stringstream url;
  url << "midas.newresources.get";

  if(this->m_Since != "")
    {
      url << "?since=" << midasUtils::EscapeForURL(m_Since);
    }
  return mws::WebAPI::Instance()->Execute(url.str().c_str());
}

bool NewResources::FetchTree()
{
  return this->Fetch();
}

/** Commit */
bool NewResources::Commit()
{
  return true;
}

} // end namespace
