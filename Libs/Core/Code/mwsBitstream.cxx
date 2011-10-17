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
#include "mwsRestResponseParser.h"
#include "mwsWebAPI.h"
#include "mwsMirrorHandler.h"
#include "mdsPartialUpload.h"

#include <QScriptEngine>
#include <QScriptValueIterator>

namespace mws
{

/** Custom Response parser: midas.bitstream.locations */
class BitstreamLocationResponseParser : public RestResponseParser
{
public:

  BitstreamLocationResponseParser()
  {
    m_Bitstream = NULL;
  }

  ~BitstreamLocationResponseParser()
  {
  }

  virtual bool Parse(const QString& response)
  {
    if( !RestResponseParser::Parse(response) )
      {
      return false;
      }

    QScriptEngine engine;
    QScriptValue  data = engine.evaluate(response).property("data");
    if( data.isArray() )
      {
      QScriptValueIterator locations(data);
      while( locations.hasNext() )
        {
        locations.next();
        mdo::Assetstore* location = new mdo::Assetstore;
        location->SetId(locations.value().property("id").toInt32() );
        location->SetInternalId(locations.value().property("internal_id").toString().toStdString() );
        location->SetName(locations.value().property("name").toString().toStdString() );
        location->SetPath(locations.value().property("path").toString().toStdString() );
        location->SetEnabled(locations.value().property("enabled").toInt32() == 1);
        location->SetType(mdo::Assetstore::AssetstoreType(
                            locations.value().property("type").toInt32() ) );

        m_Bitstream->AddLocation(location);
        }
      }
    return true;
  }

  void SetBitstream(mdo::Bitstream* bitstream)
  {
    m_Bitstream = bitstream;
  }
protected:
  mdo::Bitstream* m_Bitstream;
};

/** Constructor */
Bitstream::Bitstream()
{
  m_Bitstream = NULL;
  m_Offset = 0;
}

/** Destructor */
Bitstream::~Bitstream()
{
}

/** Fetch the object */
bool Bitstream::Fetch()
{
  if( !m_Bitstream )
    {
    std::cerr << "Bitstream::Fecth : Bitstream not set" << std::endl;
    return false;
    }

  if( m_Bitstream->IsFetched() )
    {
    return true;
    }

  if( m_Bitstream->GetId() == 0 )
    {
    std::cerr << "Bitstream::Fetch : BitstreamId not set" << std::endl;
    return false;
    }

  RestResponseParser parser;
  m_Bitstream->Clear();
  parser.AddTag("name", m_Bitstream->GetName() );
  parser.AddTag("size", m_Bitstream->GetSize() );
  parser.AddTag("uuid", m_Bitstream->GetUuid() );
  parser.AddTag("parent", m_Bitstream->GetParentStr() );
  parser.AddTag("hasAgreement", m_Bitstream->RefAgreement() );

  std::stringstream url;
  url << "midas.bitstream.get&id=" << m_Bitstream->GetId();
  if( !WebAPI::Instance()->Execute(url.str().c_str(), &parser) )
    {
    return false;
    }
  m_Bitstream->SetFetched(true);
  return true;
}

/** Commit the object */
bool Bitstream::Commit()
{
  // No metadata associated with bitstreams, so a commit is just an upload
  return this->Upload();
}

void Bitstream::SetObject(mdo::Object* object)
{
  m_Bitstream = reinterpret_cast<mdo::Bitstream *>(object);
}

void Bitstream::SetOffset(int64 offset)
{
  m_Offset = offset;
}

bool Bitstream::FetchParent()
{
  mdo::Item* parent = new mdo::Item;

  m_Bitstream->SetParentItem(parent);
  parent->SetId(m_Bitstream->GetParentId() );

  Item remote;
  remote.SetObject(parent);
  return remote.Fetch();
}

bool Bitstream::FetchLocations()
{
  BitstreamLocationResponseParser parser;

  parser.SetBitstream(m_Bitstream);

  std::stringstream url;
  url << "midas.bitstream.locations&id=" << m_Bitstream->GetId();
  if( !WebAPI::Instance()->Execute(url.str().c_str(), &parser) )
    {
    return false;
    }
  return true;
}

bool Bitstream::Delete()
{
  if( !m_Bitstream )
    {
    std::cerr << "Bitstream::Delete() : Bitstream not set" << std::endl;
    return false;
    }

  if( !m_Bitstream->GetId() )
    {
    std::cerr << "Bitstream::Delete() : Bitstream id not set" << std::endl;
    return false;
    }

  std::stringstream url;
  url << "midas.bitstream.delete&id=" << m_Bitstream->GetId();
  if( !WebAPI::Instance()->Execute(url.str().c_str() ) )
    {
    return false;
    }
  return true;
}

// -------------------------------------------------------------------
bool Bitstream::Download()
{
  this->FetchLocations();

  std::stringstream fields;
  fields << "midas.bitstream.download&id=" << m_Bitstream->GetId();

  if( m_Bitstream->GetLocations().size() > 1 &&
      WebAPI::Instance()->GetMirrorHandler() )
    {
    mdo::Assetstore* location =
      WebAPI::Instance()->GetMirrorHandler()->HandleMirroredBitstream(
        m_Bitstream);
    if( location != NULL )
      {
      fields << "&location=" << location->GetId();
      }
    }
  return WebAPI::Instance()->DownloadFile(fields.str().c_str(),
                                          m_Bitstream->GetName().c_str(), m_Offset);
}

// -------------------------------------------------------------------
bool Bitstream::Upload()
{
  std::string        uploadToken, userId;
  RestResponseParser parser;

  parser.AddTag("token", uploadToken);
  parser.AddTag("userid", userId);

  std::stringstream fields;
  fields << "midas.upload.generatetoken&itemid="
         << m_Bitstream->GetParentId()
         << "&filename=" << m_Bitstream->GetName();
  if( !WebAPI::Instance()->Execute(fields.str().c_str(), &parser)
      || userId == "" || uploadToken == "" )
    {
    return false;
    }

  mds::PartialUpload partial;
  partial.SetToken(uploadToken);
  partial.SetBitstreamId(m_Bitstream->GetId() );
  partial.SetUserId(atoi(userId.c_str() ) );
  partial.SetParentItem(m_Bitstream->GetParentId() );
  partial.Commit();

  fields.str(std::string() );
  fields << "midas.upload.bitstream&uuid=" << m_Bitstream->GetUuid()
         << "&mode=stream&itemid=" << m_Bitstream->GetParentId()
         << "&filename=" << m_Bitstream->GetName()
         << "&length=" << m_Bitstream->GetSize()
         << "&userid=" << userId
         << "&uploadtoken=" << uploadToken;

  if( !WebAPI::Instance()->UploadFile(fields.str().c_str(),
                                      m_Bitstream->GetPath().c_str() ) )
    {
    return false;
    }

  partial.Remove();
  return true;
}

bool Bitstream::ResumeUpload(const std::string& token, int userId)
{
  std::stringstream fields;

  fields << "midas.upload.bitstream&uuid=" << m_Bitstream->GetUuid()
         << "&mode=stream&itemid=" << m_Bitstream->GetParentId()
         << "&filename=" << m_Bitstream->GetName()
         << "&length=" << m_Bitstream->GetSize()
         << "&userid=" << userId
         << "&uploadtoken=" << token;

  if( !WebAPI::Instance()->UploadFile(fields.str().c_str(),
                                      m_Bitstream->GetPath().c_str(), m_Offset) )
    {
    return false;
    }
  return true;
}

} // end namespace
