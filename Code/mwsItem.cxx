/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mwsItem.h"
#include "mdoCollection.h"
#include "mwsCollection.h"
#include <sstream>
#include <iostream>
#include "mwsRestXMLParser.h"

#include "mdoItem.h"
#include "mdoBitstream.h"
#include "midasUtils.h"

namespace mws{

/** Custom XML parser */
class ItemXMLParser : public RestXMLParser
{
public:
   
  ItemXMLParser()
    {
    m_Item = NULL;
    m_Bitstream = NULL;
    };
    
  ~ItemXMLParser() 
    {
    };  

  /// Callback function -- called from XML parser with start-of-element
  /// information.
  virtual void StartElement(const char * name, const char **atts)
    {
    RestXMLParser::StartElement(name,atts);
    if(!m_CurrentTag.compare("/rsp/bitstreams/data"))
      {
      m_Bitstream = new mdo::Bitstream;
      }  
    m_CurrentValue = "";  
    }

  /// Callback function -- called from XML parser when ending tag
  /// encountered
  virtual void EndElement(const char *name)
    {
    if(!m_CurrentTag.compare("/rsp/bitstreams/data"))
      {
      m_Item->AddBitstream(m_Bitstream);
      }
    else if(!m_CurrentTag.compare("/rsp/bitstreams/data/name"))
      {
      m_Bitstream->SetName(m_CurrentValue.c_str());
      }
    else if(!m_CurrentTag.compare("/rsp/bitstreams/data/id"))
      {
      m_Bitstream->SetId(atoi(m_CurrentValue.c_str()));
      }
    else if(!m_CurrentTag.compare("/rsp/bitstreams/data/size"))
      {
      m_Bitstream->SetSize(m_CurrentValue.c_str());
      }
    else if(!m_CurrentTag.compare("/rsp/bitstreams/data/uuid"))
      {
      m_Bitstream->SetUuid(m_CurrentValue.c_str());
      }
    else if(!m_CurrentTag.compare("/rsp/authors/data"))
      {
      m_Item->AddAuthor(m_CurrentValue);
      }
    else if(!m_CurrentTag.compare("/rsp/keywords/data"))
      {
      m_Item->AddKeyword(m_CurrentValue);
      }

    RestXMLParser::EndElement(name);
    }
    
  /// Callback function -- called from XML parser with the character data
  /// for an XML element
  virtual void CharacterDataHandler(const char *inData, int inLength)
    {
    RestXMLParser::CharacterDataHandler(inData,inLength);
    m_CurrentValue.append(inData,inLength);
    }
  
  /** Set the item object */
  void SetItem(mdo::Item* item) {m_Item = item;}
  
protected:

  mdo::Item*       m_Item;
  mdo::Bitstream*  m_Bitstream;
  std::string      m_CurrentValue;
};

/** Constructor */
Item::Item()
{
  m_Item = 0;
}
  
/** Destructor */
Item::~Item()
{
}
  
/** Fecth */
bool Item::Fetch()
{
  if(!m_Item)
    {
    std::cerr << "Item::Fecth : Item not set" << std::endl;
    return false;
    }

  if(m_Item->IsFetched())
    {
    return true;
    }
    
  if(m_Item->GetId() == 0)
    {
    std::cerr << "Item::Fetch : ItemId not set" << std::endl;
    return false;
    }

  ItemXMLParser parser;
  parser.SetItem(m_Item);
  m_Item->Clear();
  parser.AddTag("/rsp/title",m_Item->GetTitle());
  parser.AddTag("/rsp/abstract",m_Item->GetAbstract());
  parser.AddTag("/rsp/uuid",m_Item->GetUuid());
  parser.AddTag("/rsp/parent",m_Item->GetParent());
  parser.AddTag("/rsp/description",m_Item->GetDescription());
  parser.AddTag("/rsp/hasAgreement",m_Item->RefAgreement());
  parser.AddTag("/rsp/size",m_Item->GetSize());
  
  mws::WebAPI::Instance()->GetRestAPI()->SetXMLParser(&parser);
  
  std::stringstream url;
  url << "midas.item.get?id=" << m_Item->GetId();
  if(!mws::WebAPI::Instance()->Execute(url.str().c_str()))
    {
    std::cout << mws::WebAPI::Instance()->GetErrorMessage() << std::endl;
    return false;
    }
  
  m_Item->SetFetched(true);
  return true;
}

/** Commit */
bool Item::Commit()
{
  return true;
}

// Add the object
void Item::SetObject(mdo::Object* object)
{  
  m_Item = static_cast<mdo::Item*>(object);
}

bool Item::FetchParent()
{
  mdo::Collection* parent = new mdo::Collection;
  m_Item->SetParentCollection(parent);
  parent->SetId(m_Item->GetParentId());

  mws::Collection remote;
  remote.SetObject(parent);
  return remote.Fetch();
}

bool Item::Delete()
{
  if(!m_Item)
    {
    std::cerr << "Item::Delete() : Item not set" << std::endl;
    return false;
    }

  if(!m_Item->GetId())
    {
    std::cerr << "Item::Delete() : Item id not set" << std::endl;
    return false;
    }
       
  RestXMLParser parser;
  mws::WebAPI::Instance()->GetRestAPI()->SetXMLParser(&parser);

  std::stringstream url;
  url << "midas.item.delete?id=" << m_Item->GetId();
  if(!mws::WebAPI::Instance()->Execute(url.str().c_str()))
    {
    std::cout << mws::WebAPI::Instance()->GetErrorMessage() << std::endl;
    return false;
    }
  return true;
}

bool Item::Create()
{
  std::stringstream postData;
  postData << "uuid=" << m_Item->GetUuid()
    << "&parentid=" << m_Item->GetParentId()
    << "&name=" << midasUtils::EscapeForURL(m_Item->GetName())
    << "&abstract=" << midasUtils::EscapeForURL(m_Item->GetAbstract())
    << "&description=" << midasUtils::EscapeForURL(m_Item->GetDescription())
    << "&authors=" << midasUtils::EscapeForURL(m_Item->GetAuthorsString())
    << "&keywords=" << midasUtils::EscapeForURL(m_Item->GetKeywordsString());

  mws::RestXMLParser parser;
  mws::WebAPI::Instance()->SetPostData(postData.str().c_str());
  mws::WebAPI::Instance()->GetRestAPI()->SetXMLParser(&parser);
  return mws::WebAPI::Instance()->Execute("midas.item.create");
}

} // end namespace
