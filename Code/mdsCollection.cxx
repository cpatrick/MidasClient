/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mdsCollection.h"
#include "mdoCollection.h"
#include "mdsItem.h"
#include "midasStandardIncludes.h"

namespace mds{

/** Constructor */
Collection::Collection()
: m_Collection(NULL), m_Recurse(true)
{
}
  
/** Destructor */
Collection::~Collection()
{
}

void Collection::SetRecursive(bool recurse)
{
  m_Recurse = recurse;
}
  
/** Fecth */
bool Collection::Fetch()
{
  if(!m_Collection)
    {
    mds::DatabaseAPI::Instance()->GetLog()->Error("Collection::Fetch : Collection not set\n");
    return false;
    }

  if(m_Collection->GetId() == 0)
    {
    mds::DatabaseAPI::Instance()->GetLog()->Error("Collection::Fetch : CollectionId not set\n");
    return false;
    }

  if(m_Collection->IsFetched())
    {
    return true;
    }

  if(m_Collection->GetUuid() == "")
    {
    m_Collection->SetUuid(mds::DatabaseAPI::Instance()->GetUuid(
      midasResourceType::COLLECTION, m_Collection->GetId()).c_str());
    }

  std::stringstream query;
  query << "SELECT short_description, introductory_text, copyright_text, name "
    "FROM collection WHERE collection_id='" << m_Collection->GetId() << "'";

  mds::DatabaseAPI::Instance()->Open();
  if(!mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str()))
    {
    std::stringstream text;
    text << "Collection::Fetch : Query failed: " << query.str() << std::endl;
    mds::DatabaseAPI::Instance()->GetLog()->Error(text.str());
    mds::DatabaseAPI::Instance()->Close();
    return false;
    }

  while(mds::DatabaseAPI::Instance()->Database->GetNextRow())
    {
    m_Collection->SetDescription(
      mds::DatabaseAPI::Instance()->Database->GetValueAsString(0));
    m_Collection->SetIntroductoryText(
      mds::DatabaseAPI::Instance()->Database->GetValueAsString(1));
    m_Collection->SetCopyright(
      mds::DatabaseAPI::Instance()->Database->GetValueAsString(2));
    m_Collection->SetName(
      mds::DatabaseAPI::Instance()->Database->GetValueAsString(3));
    }
  m_Collection->SetFetched(true);
  mds::DatabaseAPI::Instance()->Close();
  return true;
}

bool Collection::FetchSize()
{
  if(!m_Collection)
    {
    return false;
    }
  double total = 0;

  for(std::vector<mdo::Item*>::const_iterator i = m_Collection->GetItems().begin();
      i != m_Collection->GetItems().end(); ++i)
    {
    if((*i)->GetSize() == "")
      {
      mds::Item mdsItem;
      mdsItem.SetObject(*i);
      mdsItem.FetchSize();
      }
    total += midasUtils::StringToDouble((*i)->GetSize());
    }
  std::stringstream sizeStr;
  sizeStr << total;
  m_Collection->SetSize(sizeStr.str());
  return true;
}

