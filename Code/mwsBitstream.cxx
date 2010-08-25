/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mwsBitstream.h"
#include "mdoBitstream.h"
#include "mdoItem.h"
#include "mwsItem.h"
#include <sstream>
#include <iostream>
#include "mwsRestXMLParser.h"
#include "mwsWebAPI.h"

namespace mws{

/** Custom XML parser */
class BitstreamXMLParser : public RestXMLParser
{
public:
   
  BitstreamXMLParser()
    {
    m_Bitstream = NULL;
    };
  ~BitstreamXMLParser() {};  

  /// Callback function -- called from XML parser with start-of-element
  /// information.
  virtual void StartElement(const char * name,const char **atts)
    {
    RestXMLParser::StartElement(name,atts);
    }

  /// Callback function -- called from XML parser when ending tag
  /// encountered
  virtual void EndElement(const char *name)
    {
    RestXMLParser::EndElement(name);
    }
    
  /// Callback function -- called from XML parser with the character data
  /// for an XML element
  virtual void CharacterDataHandler(const char *inData, int inLength)
    {
     RestXMLParser::CharacterDataHandler(inData,inLength);
    }
  
  /** Set the bitstream object */
  void SetBitstream(mdo::Bitstream* item) {m_Bitstream = item;}
  
protected:

  mdo::Bitstream* m_Bitstream;
  
};


/** Constructor */
Bitstream::Bitstream()
{
  m_Bitstream = NULL;
}
  
/** Destructor */
Bitstream::~Bitstream()
{
}

/** Fetch the object */
bool Bitstream::Fetch()
{
  if(!m_Bitstream)
    {
    std::cerr << "Bitstream::Fecth : Bitstream not set" << std::endl;
    return false;
    }

  if(m_Bitstream->IsFetched())
    {
    return true;
    }
    
  if(m_Bitstream->GetId() == 0)
    {
    std::cerr << "Bitstream::Fetch : BitstreamId not set" << std::endl;
    return false;
    }

  BitstreamXMLParser parser;
  parser.SetBitstream(m_Bitstream);
  m_Bitstream->Clear();
  parser.AddTag("/rsp/name",m_Bitstream->GetName());
  parser.AddTag("/rsp/size",m_Bitstream->GetSize());
  parser.AddTag("/rsp/uuid",m_Bitstream->GetUuid());
  parser.AddTag("/rsp/parent",m_Bitstream->GetParent());
  
  m_WebAPI->GetRestAPI()->SetXMLParser(&parser);
  
  std::stringstream url;
  url << "midas.bitstream.get?id=" << m_Bitstream->GetId();
  if(!m_WebAPI->Execute(url.str().c_str()))
    {
    std::cout << m_WebAPI->GetErrorMessage() << std::endl;
    return false;
    }
  m_Bitstream->SetFetched(true);
  return true;
}

/** Commit the object */
bool Bitstream::Commit()
{
  return true;
}

// Add the object
void Bitstream::SetObject(mdo::Object* object)
{  
  m_Bitstream = reinterpret_cast<mdo::Bitstream*>(object);
}

bool Bitstream::FetchParent()
{
  mdo::Item* parent = new mdo::Item;
  m_Bitstream->SetParentItem(parent);
  parent->SetId(m_Bitstream->GetParentId());

  mws::Item remote;
  remote.SetWebAPI(mws::WebAPI::Instance());
  remote.SetObject(parent);
  return remote.Fetch(); 
}

} // end namespace
