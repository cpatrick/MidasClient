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
  mds::DatabaseAPI db;
  if(!m_Collection)
    {
    db.GetLog()->Error("Collection::Fetch : Collection not set\n");
    return false;
    }

  if(m_Collection->GetId() == 0)
    {
    db.GetLog()->Error("Collection::Fetch : CollectionId not set\n");
    return false;
    }

  if(m_Collection->IsFetched())
    {
    return true;
    }

  if(m_Collection->GetUuid() == "")
    {
    m_Collection->SetUuid(db.GetUuid(
      midasResourceType::COLLECTION, m_Collection->GetId()).c_str());
    }

  std::stringstream query;
  query << "SELECT short_description, introductory_text, copyright_text, name "
    "FROM collection WHERE collection_id='" << m_Collection->GetId() << "'";

  db.Open();
  if(!db.Database->ExecuteQuery(query.str().c_str()))
    {
    std::stringstream text;
    text << "Collection::Fetch : Query failed: " << query.str() << std::endl;
    db.GetLog()->Error(text.str());
    db.Close();
    return false;
    }

  while(db.Database->GetNextRow())
    {
    m_Collection->SetDescription(
      db.Database->GetValueAsString(0));
    m_Collection->SetIntroductoryText(
      db.Database->GetValueAsString(1));
    m_Collection->SetCopyright(
      db.Database->GetValueAsString(2));
    m_Collection->SetName(
      db.Database->GetValueAsString(3));
    }
  m_Collection->SetFetched(true);
  db.Close();
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
  mds::DatabaseAPI db;
  if(!m_Collection)
    {
    db.GetLog()->Error("Collection::Commit : Collection not set\n");
    return false;
    }

  if(m_Collection->GetId() == 0)
    {
    db.GetLog()->Error("Collection::Commit : Collection not set\n");
    return false;
    }

  if(m_Collection->GetUuid() == "")
    {
    m_Collection->SetUuid(db.GetUuid(
      midasResourceType::COLLECTION, m_Collection->GetId()).c_str());
    }

  std::string path = db.GetRecordByUuid(m_Collection->GetUuid()).Path;
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

      db.Open();
      db.Database->ExecuteQuery(pathQuery.str().c_str());
      db.Close();

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
      db.GetLog()->Error("Collection::Commit : could not rename "
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

  db.Open();
  if(db.Database->ExecuteQuery(query.str().c_str()))
    {
    db.Close();
    if(m_MarkDirty)
      {
      db.MarkDirtyResource(m_Collection->GetUuid(), midasDirtyAction::MODIFIED);
      }
    return true;
    }
  std::stringstream text;
  text << "Collection::Commit : Query failed: " << query.str() << std::endl;
  db.GetLog()->Error(text.str());
  db.Close();
  return false;
}