/** Commit */
bool Collection::Commit()
{
  if(!m_Collection)
    {
    mds::DatabaseAPI::Instance()->GetLog()->Error("Collection::Commit : Collection not set\n");
    return false;
    }

  if(m_Collection->GetId() == 0)
    {
    mds::DatabaseAPI::Instance()->GetLog()->Error("Collection::Commit : Collection not set\n");
    return false;
    }

  if(m_Collection->GetUuid() == "")
    {
    m_Collection->SetUuid(mds::DatabaseAPI::Instance()->GetUuid(
      midasResourceType::COLLECTION, m_Collection->GetId()).c_str());
    }

  std::string path = mds::DatabaseAPI::Instance()->GetRecordByUuid(m_Collection->GetUuid()).Path;
  std::string parentDir = kwsys::SystemTools::GetParentDirectory(path.c_str());
  std::string oldName = kwsys::SystemTools::GetFilenameName(path);

  if(oldName != m_Collection->GetName())
    {
    std::string newPath = parentDir + "/" + m_Collection->GetName();
    if(rename(path.c_str(), newPath.c_str()) == 0)
      {
      std::stringstream pathQuery;
      pathQuery << "UPDATE resource_uuid SET path='" << newPath <<
        "' WHERE uuid='" << m_Collection->GetUuid() << "'";

      mds::DatabaseAPI::Instance()->Open();
      mds::DatabaseAPI::Instance()->Database->ExecuteQuery(pathQuery.str().c_str());
      mds::DatabaseAPI::Instance()->Close();

      this->FetchTree();

      for(std::vector<mdo::Item*>::const_iterator i =
          m_Collection->GetItems().begin();
          i != m_Collection->GetItems().end(); ++i)
        {
        mds::Item mdsItem;
        mdsItem.SetObject(*i);
        mdsItem.ParentPathChanged(newPath);
        }
      }
    else
      {
      mds::DatabaseAPI::Instance()->GetLog()->Error("Collection::Commit : could not rename "
        "directory on disk. It may be locked.\n");
      return false;
      }
    }

  std::stringstream query;
  query << "UPDATE collection SET " <<
    "name='" <<
    midasUtils::EscapeForSQL(m_Collection->GetName()) << "', "
    "short_description='" <<
    midasUtils::EscapeForSQL(m_Collection->GetDescription()) << "', "
    "introductory_text='" <<
    midasUtils::EscapeForSQL(m_Collection->GetIntroductoryText()) << "', "
    "copyright_text='" <<
    midasUtils::EscapeForSQL(m_Collection->GetCopyright()) << "'" <<
    " WHERE collection_id='" << m_Collection->GetId() << "'";

  mds::DatabaseAPI::Instance()->Open();
  if(mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str()))
    {
    mds::DatabaseAPI::Instance()->Close();
    if(m_MarkDirty)
      {
      mds::DatabaseAPI::Instance()->MarkDirtyResource(m_Collection->GetUuid(), midasDirtyAction::MODIFIED);
      }
    return true;
    }
  std::stringstream text;
  text << "Collection::Commit : Query failed: " << query.str() << std::endl;
  mds::DatabaseAPI::Instance()->GetLog()->Error(text.str());
  mds::DatabaseAPI::Instance()->Close();
  return false;
}

bool Collection::FetchTree()
{
  if(!m_Collection)
    {
    mds::DatabaseAPI::Instance()->GetLog()->Error("Collection::FetchTree : Collection not set\n");
    return false;
    }

  if(m_Collection->GetId() == 0)
    {
    mds::DatabaseAPI::Instance()->GetLog()->Error("Collection::FetchTree : Collection not set\n");
    return false;
    }

  if(m_Collection->GetUuid() == "")
    {
    m_Collection->SetUuid(mds::DatabaseAPI::Instance()->GetUuid(
      midasResourceType::COLLECTION, m_Collection->GetId()).c_str());
    }
  
  m_Collection->SetDirty(mds::DatabaseAPI::Instance()->IsResourceDirty(m_Collection->GetUuid()));

  std::stringstream query;
  query << "SELECT item.item_id, item.title, resource_uuid.uuid FROM item, resource_uuid "
    "WHERE resource_uuid.resource_type_id='" << midasResourceType::ITEM << "' AND "
    "resource_uuid.resource_id=item.item_id AND item.item_id IN (SELECT item_id FROM "
    "collection2item WHERE collection_id=" << m_Collection->GetId() << ") "
    "ORDER BY item.title COLLATE NOCASE ASC";
  mds::DatabaseAPI::Instance()->Open();
  mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str());

  std::vector<mdo::Item*> items;
  while(mds::DatabaseAPI::Instance()->Database->GetNextRow())
    {
    mdo::Item* item = new mdo::Item;
    item->SetId(mds::DatabaseAPI::Instance()->Database->GetValueAsInt(0));
    item->SetTitle(mds::DatabaseAPI::Instance()->Database->GetValueAsString(1));
    item->SetUuid(mds::DatabaseAPI::Instance()->Database->GetValueAsString(2));
    m_Collection->AddItem(item);
    items.push_back(item);
    }
  mds::DatabaseAPI::Instance()->Close();

  if(m_Recurse)
    {
    for(std::vector<mdo::Item*>::iterator i = items.begin();
        i != items.end(); ++i)
      {
      mds::Item mdsItem;
      mdsItem.SetObject(*i);
      if(!mdsItem.FetchTree())
        {
        return false;
        }
      }
    }
  return true;
}

