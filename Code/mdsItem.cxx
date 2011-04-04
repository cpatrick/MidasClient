/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mdsItem.h"
#include "mdoItem.h"
#include "mdsBitstream.h"
#include "midasStandardIncludes.h"

namespace mds{

/** Constructor */
Item::Item()
: m_Item(NULL)
{
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
    mds::DatabaseAPI::Instance()->GetLog()->Error("Item::Fetch : Item not set\n");
    return false;
    }
    
  if(m_Item->GetId() == 0)
    {
    mds::DatabaseAPI::Instance()->GetLog()->Error("Item::Fetch : ItemId not set\n");
    return false;
    }

  if(m_Item->IsFetched())
    {
    return true;
    }

  if(m_Item->GetUuid() == "")
    {
    m_Item->SetUuid(mds::DatabaseAPI::Instance()->GetUuid(
      midasResourceType::ITEM, m_Item->GetId()).c_str());
    }

  std::stringstream query;
  query << "SELECT text_value FROM metadatavalue WHERE item_id='"
    << m_Item->GetId() << "' AND metadata_field_id='27'";
  
  mds::DatabaseAPI::Instance()->Open();
  mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str());

  while(mds::DatabaseAPI::Instance()->Database->GetNextRow())
    {
    m_Item->SetAbstract(mds::DatabaseAPI::Instance()->Database->GetValueAsString(0));
    }

  query.str(std::string());
  query << "SELECT text_value FROM metadatavalue WHERE item_id='"
    << m_Item->GetId() << "' AND metadata_field_id='3'";
  mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str());

  while(mds::DatabaseAPI::Instance()->Database->GetNextRow())
    {
    m_Item->AddAuthor(mds::DatabaseAPI::Instance()->Database->GetValueAsString(0));
    }
  
  query.str(std::string());
  query << "SELECT text_value FROM metadatavalue WHERE item_id='"
    << m_Item->GetId() << "' AND metadata_field_id='57'";
  mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str());

  while(mds::DatabaseAPI::Instance()->Database->GetNextRow())
    {
    m_Item->AddKeyword(mds::DatabaseAPI::Instance()->Database->GetValueAsString(0));
    }

  query.str(std::string());
  query << "SELECT text_value FROM metadatavalue WHERE item_id='"
    << m_Item->GetId() << "' AND metadata_field_id='26'";
  mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str());

  while(mds::DatabaseAPI::Instance()->Database->GetNextRow())
    {
    m_Item->SetDescription(mds::DatabaseAPI::Instance()->Database->GetValueAsString(0));
    }

  query.str(std::string());
  query << "SELECT title FROM item WHERE item_id='"
    << m_Item->GetId() << "'";
  mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str());

  while(mds::DatabaseAPI::Instance()->Database->GetNextRow())
    {
    m_Item->SetTitle(mds::DatabaseAPI::Instance()->Database->GetValueAsString(0));
    }

  mds::DatabaseAPI::Instance()->Close();
  m_Item->SetFetched(true);
  return true;
}

/** Commit */
bool Item::Commit()
{
  bool ok = true;

  std::string path = mds::DatabaseAPI::Instance()->GetRecordByUuid(m_Item->GetUuid()).Path;
  std::string parentDir = kwsys::SystemTools::GetParentDirectory(path.c_str());
  std::string oldName = kwsys::SystemTools::GetFilenameName(path);

  if(oldName != m_Item->GetName())
    {
    std::string newPath = parentDir + "/" + m_Item->GetTitle();
    if(rename(path.c_str(), newPath.c_str()) == 0)
      {
      std::stringstream pathQuery;
      pathQuery << "UPDATE resource_uuid SET path='" << newPath <<
        "' WHERE uuid='" << m_Item->GetUuid() << "'";

      mds::DatabaseAPI::Instance()->Open();
      mds::DatabaseAPI::Instance()->Database->ExecuteQuery(pathQuery.str().c_str());
      mds::DatabaseAPI::Instance()->Close();

      this->FetchTree();

      for(std::vector<mdo::Bitstream*>::const_iterator i =
          m_Item->GetBitstreams().begin();
          i != m_Item->GetBitstreams().end(); ++i)
        {
        mds::Bitstream mdsBitstream;
        mdsBitstream.SetObject(*i);
        mdsBitstream.ParentPathChanged(newPath);
        }
      }
    else
      {
      mds::DatabaseAPI::Instance()->GetLog()->Error("Item::Commit : could not rename directory "
        "on disk. It may be locked.\n");
      return false;
      }
    }

  mds::DatabaseAPI::Instance()->Open();
  std::stringstream query;
  query << "DELETE FROM metadatavalue WHERE item_id='" << m_Item->GetId() << "'";
  mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str());
  
  query.str(std::string());
  query << "INSERT INTO metadatavalue (item_id,metadata_field_id,text_value) "
    << "VALUES ('" << m_Item->GetId() << "','27','" <<
    midasUtils::EscapeForSQL(m_Item->GetAbstract()) << "')";
  ok &= mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str());
 
  query.str(std::string());
  query << "UPDATE item SET title='" <<
    midasUtils::EscapeForSQL(m_Item->GetTitle()) << "' WHERE item_id='" <<
    m_Item->GetId() << "'";
  ok &= mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str());

  query.str(std::string());
  query << "INSERT INTO metadatavalue (item_id,metadata_field_id,text_value) "
    << "VALUES ('" << m_Item->GetId() << "','26','" <<
    midasUtils::EscapeForSQL(m_Item->GetDescription()) << "')";
  ok &= mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str());

  for(std::vector<std::string>::iterator i = m_Item->GetAuthors().begin();
      i != m_Item->GetAuthors().end(); ++i)
    {
    query.str(std::string());
    query << "INSERT INTO metadatavalue (item_id,metadata_field_id,text_value)"
      << " VALUES ('" << m_Item->GetId() << "','3','"
      << midasUtils::EscapeForSQL(*i) << "')";
    ok &= mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str());
    }

  for(std::vector<std::string>::iterator i = m_Item->GetKeywords().begin();
      i != m_Item->GetKeywords().end(); ++i)
    {
    query.str(std::string());
    query << "INSERT INTO metadatavalue (item_id,metadata_field_id,text_value)"
      << " VALUES ('" << m_Item->GetId() << "','57','"
      << midasUtils::EscapeForSQL(*i) << "')";
    ok &= mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str());
    }
  mds::DatabaseAPI::Instance()->Close();

  if(ok)
    {
    if(m_MarkDirty)
      {
      mds::DatabaseAPI::Instance()->MarkDirtyResource(m_Item->GetUuid(), midasDirtyAction::MODIFIED);
      }
    return true;
    }
  mds::DatabaseAPI::Instance()->GetLog()->Error("Item::Commit : database update failed");
  return false;
}