bool Collection::FetchTree()
{
  mds::DatabaseAPI db;
  if(!m_Collection)
    {
    db.GetLog()->Error("Collection::FetchTree : Collection not set\n");
    return false;
    }

  if(m_Collection->GetId() == 0)
    {
    db.GetLog()->Error("Collection::FetchTree : Collection not set\n");
    return false;
    }

  if(m_Collection->GetUuid() == "")
    {
    m_Collection->SetUuid(db.GetUuid(
      midasResourceType::COLLECTION, m_Collection->GetId()).c_str());
    }
  
  m_Collection->SetDirty(db.IsResourceDirty(m_Collection->GetUuid()));

  std::stringstream query;
  query << "SELECT item.item_id, item.title, resource_uuid.uuid FROM item, resource_uuid "
    "WHERE resource_uuid.resource_type_id='" << midasResourceType::ITEM << "' AND "
    "resource_uuid.resource_id=item.item_id AND item.item_id IN (SELECT item_id FROM "
    "collection2item WHERE collection_id=" << m_Collection->GetId() << ") "
    "ORDER BY item.title COLLATE NOCASE ASC";
  db.Open();
  db.Database->ExecuteQuery(query.str().c_str());

  std::vector<mdo::Item*> items;
  while(db.Database->GetNextRow())
    {
    mdo::Item* item = new mdo::Item;
    item->SetId(db.Database->GetValueAsInt(0));
    item->SetTitle(db.Database->GetValueAsString(1));
    item->SetUuid(db.Database->GetValueAsString(2));
    m_Collection->AddItem(item);
    items.push_back(item);
    }
  db.Close();
  for(std::vector<mdo::Item*>::const_iterator i =
      m_Collection->GetItems().begin();
      i != m_Collection->GetItems().end(); ++i)
    {
    (*i)->SetDirty(db.IsResourceDirty((*i)->GetUuid()));
    }

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
  mds::DatabaseAPI db;
  std::vector<int> children;
  std::stringstream query;
  query << "SELECT item_id FROM collection2item WHERE "
    "collection_id='" << m_Collection->GetId() << "'";
  db.Open();
  if(!db.Database->ExecuteQuery(query.str().c_str()))
    {
    db.Database->Close();
    return false;
    }

  while(db.Database->GetNextRow())
    {
    children.push_back(db.Database->GetValueAsInt(0));
    }
  db.Database->Close();
  bool ok = true;
  for(std::vector<int>::iterator i = children.begin();
      i != children.end(); ++i)
    {
    mds::Item mdsItem;
    mdo::Item* item = new mdo::Item;
    item->SetId(*i);
    item->SetUuid(db.GetUuid(midasResourceType::ITEM, *i).c_str());
    mdsItem.SetObject(item);
    mdsItem.SetPath(db.GetRecordByUuid(item->GetUuid()).Path);
    ok &= mdsItem.Delete(deleteOnDisk);
    delete item;

    if(!ok)
      {
      return false;
      }
    }

  db.Open();
  db.Database->ExecuteQuery("BEGIN");
  query.str(std::string());
  query << "DELETE FROM collection2item WHERE collection_id='" <<
    m_Collection->GetId() << "'";
  if(!db.Database->ExecuteQuery(query.str().c_str()))
    {
    db.Database->ExecuteQuery("ROLLBACK");
    db.Database->Close();
    return false;
    }

  query.str(std::string());
  query << "DELETE FROM collection WHERE collection_id='" <<
    m_Collection->GetId() << "'";
  if(!db.Database->ExecuteQuery(query.str().c_str()))
    {
    db.Database->ExecuteQuery("ROLLBACK");
    db.Database->Close();
    return false;
    }

  query.str(std::string());
  query << "DELETE FROM community2collection WHERE collection_id='" <<
    m_Collection->GetId() << "'";
  if(!db.Database->ExecuteQuery(query.str().c_str()))
    {
    db.Database->ExecuteQuery("ROLLBACK");
    db.Database->Close();
    return false;
    }

  query.str(std::string());
  query << "DELETE FROM dirty_resource WHERE uuid='" <<
    m_Collection->GetUuid() << "'";
  if(!db.Database->ExecuteQuery(query.str().c_str()))
    {
    db.Database->ExecuteQuery("ROLLBACK");
    db.Database->Close();
    return false;
    }

  query.str(std::string());
  query << "DELETE FROM resource_uuid WHERE uuid='" <<
    m_Collection->GetUuid() << "'";
  if(!db.Database->ExecuteQuery(query.str().c_str()))
    {
    db.Database->ExecuteQuery("ROLLBACK");
    db.Database->Close();
    return false;
    }
  db.Database->ExecuteQuery("COMMIT");
  db.Database->Close();
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
  mds::DatabaseAPI db;
  std::string newPath = parentPath + "/" + m_Collection->GetName();
  std::stringstream query;
  query << "UPDATE resource_uuid SET path='" << newPath << "' WHERE "
    "uuid='" << m_Collection->GetUuid() << "'";

  db.Open();
  db.Database->ExecuteQuery(query.str().c_str());
  db.Close();

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