bool Collection::Delete(bool deleteOnDisk)
{
  std::vector<int> children;
  std::stringstream query;
  query << "SELECT item_id FROM collection2item WHERE "
    "collection_id='" << m_Collection->GetId() << "'";
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
    mds::Item mdsItem;
    mdo::Item* item = new mdo::Item;
    item->SetId(*i);
    item->SetUuid(mds::DatabaseAPI::Instance()->GetUuid(midasResourceType::ITEM, *i).c_str());
    mdsItem.SetObject(item);
    mdsItem.SetPath(mds::DatabaseAPI::Instance()->GetRecordByUuid(item->GetUuid()).Path);
    ok &= mdsItem.Delete(deleteOnDisk);
    delete item;

    if(!ok)
      {
      return false;
      }
    }

  mds::DatabaseAPI::Instance()->Database->Open(mds::DatabaseAPI::Instance()->GetDatabasePath().c_str());
  mds::DatabaseAPI::Instance()->Database->ExecuteQuery("BEGIN");
  query.str(std::string());
  query << "DELETE FROM collection2item WHERE collection_id='" <<
    m_Collection->GetId() << "'";
  if(!mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str()))
    {
    mds::DatabaseAPI::Instance()->Database->ExecuteQuery("ROLLBACK");
    mds::DatabaseAPI::Instance()->Database->Close();
    return false;
    }

  query.str(std::string());
  query << "DELETE FROM collection WHERE collection_id='" <<
    m_Collection->GetId() << "'";
  if(!mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str()))
    {
    mds::DatabaseAPI::Instance()->Database->ExecuteQuery("ROLLBACK");
    mds::DatabaseAPI::Instance()->Database->Close();
    return false;
    }

  query.str(std::string());
  query << "DELETE FROM community2collection WHERE collection_id='" <<
    m_Collection->GetId() << "'";
  if(!mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str()))
    {
    mds::DatabaseAPI::Instance()->Database->ExecuteQuery("ROLLBACK");
    mds::DatabaseAPI::Instance()->Database->Close();
    return false;
    }

  query.str(std::string());
  query << "DELETE FROM dirty_resource WHERE uuid='" <<
    m_Collection->GetUuid() << "'";
  if(!mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str()))
    {
    mds::DatabaseAPI::Instance()->Database->ExecuteQuery("ROLLBACK");
    mds::DatabaseAPI::Instance()->Database->Close();
    return false;
    }

  query.str(std::string());
  query << "DELETE FROM resource_uuid WHERE uuid='" <<
    m_Collection->GetUuid() << "'";
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

void Collection::SetObject(mdo::Object* object)
{  
  m_Collection = reinterpret_cast<mdo::Collection*>(object);
}

void Collection::SetPath(std::string path)
{
  m_Path = path;
}

void Collection::ParentPathChanged(std::string parentPath)
{
  std::string newPath = parentPath + "/" + m_Collection->GetName();
  std::stringstream query;
  query << "UPDATE resource_uuid SET path='" << newPath << "' WHERE "
    "uuid='" << m_Collection->GetUuid() << "'";

  mds::DatabaseAPI::Instance()->Open();
  mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str());
  mds::DatabaseAPI::Instance()->Close();

  for(std::vector<mdo::Item*>::const_iterator i =
      m_Collection->GetItems().begin();
      i != m_Collection->GetItems().end(); ++i)
    {
    mds::Item mdsItem;
    mdsItem.SetObject(*i);
    mdsItem.ParentPathChanged(newPath);
    }
}

} // end namespace
