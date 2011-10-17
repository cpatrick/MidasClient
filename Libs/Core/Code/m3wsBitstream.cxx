/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "m3wsBitstream.h"
#include "m3doBitstream.h"
#include "m3doItem.h"
#include "m3wsItem.h"
#include "mwsRestResponseParser.h"

#include <QScriptEngine>
#include <QScriptValueIterator>

namespace m3ws
{

// ------------------------------------------------------------------

/** Constructor */
Bitstream::Bitstream()
{
  m_Bitstream = NULL;
}

/** Destructor */
Bitstream::~Bitstream()
{
}

// Set the object
void Bitstream::SetObject(mdo::Object* object)
{
  m_Bitstream = static_cast<m3do::Bitstream *>(object);
}

/** Fetch */
bool Bitstream::Fetch()
{
  // this get method also fetches the subtree, so the behavior is the same
  return this->FetchTree();
}

bool Bitstream::FetchTree()
{
  if( !m_Bitstream )
    {
    std::cerr << "Bitstream::Fetch() : Bitstream not set" << std::endl;
    return false;
    }

  if( m_Bitstream->IsFetched() )
    {
    return true;
    }

  if( !m_Bitstream->GetId() )
    {
    std::cerr << "Bitstream::Fetch() : Bitstream id not set" << std::endl;
    return false;
    }

  mws::RestResponseParser parser;
  m_Bitstream->Clear();
  parser.AddTag("name", m_Bitstream->GetName() );
  parser.AddTag("checksum", m_Bitstream->GetChecksum() );
  // parser.AddTag("uuid", m_Bitstream->GetUuid());
  parser.AddTag("item_id", m_Bitstream->GetParentStr() );
  parser.AddTag("size", m_Bitstream->GetSize() );
  // parser.AddTag("hasAgreement", m_Item->RefAgreement());

  std::stringstream url;
  url << "midas.bitstream.get&id=" << m_Bitstream->GetId();
  if( !mws::WebAPI::Instance()->Execute(url.str().c_str(), &parser) )
    {
    return false;
    }
  m_Bitstream->SetFetched(true);
  return true;
}

bool Bitstream::FetchParent()
{
  int id = m_Bitstream->GetParentId();

  if( id )
    {
    m3do::Item* parent = new m3do::Item;
    m_Bitstream->SetParentItem(parent);
    parent->SetId(id);

    m3ws::Item remote;
    remote.SetObject(parent);
    return remote.Fetch();
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
  if( !mws::WebAPI::Instance()->Execute(url.str().c_str() ) )
    {
    return false;
    }
  return true;
}

bool Bitstream::Commit()
{
  return true; // NOP
}

bool Bitstream::Download()
{
  std::stringstream fields;

  fields << "midas.bitstream.download&id=" << m_Bitstream->GetId();

  return mws::WebAPI::Instance()->DownloadFile(fields.str().c_str(),
                                               m_Bitstream->GetName().c_str(), 0); //
                                                                                   // TODO
                                                                                   // set
                                                                                   // partial
                                                                                   // download
                                                                                   // offset
}

bool Bitstream::Upload()
{
  std::string             uploadToken;
  mws::RestResponseParser parser;

  parser.AddTag("token", uploadToken);

  std::stringstream fields;
  fields << "midas.upload.generatetoken&itemid="
         << m_Bitstream->GetParentId()
         << "&checksum=" << m_Bitstream->GetChecksum()
         << "&filename=" << m_Bitstream->GetName();
  if( !mws::WebAPI::Instance()->Execute(fields.str().c_str(), &parser) )
    {
    return false;
    }
  if( uploadToken == "" ) // server already has this file, upload is not
                          // necessary
    {
    return true;
    }

  /*mds::PartialUpload partial;
  partial.SetToken(uploadToken);
  partial.SetBitstreamId(m_Bitstream->GetId());
  partial.SetUserId(atoi(userId.c_str()));
  partial.SetParentItem(m_Bitstream->GetParentId());
  partial.Commit();*/

  fields.str(std::string() );
  fields << "midas.upload.perform&mode=stream&revision=head"
  "&itemid=" << m_Bitstream->GetParentId()
         << "&filename=" << m_Bitstream->GetName()
         << "&length=" << m_Bitstream->GetSize()
         << "&uploadtoken=" << uploadToken;

  if( !mws::WebAPI::Instance()->UploadFile(fields.str().c_str(),
                                           m_Bitstream->GetPath().c_str() ) )
    {
    return false;
    }

  // partial.Remove();
  return true;
}

} // end namespace
