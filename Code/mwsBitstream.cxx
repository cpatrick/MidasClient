/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "mwsBitstream.h"
#include "mdoBitstream.h"
#include "mdoAssetstore.h"
#include "mdoItem.h"
#include "mwsItem.h"
#include <sstream>
#include <iostream>
#include "mwsRestXMLParser.h"
#include "mwsWebAPI.h"
#include "mwsMirrorHandler.h"

namespace mws{

/** Custom XML parser: midas.bitstream.locations */
class BitstreamLocationXMLParser : public RestXMLParser
{
public:
   
  BitstreamLocationXMLParser()
    {
    m_Bitstream = NULL;
    m_Location = NULL;
    m_CurrentValue = "";
    };
  ~BitstreamLocationXMLParser() {};  

  // Callback function called from XML parser with start-of-element
  virtual void StartElement(const char* name, const char** atts)
    {
    RestXMLParser::StartElement(name,atts);
    m_CurrentValue = "";

    if(!strcmp(name, "data"))
      {
      m_Location = new mdo::Assetstore;
      }
    }

  // Callback function called from XML parser when ending tag encountered
  virtual void EndElement(const char *name)
    {
    if(!strcmp(name, "data"))
      {
      m_Bitstream->AddLocation(m_Location);
      }
    else if(!strcmp(name, "id"))
      {
      m_Location->SetId(atoi(m_CurrentValue.c_str()));
      }
    else if(!strcmp(name, "internal_id"))
      {
      m_Location->SetInternalId(m_CurrentValue);
      }
    else if(!strcmp(name, "path"))
      {
      m_Location->SetPath(m_CurrentValue);
      }
    else if(!strcmp(name, "name"))
      {
      m_Location->SetName(m_CurrentValue);
      }
    else if(!strcmp(name, "type"))
      {
      m_Location->SetType(
        mdo::Assetstore::AssetstoreType(atoi(m_CurrentValue.c_str())));
      }
    else if(!strcmp(name, "enabled"))
      {
      m_Location->SetEnabled(bool(atoi(m_CurrentValue.c_str())));
      }
    m_Bitstream->AddLocation(m_Location);
    RestXMLParser::EndElement(name);
    }

  // Callback function called from XML parser with the character data
  virtual void CharacterDataHandler(const char* inData, int inLength)
    {
    RestXMLParser::CharacterDataHandler(inData,inLength);
    m_CurrentValue.append(inData, inLength);
    }

  void SetBitstream(mdo::Bitstream* bitstream) { m_Bitstream = bitstream; }

protected:
  mdo::Bitstream*  m_Bitstream;
  mdo::Assetstore* m_Location;
  std::string      m_CurrentValue;
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

  RestXMLParser parser;
  m_Bitstream->Clear();
  parser.AddTag("/rsp/name",m_Bitstream->GetName());
  parser.AddTag("/rsp/size",m_Bitstream->GetSize());
  parser.AddTag("/rsp/uuid",m_Bitstream->GetUuid());
  parser.AddTag("/rsp/parent",m_Bitstream->GetParent());
  parser.AddTag("/rsp/hasAgreement",m_Bitstream->RefAgreement());
  
  std::stringstream url;
  url << "midas.bitstream.get?id=" << m_Bitstream->GetId();
  if(!WebAPI::Instance()->Execute(url.str().c_str(), &parser))
    {
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

  Item remote;
  remote.SetObject(parent);
  return remote.Fetch(); 
}

bool Bitstream::FetchLocations()
{
  BitstreamLocationXMLParser parser;
  parser.SetBitstream(m_Bitstream);

  std::stringstream url;
  url << "midas.bitstream.locations?id=" << m_Bitstream->GetId();
  if(!WebAPI::Instance()->Execute(url.str().c_str(), &parser))
    {
    return false;
    }
  return true;
}

bool Bitstream::Delete()
{
  if(!m_Bitstream)
    {
    std::cerr << "Bitstream::Delete() : Bitstream not set" << std::endl;
    return false;
    }

  if(!m_Bitstream->GetId())
    {
    std::cerr << "Bitstream::Delete() : Bitstream id not set" << std::endl;
    return false;
    }

  std::stringstream url;
  url << "midas.bitstream.delete?id=" << m_Bitstream->GetId();
  if(!WebAPI::Instance()->Execute(url.str().c_str()))
    {
    return false;
    }
  return true;
}

//-------------------------------------------------------------------
bool Bitstream::Download()
{
  this->FetchLocations();

  std::stringstream fields;
  fields << "midas.bitstream.download?id=" << m_Bitstream->GetId();

  if(m_Bitstream->GetLocations().size() > 1 &&
    WebAPI::Instance()->GetMirrorHandler())
    {
    mdo::Assetstore* location =
      WebAPI::Instance()->GetMirrorHandler()->HandleMirroredBitstream(
      m_Bitstream);
    fields << "&location=" << location->GetId();
    }
  return WebAPI::Instance()->DownloadFile(fields.str().c_str(),
    m_Bitstream->GetName().c_str());
}

//-------------------------------------------------------------------
bool Bitstream::Upload()
{
  std::stringstream fields;
  fields << "midas.upload.bitstream?uuid=" << m_Bitstream->GetUuid() <<
    "&itemid=" << m_Bitstream->GetParentId() << "&mode=stream&filename=" <<
    midasUtils::EscapeForURL(m_Bitstream->GetName()) << "&path=" <<
    midasUtils::EscapeForURL(m_Bitstream->GetName()) << "&size=" <<
    m_Bitstream->GetSize();
  return WebAPI::Instance()->UploadFile(fields.str().c_str(),
    m_Bitstream->GetPath().c_str());
}

} // end namespace