bool Item::FetchTree()
{
  if(!m_Item)
    {
    mds::DatabaseAPI::Instance()->GetLog()->Error("Item::FetchTree : Item not set\n");
    return false;
    }
    
  if(m_Item->GetId() == 0)
    {
    mds::DatabaseAPI::Instance()->GetLog()->Error("Item::FetchTree : ItemId not set\n");
    return false;
    }

  if(m_Item->GetUuid() == "")
    {
    m_Item->SetUuid(mds::DatabaseAPI::Instance()->GetUuid(
      midasResourceType::ITEM, m_Item->GetId()).c_str());
    }

  m_Item->SetDirty(mds::DatabaseAPI::Instance()->IsResourceDirty(m_Item->GetUuid()));

  std::stringstream query;
  query << "SELECT bitstream.bitstream_id, bitstream.name, bitstream.size_bytes, resource_uuid.uuid "
    "FROM bitstream, resource_uuid WHERE resource_uuid.resource_id=bitstream.bitstream_id "
    "AND resource_uuid.resource_type_id='" << midasResourceType::BITSTREAM << "' AND "
    "bitstream.bitstream_id IN (SELECT bitstream_id FROM item2bitstream WHERE item_id="
    << m_Item->GetId() << ") ORDER BY bitstream.name COLLATE NOCASE ASC";
  mds::DatabaseAPI::Instance()->Open();
  mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str());

  std::vector<mdo::Bitstream*> bitstreams;
  while(mds::DatabaseAPI::Instance()->Database->GetNextRow())
    {
    mdo::Bitstream* bitstream = new mdo::Bitstream;
    bitstream->SetId(mds::DatabaseAPI::Instance()->Database->GetValueAsInt(0));
    bitstream->SetName(mds::DatabaseAPI::Instance()->Database->GetValueAsString(1));
    std::stringstream val;
    val << mds::DatabaseAPI::Instance()->Database->GetValueAsInt64(2);
    bitstream->SetSize(val.str());
    bitstream->SetUuid(mds::DatabaseAPI::Instance()->Database->GetValueAsString(3));
    bitstream->SetFetched(true);
    m_Item->AddBitstream(bitstream);
    bitstreams.push_back(bitstream);
    }
  mds::DatabaseAPI::Instance()->Close();

  for(std::vector<mdo::Bitstream*>::iterator i = bitstreams.begin();
      i != bitstreams.end(); ++i)
    {
    (*i)->SetDirty(mds::DatabaseAPI::Instance()->IsResourceDirty((*i)->GetUuid()));
    }
  return true;
}

bool Item::FetchSize()
{
  if(!m_Item)
    {
    return false;
    }
  double total = 0;

  for(std::vector<mdo::Bitstream*>::const_iterator i = m_Item->GetBitstreams().begin();
      i != m_Item->GetBitstreams().end(); ++i)
    {
    total += midasUtils::StringToDouble((*i)->GetSize());
    }
  std::stringstream sizeStr;
  sizeStr << total;
  m_Item->SetSize(sizeStr.str());
  return true;
}

