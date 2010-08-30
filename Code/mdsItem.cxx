/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mdsItem.h"
#include "mdoItem.h"
#include "midasStandardIncludes.h"

namespace mds{

/** Constructor */
Item::Item()
: m_Item(NULL), m_Recurse(true)
{
}
  
/** Destructor */
Item::~Item()
{
}

void Item::SetRecursive(bool recurse)
{
  m_Recurse = recurse;
}
  
/** Fecth */
bool Item::Fetch()
{
  if(!m_Item)
    {
    m_Database->GetLog()->Error("Item::Fetch : Item not set\n");
    return false;
    }
    
  if(m_Item->GetId() == 0)
    {
    m_Database->GetLog()->Error("Item::Fetch : ItemId not set\n");
    return false;
    }

  if(m_Item->IsFetched())
    {
    return true;
    }

  if(m_Item->GetUuid() == "")
    {
    m_Item->SetUuid(m_Database->GetUuid(
      midasResourceType::ITEM, m_Item->GetId()).c_str());
    }

  std::stringstream query;
  query << "SELECT text_value FROM metadatavalue WHERE item_id='"
    << m_Item->GetId() << "' AND metadata_field_id='27'";
  
  m_Database->Open();
  m_Database->GetDatabase()->ExecuteQuery(query.str().c_str());

  while(m_Database->GetDatabase()->GetNextRow())
    {
    m_Item->SetAbstract(m_Database->GetDatabase()->GetValueAsString(0));
    }

  query.str(std::string());
  query << "SELECT text_value FROM metadatavalue WHERE item_id='"
    << m_Item->GetId() << "' AND metadata_field_id='3'";
  m_Database->GetDatabase()->ExecuteQuery(query.str().c_str());

  while(m_Database->GetDatabase()->GetNextRow())
    {
    m_Item->AddAuthor(m_Database->GetDatabase()->GetValueAsString(0));
    }
  
  query.str(std::string());
  query << "SELECT text_value FROM metadatavalue WHERE item_id='"
    << m_Item->GetId() << "' AND metadata_field_id='57'";
  m_Database->GetDatabase()->ExecuteQuery(query.str().c_str());

  while(m_Database->GetDatabase()->GetNextRow())
    {
    m_Item->AddKeyword(m_Database->GetDatabase()->GetValueAsString(0));
    }

  query.str(std::string());
  query << "SELECT text_value FROM metadatavalue WHERE item_id='"
    << m_Item->GetId() << "' AND metadata_field_id='26'";
  m_Database->GetDatabase()->ExecuteQuery(query.str().c_str());

  while(m_Database->GetDatabase()->GetNextRow())
    {
    m_Item->SetDescription(m_Database->GetDatabase()->GetValueAsString(0));
    }

  query.str(std::string());
  query << "SELECT title FROM item WHERE item_id='"
    << m_Item->GetId() << "'";
  m_Database->GetDatabase()->ExecuteQuery(query.str().c_str());

  while(m_Database->GetDatabase()->GetNextRow())
    {
    m_Item->SetTitle(m_Database->GetDatabase()->GetValueAsString(0));
    }

  m_Database->Close();
  m_Item->SetFetched(true);
  return true;
}

/** Commit */
bool Item::Commit()
{
  bool ok = true;
  m_Database->Open();
  std::stringstream query;
  query << "DELETE FROM metadatavalue WHERE item_id='" << m_Item->GetId() << "'";
  m_Database->GetDatabase()->ExecuteQuery(query.str().c_str());
  
  query.str(std::string());
  query << "INSERT INTO metadatavalue (item_id,metadata_field_id,text_value) "
    << "VALUES ('" << m_Item->GetId() << "','27','" <<
    midasUtils::EscapeForSQL(m_Item->GetAbstract()) << "')";
  ok &= m_Database->GetDatabase()->ExecuteQuery(query.str().c_str());
 
  query.str(std::string());
  query << "UPDATE item SET title='" <<
    midasUtils::EscapeForSQL(m_Item->GetTitle()) << "' WHERE item_id='" <<
    m_Item->GetId() << "'";
  ok &= m_Database->GetDatabase()->ExecuteQuery(query.str().c_str());

  query.str(std::string());
  query << "INSERT INTO metadatavalue (item_id,metadata_field_id,text_value) "
    << "VALUES ('" << m_Item->GetId() << "','26','" <<
    midasUtils::EscapeForSQL(m_Item->GetDescription()) << "')";
  ok &= m_Database->GetDatabase()->ExecuteQuery(query.str().c_str());

  for(std::vector<std::string>::iterator i = m_Item->GetAuthors().begin();
      i != m_Item->GetAuthors().end(); ++i)
    {
    query.str(std::string());
    query << "INSERT INTO metadatavalue (item_id,metadata_field_id,text_value)"
      << " VALUES ('" << m_Item->GetId() << "','3','"
      << midasUtils::EscapeForSQL(*i) << "')";
    ok &= m_Database->GetDatabase()->ExecuteQuery(query.str().c_str());
    }

  for(std::vector<std::string>::iterator i = m_Item->GetKeywords().begin();
      i != m_Item->GetKeywords().end(); ++i)
    {
    query.str(std::string());
    query << "INSERT INTO metadatavalue (item_id,metadata_field_id,text_value)"
      << " VALUES ('" << m_Item->GetId() << "','57','"
      << midasUtils::EscapeForSQL(*i) << "')";
    ok &= m_Database->GetDatabase()->ExecuteQuery(query.str().c_str());
    }
  m_Database->Close();

  if(ok)
    {
    if(m_MarkDirty)
      {
      m_Database->MarkDirtyResource(m_Item->GetUuid(), midasDirtyAction::MODIFIED);
      }
    return true;
    }
  m_Database->GetLog()->Error("Item::Commit : database update failed");
  return false;
}

bool Item::FetchTree()
{
  if(!m_Item)
    {
    m_Database->GetLog()->Error("Item::FetchTree : Item not set\n");
    return false;
    }
    
  if(m_Item->GetId() == 0)
    {
    m_Database->GetLog()->Error("Item::FetchTree : ItemId not set\n");
    return false;
    }

  if(m_Item->GetUuid() == "")
    {
    m_Item->SetUuid(m_Database->GetUuid(
      midasResourceType::ITEM, m_Item->GetId()).c_str());
    }

  m_Item->SetDirty(m_Database->IsResourceDirty(m_Item->GetUuid()));

  std::stringstream query;
  query << "SELECT bitstream.bitstream_id, bitstream.name, resource_uuid.uuid "
    "FROM bitstream, resource_uuid WHERE resource_uuid.resource_id=bitstream.bitstream_id "
    "AND resource_uuid.resource_type_id='" << midasResourceType::BITSTREAM << "' AND "
    "bitstream.bitstream_id IN (SELECT bitstream_id FROM item2bitstream WHERE item_id="
    << m_Item->GetId() << ")";
  m_Database->Open();
  m_Database->GetDatabase()->ExecuteQuery(query.str().c_str());

  std::vector<mdo::Bitstream*> bitstreams;
  while(m_Database->GetDatabase()->GetNextRow())
    {
    mdo::Bitstream* bitstream = new mdo::Bitstream;
    bitstream->SetId(m_Database->GetDatabase()->GetValueAsInt(0));
    bitstream->SetName(m_Database->GetDatabase()->GetValueAsString(1));
    bitstream->SetUuid(m_Database->GetDatabase()->GetValueAsString(2));
    m_Item->AddBitstream(bitstream);
    bitstreams.push_back(bitstream);
    }
  m_Database->Close();

  for(std::vector<mdo::Bitstream*>::iterator i = bitstreams.begin();
      i != bitstreams.end(); ++i)
    {
    (*i)->SetDirty(m_Database->IsResourceDirty((*i)->GetUuid()));
    }
  return true;
}

bool Item::Delete()
{
  return true;
}

void Item::SetObject(mdo::Object* object)
{  
  m_Item = reinterpret_cast<mdo::Item*>(object);
}

} // end namespace
