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
    m_Database->GetLog()->Error("Collection::Fetch : Collection not set\n");
    return false;
    }

  if(m_Collection->GetId() == 0)
    {
    m_Database->GetLog()->Error("Collection::Fetch : CollectionId not set\n");
    return false;
    }

  if(m_Collection->IsFetched())
    {
    return true;
    }

  if(m_Collection->GetUuid() == "")
    {
    m_Collection->SetUuid(m_Database->GetUuid(
      midasResourceType::COLLECTION, m_Collection->GetId()).c_str());
    }

  std::stringstream query;
  query << "SELECT short_description, introductory_text, copyright_text, name "
    "FROM collection WHERE collection_id='" << m_Collection->GetId() << "'";

  m_Database->Open();
  if(!m_Database->GetDatabase()->ExecuteQuery(query.str().c_str()))
    {
    std::stringstream text;
    text << "Collection::Fetch : Query failed: " << query.str() << std::endl;
    m_Database->GetLog()->Error(text.str());
    m_Database->Close();
    return false;
    }

  while(m_Database->GetDatabase()->GetNextRow())
    {
    m_Collection->SetDescription(
      m_Database->GetDatabase()->GetValueAsString(0));
    m_Collection->SetIntroductoryText(
      m_Database->GetDatabase()->GetValueAsString(1));
    m_Collection->SetCopyright(
      m_Database->GetDatabase()->GetValueAsString(2));
    m_Collection->SetName(
      m_Database->GetDatabase()->GetValueAsString(3));
    }
  m_Collection->SetFetched(true);
  m_Database->Close();
  return true;
}

/** Commit */
bool Collection::Commit()
{
  if(!m_Collection)
    {
    m_Database->GetLog()->Error("Collection::Commit : Collection not set\n");
    return false;
    }

  if(m_Collection->GetId() == 0)
    {
    m_Database->GetLog()->Error("Collection::Commit : Collection not set\n");
    return false;
    }

  if(m_Collection->GetUuid() == "")
    {
    m_Collection->SetUuid(m_Database->GetUuid(
      midasResourceType::COLLECTION, m_Collection->GetId()).c_str());
    }

  std::string path = m_Database->GetRecordByUuid(m_Collection->GetUuid()).Path;
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

      m_Database->Open();
      m_Database->GetDatabase()->ExecuteQuery(pathQuery.str().c_str());
      m_Database->Close();

      this->FetchTree();

      for(std::vector<mdo::Item*>::const_iterator i =
          m_Collection->GetItems().begin();
          i != m_Collection->GetItems().end(); ++i)
        {
        mds::Item mdsItem;
        mdsItem.SetDatabase(m_Database);
        mdsItem.SetObject(*i);
        mdsItem.ParentPathChanged(newPath);
        }
      }
    else
      {
      m_Database->GetLog()->Error("Collection::Commit : could not rename "
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

  m_Database->Open();
  if(m_Database->GetDatabase()->ExecuteQuery(query.str().c_str()))
    {
    m_Database->Close();
    if(m_MarkDirty)
      {
      m_Database->MarkDirtyResource(m_Collection->GetUuid(), midasDirtyAction::MODIFIED);
      }
    return true;
    }
  std::stringstream text;
  text << "Collection::Commit : Query failed: " << query.str() << std::endl;
  m_Database->GetLog()->Error(text.str());
  m_Database->Close();
  return false;
}

bool Collection::FetchTree()
{
  if(!m_Collection)
    {
    m_Database->GetLog()->Error("Collection::FetchTree : Collection not set\n");
    return false;
    }

  if(m_Collection->GetId() == 0)
    {
    m_Database->GetLog()->Error("Collection::FetchTree : Collection not set\n");
    return false;
    }

  if(m_Collection->GetUuid() == "")
    {
    m_Collection->SetUuid(m_Database->GetUuid(
      midasResourceType::COLLECTION, m_Collection->GetId()).c_str());
    }
  
  m_Collection->SetDirty(m_Database->IsResourceDirty(m_Collection->GetUuid()));

  std::stringstream query;
  query << "SELECT item.item_id, item.title, resource_uuid.uuid FROM item, resource_uuid "
    "WHERE resource_uuid.resource_type_id='" << midasResourceType::ITEM << "' AND "
    "resource_uuid.resource_id=item.item_id AND item.item_id IN (SELECT item_id FROM "
    "collection2item WHERE collection_id=" << m_Collection->GetId() << ")";
  m_Database->Open();
  m_Database->GetDatabase()->ExecuteQuery(query.str().c_str());

  std::vector<mdo::Item*> items;
  while(m_Database->GetDatabase()->GetNextRow())
    {
    mdo::Item* item = new mdo::Item;
    item->SetId(m_Database->GetDatabase()->GetValueAsInt(0));
    item->SetTitle(m_Database->GetDatabase()->GetValueAsString(1));
    item->SetUuid(m_Database->GetDatabase()->GetValueAsString(2));
    m_Collection->AddItem(item);
    items.push_back(item);
    }

  if(m_Recurse)
    {
    for(std::vector<mdo::Item*>::iterator i = items.begin();
        i != items.end(); ++i)
      {
      mds::Item mdsItem;
      mdsItem.SetObject(*i);
      mdsItem.SetDatabase(m_Database);
      if(!mdsItem.FetchTree())
        {
        return false;
        }
      }
    }
  return true;
}

bool Collection::Delete()
{
  return true;
}

void Collection::SetObject(mdo::Object* object)
{  
  m_Collection = reinterpret_cast<mdo::Collection*>(object);
}

void Collection::ParentPathChanged(std::string parentPath)
{
  std::string newPath = parentPath + "/" + m_Collection->GetName();
  std::stringstream query;
  query << "UPDATE resource_uuid SET path='" << newPath << "' WHERE "
    "uuid='" << m_Collection->GetUuid() << "'";

  m_Database->Open();
  m_Database->GetDatabase()->ExecuteQuery(query.str().c_str());
  m_Database->Close();

  for(std::vector<mdo::Item*>::const_iterator i =
      m_Collection->GetItems().begin();
      i != m_Collection->GetItems().end(); ++i)
    {
    mds::Item mdsItem;
    mdsItem.SetObject(*i);
    mdsItem.SetDatabase(m_Database);
    mdsItem.ParentPathChanged(newPath);
    }
}

} // end namespace