bool Item::Delete(bool deleteOnDisk)
{
  std::vector<int> children;
  std::stringstream query;
  query << "SELECT bitstream_id FROM item2bitstream WHERE "
    "item_id='" << m_Item->GetId() << "'";
  mds::DatabaseAPI::Instance()->Database->Open(mds::DatabaseAPI::Instance()->GetDatabasePath().c_str());
  if(!mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str()))
    {
    mds::DatabaseAPI::Instance()->Database->Close();
    return false;
    }

  while(mds::DatabaseAPI::Instance()->Database->GetNextRow())
    {
    children.push_back(mds::DatabaseAPI::Instance()->Database->GetValueAsInt(0));
    }
  mds::DatabaseAPI::Instance()->Database->Close();
  bool ok = true;
  for(std::vector<int>::iterator i = children.begin();
      i != children.end(); ++i)
    {
    mds::Bitstream mdsBitstream;
    mdo::Bitstream* bitstream = new mdo::Bitstream;
    bitstream->SetId(*i);
    bitstream->SetUuid(mds::DatabaseAPI::Instance()->GetUuid(midasResourceType::BITSTREAM, *i).c_str());
    mdsBitstream.SetObject(bitstream);
    mdsBitstream.SetPath(mds::DatabaseAPI::Instance()->GetRecordByUuid(bitstream->GetUuid()).Path);
    ok &= mdsBitstream.Delete(deleteOnDisk);
    delete bitstream;

    if(!ok)
      {
      return false;
      }
    }

  mds::DatabaseAPI::Instance()->Database->Open(mds::DatabaseAPI::Instance()->GetDatabasePath().c_str());
  mds::DatabaseAPI::Instance()->Database->ExecuteQuery("BEGIN");
  query.str(std::string());
  query << "DELETE FROM item2bitstream WHERE item_id='" <<
    m_Item->GetId() << "'";
  if(!mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str()))
    {
    mds::DatabaseAPI::Instance()->Database->ExecuteQuery("ROLLBACK");
    mds::DatabaseAPI::Instance()->Database->Close();
    return false;
    }

  query.str(std::string());
  query << "DELETE FROM metadatavalue WHERE item_id='" <<
    m_Item->GetId() << "'";
  if(!mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str()))
    {
    mds::DatabaseAPI::Instance()->Database->ExecuteQuery("ROLLBACK");
    mds::DatabaseAPI::Instance()->Database->Close();
    return false;
    }

  query.str(std::string());
  query << "DELETE FROM item WHERE item_id='" <<
    m_Item->GetId() << "'";
  if(!mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str()))
    {
    mds::DatabaseAPI::Instance()->Database->ExecuteQuery("ROLLBACK");
    mds::DatabaseAPI::Instance()->Database->Close();
    return false;
    }

  query.str(std::string());
  query << "DELETE FROM collection2item WHERE item_id='" <<
    m_Item->GetId() << "'";
  if(!mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str()))
    {
    mds::DatabaseAPI::Instance()->Database->ExecuteQuery("ROLLBACK");
    mds::DatabaseAPI::Instance()->Database->Close();
    return false;
    }

  query.str(std::string());
  query << "DELETE FROM dirty_resource WHERE uuid='" <<
    m_Item->GetUuid() << "'";
  if(!mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str()))
    {
    mds::DatabaseAPI::Instance()->Database->ExecuteQuery("ROLLBACK");
    mds::DatabaseAPI::Instance()->Database->Close();
    return false;
    }

  query.str(std::string());
  query << "DELETE FROM resource_uuid WHERE uuid='" <<
    m_Item->GetUuid() << "'";
  if(!mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str()))
    {
    mds::DatabaseAPI::Instance()->Database->ExecuteQuery("ROLLBACK");
    mds::DatabaseAPI::Instance()->Database->Close();
    return false;
    }
  mds::DatabaseAPI::Instance()->Database->ExecuteQuery("COMMIT");
  mds::DatabaseAPI::Instance()->Database->Close();

  if(deleteOnDisk)
    {
    kwsys::SystemTools::RemoveADirectory(this->m_Path.c_str());
    }
  return true;
}

void Item::SetObject(mdo::Object* object)
{  
  m_Item = reinterpret_cast<mdo::Item*>(object);
}

void Item::SetPath(std::string path)
{
  m_Path = path;
}

void Item::ParentPathChanged(std::string parentPath)
{
  std::string newPath = parentPath + "/" + m_Item->GetTitle();
  std::stringstream query;
  query << "UPDATE resource_uuid SET path='" << newPath << "' WHERE "
    "uuid='" << m_Item->GetUuid() << "'";

  mds::DatabaseAPI::Instance()->Open();
  mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str());
  mds::DatabaseAPI::Instance()->Close();

  for(std::vector<mdo::Bitstream*>::const_iterator i =
      m_Item->GetBitstreams().begin();
      i != m_Item->GetBitstreams().end(); ++i)
    {
    mds::Bitstream mdsBitstream;
    mdsBitstream.SetObject(*i);
    mdsBitstream.ParentPathChanged(newPath);
    }
}

} // end namespace
