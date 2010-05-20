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
  
  m_WebAPI->GetRestAPI()->SetXMLParser(&parser);
  
  std::stringstream url;
  url << "midas.item.get?id=" << m_Item->GetId();
  if(!m_WebAPI->Execute(url.str().c_str()))
    {
    std::cout << m_WebAPI->GetErrorMessage() << std::endl;
    return false;
    }
    
  return true;
}

/** Commit */
bool Item::Commit()
{
  return true;
}


// Upload a bitstream
unsigned int Item::UploadBitstream(unsigned int itemid, const char* filename)
{
  std::string bitstreamidstring;
  
  RestXMLParser* parser = m_WebAPI->GetRestXMLParser();
  parser->ClearTags();
  parser->AddTag("/rsp/id", bitstreamidstring);

  std::stringstream url;
  url << "midas.upload.bitstream?itemid=" << itemid;
  if(!m_WebAPI->UploadFile(url.str().c_str(),filename))
    {
    std::cout << m_WebAPI->GetErrorMessage() << std::endl;
    }
  
  unsigned int bitstreamid = atoi(bitstreamidstring.c_str());  
  return bitstreamid;
}
 
// Download a bitstream
std::string Item::DownloadBitstream(const char* uuid, const char* filename)
{
  std::stringstream url;
  url << "midas.bitstream.download?id=" << uuid;
  if(!m_WebAPI->DownloadFile(url.str().c_str(),filename))
    {
    std::cout << m_WebAPI->GetErrorMessage() << std::endl;
    return "";
    }
  return filename;
}

// Create a resource given an item id
bool Item::CreateResource(unsigned int itemid)
{
  std::stringstream url;
  url << "midas.item.resource.create?id=" << itemid;
  if(!m_WebAPI->Execute(url.str().c_str()))
    {
    std::cout << m_WebAPI->GetErrorMessage() << std::endl;
    return false;
    }
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
  remote.SetWebAPI(mws::WebAPI::Instance());
  remote.SetObject(parent);
  return remote.Fetch();
}

} // end namespace
