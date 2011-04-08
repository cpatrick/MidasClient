/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mwsCommunity.h"
#include "mdoCommunity.h"
#include "mdoCollection.h"
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <iostream>
#include "mwsRestXMLParser.h"

namespace mws{

/** Custom XML parser */
class CommunityXMLParser : public RestXMLParser
{
public:
   
  CommunityXMLParser()
    {
    m_ParentCommunity = NULL;
    m_Community = NULL;
    m_Collection = NULL;
    m_InCollection = false;
    m_InCommunity = true;  
    };
    
  ~CommunityXMLParser() 
    {
    };  

  // Callback function -- called from XML parser with start-of-element
  // information.
  virtual void StartElement(const char * name,const char **atts)
    {
    RestXMLParser::StartElement(name,atts);
    if(!strcmp(name,"data"))
      {
      if(m_InCommunity)
        {
        if(!m_Community)
          {
          std::cout << "Community is null" << std::endl;
          }
        m_ParentCommunity = m_Community;
        m_Community = new mdo::Community();
        m_Community->SetParentCommunity(m_ParentCommunity);
        if(!m_ParentCommunity)
          {
          std::cout << "Parent community is null" << std::endl;
          }
        m_ParentCommunity->AddCommunity(m_Community);
        }
      else if(m_InCollection)
        {
        m_Collection = new mdo::Collection();
        m_Community->AddCollection(m_Collection);
        }
      }
    else if(!strcmp(name,"communities"))
      {
      m_InCommunity = true;
      m_InCollection = false;
      }  
    else if(!strcmp(name,"collections"))
      {
      m_InCollection = true;
      m_InCommunity = false;
      }  
    m_CurrentValue = "";  
    }

  // Callback function -- called from XML parser when ending tag
  // encountered
  virtual void EndElement(const char *name)
    {
    if(!strcmp(name,"data"))
      {
      if(m_InCommunity)
        {
        if(m_ParentCommunity)
          {
          m_Community = m_ParentCommunity;
          m_ParentCommunity = m_ParentCommunity->GetParentCommunity();
          }
        }
      }
    else if(!strcmp(name,"collections"))
      {
      m_InCollection = false;
      m_InCommunity = true;
      } 
    // If we are in the collection
    else if(m_InCollection)
      {
      if(!strcmp(name,"id"))
        {
        m_Collection->SetId(atoi(m_CurrentValue.c_str()));
        }
      if(!strcmp(name,"name"))
        {
        m_Collection->SetName(m_CurrentValue.c_str());
        }
      if(!strcmp(name,"description"))
        {
        m_Collection->SetDescription(m_CurrentValue.c_str());
        }
      if(!strcmp(name,"parentid"))
        {
        m_Collection->SetParent(m_CurrentValue.c_str());
        }
      if(!strcmp(name,"uuid"))
        {
        m_Collection->SetUuid(m_CurrentValue.c_str());
        }
      }
    // If we are in the community
    else if(m_InCommunity)
      {
      if(!strcmp(name,"id"))
        {
        m_Community->SetId(atoi(m_CurrentValue.c_str()));
        }
      if(!strcmp(name,"name"))
        {
        m_Community->SetName(m_CurrentValue.c_str());
        }
      if(!strcmp(name,"description"))
        {
        m_Community->SetDescription(m_CurrentValue.c_str());
        }
      if(!strcmp(name,"parentid"))
        {
        m_Community->SetParent(m_CurrentValue.c_str());
        }
      if(!strcmp(name,"uuid"))
        {
        m_Community->SetUuid(m_CurrentValue.c_str());
        }
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
  
  /** Set the community object */
  void SetCommunity(mdo::Community* community) 
    {
    m_ParentCommunity = NULL;
    m_Community = community;
    }
  
protected:

  bool              m_InCollection;
  bool              m_InCommunity;
  mdo::Community*   m_ParentCommunity;
  mdo::Community*   m_Community;
  mdo::Collection*  m_Collection;
  std::string       m_CurrentValue;
};


/** Constructor */
Community::Community()
{
  m_Community=0;
}
  
/** Destructor */
Community::~Community()
{
}

// Add the object
void Community::SetObject(mdo::Object* object)
{  
  m_Community = static_cast<mdo::Community*>(object);
}

/** Fetch */
bool Community::Fetch()
{
  if(!m_Community)
    {
    std::cerr << "Community::Fetch() : Community not set" << std::endl;
    return false;
    }

  if(m_Community->IsFetched())
    {
    return true;
    }
    
  if(!m_Community->GetId())
    {
    std::cerr << "Community::Fetch() : Community id not set" << std::endl;
    return false;
    } 
    
  CommunityXMLParser parser;
  parser.SetCommunity(m_Community);
  m_Community->Clear();
  parser.AddTag("/rsp/name",m_Community->GetName());
  parser.AddTag("/rsp/description",m_Community->GetDescription());
  parser.AddTag("/rsp/copyright",m_Community->GetCopyright());
  parser.AddTag("/rsp/introductory",m_Community->GetIntroductoryText());
  parser.AddTag("/rsp/uuid",m_Community->GetUuid());
  parser.AddTag("/rsp/parent",m_Community->GetParent());
  parser.AddTag("/rsp/hasAgreement",m_Community->RefAgreement());
  parser.AddTag("/rsp/size",m_Community->GetSize());
  
  std::stringstream url;
  url << "midas.community.get?id=" << m_Community->GetId();
  if(!WebAPI::Instance()->Execute(url.str().c_str(), &parser))
    {
    return false;
    }
  m_Community->SetFetched(true);
  return true;
}

/** Fill the full tree with community and collection */
bool Community::FetchTree()
{
  CommunityXMLParser parser;
  parser.SetCommunity(m_Community);
  
  std::stringstream url;
  url << "midas.community.tree?id=" << m_Community->GetId();
  if(!mws::WebAPI::Instance()->Execute(url.str().c_str(), &parser))
    {
    return false;
    }
  return true;
}

/** Commit */
bool Community::Commit()
{
  return true;
}

bool Community::FetchParent()
{
  int id = m_Community->GetParentId();
  if(id)
    {
    mdo::Community* parent = new mdo::Community;
    m_Community->SetParentCommunity(parent);
    parent->SetId(m_Community->GetParentId());

    mws::Community remote;
    remote.SetObject(parent);
    return remote.Fetch();
    }
  return true;
}

bool Community::Delete()
{
  if(!m_Community)
    {
    std::cerr << "Community::Delete() : Community not set" << std::endl;
    return false;
    }

  if(!m_Community->GetId())
    {
    std::cerr << "Community::Delete() : Community id not set" << std::endl;
    return false;
    }

  std::stringstream url;
  url << "midas.community.delete?id=" << m_Community->GetId();
  if(!WebAPI::Instance()->Execute(url.str().c_str()))
    {
    return false;
    }
  return true;
}

bool Community::Create()
{
  std::stringstream postData;
  postData << "uuid=" << m_Community->GetUuid() 
    << "&parentid=" << m_Community->GetParentId()
    << "&name=" << midasUtils::EscapeForURL(m_Community->GetName())
    << "&copyright=" << midasUtils::EscapeForURL(m_Community->GetCopyright())
    << "&introductorytext=" <<
    midasUtils::EscapeForURL(m_Community->GetIntroductoryText())
    << "&description=" <<
    midasUtils::EscapeForURL(m_Community->GetDescription())
    << "&links=" << midasUtils::EscapeForURL(m_Community->GetLinks());

  return WebAPI::Instance()->Execute("midas.community.create", NULL,
    postData.str().c_str());
}

} // end namespace
