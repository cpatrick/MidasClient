/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mdsCommunity.h"
#include "mdoCommunity.h"
#include "mdsCollection.h"
#include "midasStandardIncludes.h"

namespace mds{

/** Constructor */
Community::Community()
: m_Community(NULL), m_Recurse(true)
{
}
  
/** Destructor */
Community::~Community()
{
}

void Community::SetRecursive(bool recurse)
{
  m_Recurse = recurse;
}
  
/** Fecth */
bool Community::Fetch()
{
  if(!m_Community)
    {
    mds::DatabaseAPI::Instance()->GetLog()->Error("Community::Fetch : Community not set\n");
    return false;
    }

  if(m_Community->GetId() == 0)
    {
    mds::DatabaseAPI::Instance()->GetLog()->Error("Community::Fetch : CommunityId not set\n");
    return false;
    }

  if(m_Community->IsFetched())
    {
    return true;
    }

  if(m_Community->GetUuid() == "")
    {
    m_Community->SetUuid(mds::DatabaseAPI::Instance()->GetUuid(
      midasResourceType::COMMUNITY, m_Community->GetId()).c_str());
    }

  std::stringstream query;
  query << "SELECT short_description, introductory_text, copyright_text, name "
    "FROM community WHERE community_id='" << m_Community->GetId() << "'";

  mds::DatabaseAPI::Instance()->Open();
  if(!mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str()))
    {
    std::stringstream text;
    text << "Community::Fetch : Query failed: " << query.str() << std::endl;
    mds::DatabaseAPI::Instance()->GetLog()->Error(text.str());
    mds::DatabaseAPI::Instance()->Close();
    return false;
    }

  while(mds::DatabaseAPI::Instance()->Database->GetNextRow())
    {
    m_Community->SetDescription(
      mds::DatabaseAPI::Instance()->Database->GetValueAsString(0));
    m_Community->SetIntroductoryText(
      mds::DatabaseAPI::Instance()->Database->GetValueAsString(1));
    m_Community->SetCopyright(
      mds::DatabaseAPI::Instance()->Database->GetValueAsString(2));
    m_Community->SetName(
      mds::DatabaseAPI::Instance()->Database->GetValueAsString(3));
    }
  m_Community->SetFetched(true);
  mds::DatabaseAPI::Instance()->Close();
  return true;
}

bool Community::FetchSize()
{
  if(!m_Community)
    {
    return false;
    }
  double total = 0;

  for(std::vector<mdo::Community*>::const_iterator i = m_Community->GetCommunities().begin();
      i != m_Community->GetCommunities().end(); ++i)
    {
    if((*i)->GetSize() == "")
      {
      mds::Community mdsComm;
      mdsComm.SetObject(*i);
      mdsComm.FetchSize();
      }
    total += midasUtils::StringToDouble((*i)->GetSize());
    }
  for(std::vector<mdo::Collection*>::const_iterator i = m_Community->GetCollections().begin();
      i != m_Community->GetCollections().end(); ++i)
    {
    if((*i)->GetSize() == "")
      {
      mds::Collection mdsColl;
      mdsColl.SetObject(*i);
      mdsColl.FetchSize();
      }
    total += midasUtils::StringToDouble((*i)->GetSize());
    }
  std::stringstream sizeStr;
  sizeStr << total;
  m_Community->SetSize(sizeStr.str());
  return true;
}

/** Commit */
bool Community::Commit()
{
  if(!m_Community)
    {
    mds::DatabaseAPI::Instance()->GetLog()->Error("Community::Commit : Community not set\n");
    return false;
    }

  if(m_Community->GetId() == 0)
    {
    mds::DatabaseAPI::Instance()->GetLog()->Error("Community::Commit : CommunityId not set\n");
    return false;
    }

  if(m_Community->GetUuid() == "")
    {
    m_Community->SetUuid(mds::DatabaseAPI::Instance()->GetUuid(
      midasResourceType::COMMUNITY, m_Community->GetId()).c_str());
    }

  std::string path = mds::DatabaseAPI::Instance()->GetRecordByUuid(m_Community->GetUuid()).Path;
  std::string parentDir = kwsys::SystemTools::GetParentDirectory(path.c_str());
  std::string oldName = kwsys::SystemTools::GetFilenameName(path);

  if(oldName != m_Community->GetName())
    {
    std::string newPath = parentDir + "/" + m_Community->GetName();
    if(rename(path.c_str(), newPath.c_str()) == 0)
      {
      std::stringstream pathQuery;
      pathQuery << "UPDATE resource_uuid SET path='" << newPath <<
        "' WHERE uuid='" << m_Community->GetUuid() << "'";

      mds::DatabaseAPI::Instance()->Open();
      mds::DatabaseAPI::Instance()->Database->ExecuteQuery(pathQuery.str().c_str());
      mds::DatabaseAPI::Instance()->Close();

      this->FetchTree();

      for(std::vector<mdo::Community*>::const_iterator i =
          m_Community->GetCommunities().begin();
          i != m_Community->GetCommunities().end(); ++i)
        {
        mds::Community mdsComm;
        mdsComm.SetObject(*i);
        mdsComm.ParentPathChanged(newPath);
        }

      for(std::vector<mdo::Collection*>::const_iterator i =
          m_Community->GetCollections().begin();
          i != m_Community->GetCollections().end(); ++i)
        {
        mds::Collection mdsColl;
        mdsColl.SetObject(*i);
        mdsColl.ParentPathChanged(newPath);
        }
      }
    else
      {
      mds::DatabaseAPI::Instance()->GetLog()->Error("Community::Commit : could not rename "
        "directory on disk. It may be locked.\n");
      return false;
      }
    }

  std::stringstream query;
  query << "UPDATE community SET " <<
    "name='" <<
    midasUtils::EscapeForSQL(m_Community->GetName()) << "', "
    "short_description='" <<
    midasUtils::EscapeForSQL(m_Community->GetDescription()) << "', "
    "introductory_text='" <<
    midasUtils::EscapeForSQL(m_Community->GetIntroductoryText()) << "', "
    "copyright_text='" <<
    midasUtils::EscapeForSQL(m_Community->GetCopyright()) << "' WHERE "
    "community_id='" << m_Community->GetId() << "'";

  mds::DatabaseAPI::Instance()->Open();
  if(mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str()))
    {
    mds::DatabaseAPI::Instance()->Close();
    if(m_MarkDirty)
      {
      mds::DatabaseAPI::Instance()->MarkDirtyResource(m_Community->GetUuid(),
        midasDirtyAction::MODIFIED);
      }
    return true;
    }
  std::stringstream text;
  text << "Community::Commit : Query failed: " << query.str() << std::endl;
  mds::DatabaseAPI::Instance()->GetLog()->Error(text.str());
  mds::DatabaseAPI::Instance()->Close();
  return false;
}

bool Community::FetchTree()
{
  if(!m_Community)
    {
    mds::DatabaseAPI::Instance()->GetLog()->Error("Community::FetchTree : Community not set\n");
    return false;
    }

  if(m_Community->GetId() == 0)
    {
    mds::DatabaseAPI::Instance()->GetLog()->Error("Community::FetchTree : CommunityId not set\n");
    return false;
    }

  if(m_Community->GetUuid() == "")
    {
    m_Community->SetUuid(mds::DatabaseAPI::Instance()->GetUuid(
      midasResourceType::COMMUNITY, m_Community->GetId()).c_str());
    }

  m_Community->SetDirty(mds::DatabaseAPI::Instance()->IsResourceDirty(m_Community->GetUuid()));

  std::stringstream query;
  query << "SELECT community.community_id, community.name, resource_uuid.uuid "
    "FROM community, resource_uuid WHERE resource_uuid.resource_type_id='"
    << midasResourceType::COMMUNITY << "' AND resource_uuid.resource_id="
    "community.community_id AND community.community_id IN "
    "(SELECT child_comm_id FROM community2community WHERE parent_comm_id="
    << m_Community->GetId() << ") ORDER BY community.name COLLATE NOCASE ASC";
  mds::DatabaseAPI::Instance()->Open();
  mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str());

  std::vector<mdo::Community*> childCommunities;
  while(mds::DatabaseAPI::Instance()->Database->GetNextRow())
    {
    mdo::Community* community = new mdo::Community;
    community->SetId(mds::DatabaseAPI::Instance()->Database->GetValueAsInt(0));
    community->SetName(mds::DatabaseAPI::Instance()->Database->GetValueAsString(1));
    community->SetUuid(mds::DatabaseAPI::Instance()->Database->GetValueAsString(2));
    childCommunities.push_back(community);
    m_Community->AddCommunity(community);
    }
  mds::DatabaseAPI::Instance()->Close();

  if(m_Recurse)
    {
    for(std::vector<mdo::Community*>::iterator i = childCommunities.begin();
        i != childCommunities.end(); ++i)
      {
      mds::Community mdsComm;
      mdsComm.SetObject(*i);
      if(!mdsComm.FetchTree())
        {
        return false;
        }
      }
    }

  query.str(std::string());
  query << "SELECT collection.collection_id, collection.name, resource_uuid.uuid "
    "FROM collection, resource_uuid WHERE resource_uuid.resource_type_id='"
    << midasResourceType::COLLECTION << "' AND resource_uuid.resource_id="
    "collection.collection_id AND collection.collection_id IN (SELECT collection_id "
    "FROM community2collection WHERE community_id=" << m_Community->GetId() << ")"
    << " ORDER BY collection.name COLLATE NOCASE ASC";
  mds::DatabaseAPI::Instance()->Open();
  mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str());

  std::vector<mdo::Collection*> collections;
  while(mds::DatabaseAPI::Instance()->Database->GetNextRow())
    {
    mdo::Collection* collection = new mdo::Collection;
    collection->SetId(mds::DatabaseAPI::Instance()->Database->GetValueAsInt(0));
    collection->SetName(mds::DatabaseAPI::Instance()->Database->GetValueAsString(1));
    collection->SetUuid(mds::DatabaseAPI::Instance()->Database->GetValueAsString(2));
    collections.push_back(collection);
    m_Community->AddCollection(collection);
    }
  mds::DatabaseAPI::Instance()->Close();

  if(m_Recurse)
    {
    for(std::vector<mdo::Collection*>::iterator i = collections.begin();
        i != collections.end(); ++i)
      {
      mds::Collection mdsColl;
      mdsColl.SetObject(*i);
      if(!mdsColl.FetchTree())
        {
        return false;
        }
      }
    }
  return true;
}

bool Community::Delete(bool deleteOnDisk)
{
  std::vector<int> children;
  std::stringstream query;
  query << "SELECT child_comm_id FROM community2community WHERE "
    "parent_comm_id='" << m_Community->GetId() << "'";
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
    mds::Community mdsComm;
    mdo::Community* comm = new mdo::Community;
    comm->SetId(*i);
    comm->SetUuid(mds::DatabaseAPI::Instance()->GetUuid(midasResourceType::COMMUNITY, *i).c_str());
    mdsComm.SetObject(comm);
    mdsComm.SetPath(mds::DatabaseAPI::Instance()->GetRecordByUuid(comm->GetUuid()).Path);
    ok &= mdsComm.Delete(deleteOnDisk);
    delete comm;

    if(!ok)
      {
      return false;
      }
    }

  query.str(std::string());
  query << "SELECT collection_id FROM community2collection WHERE "
    "community_id='" << m_Community->GetId() << "'";
  mds::DatabaseAPI::Instance()->Database->Open(mds::DatabaseAPI::Instance()->GetDatabasePath().c_str());
  mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str());

  children.clear();
  while(mds::DatabaseAPI::Instance()->Database->GetNextRow())
    {
    children.push_back(mds::DatabaseAPI::Instance()->Database->GetValueAsInt(0)); 
    }
  mds::DatabaseAPI::Instance()->Database->Close();
  for(std::vector<int>::iterator i = children.begin();
      i != children.end(); ++i)
    {
    mds::Collection mdsColl;
    mdo::Collection* coll = new mdo::Collection;
    coll->SetId(*i);
    coll->SetUuid(mds::DatabaseAPI::Instance()->GetUuid(midasResourceType::COLLECTION, *i).c_str());
    mdsColl.SetObject(coll);
    mdsColl.SetPath(mds::DatabaseAPI::Instance()->GetRecordByUuid(coll->GetUuid()).Path);
    ok &= mdsColl.Delete(deleteOnDisk);
    delete coll;

    if(!ok)
      {
      return false;
      }
    }

  mds::DatabaseAPI::Instance()->Database->Open(mds::DatabaseAPI::Instance()->GetDatabasePath().c_str());
  mds::DatabaseAPI::Instance()->Database->ExecuteQuery("BEGIN");
  query.str(std::string());
  query << "DELETE FROM community2community WHERE parent_comm_id='" <<
    m_Community->GetId() << "' OR child_comm_id='" <<
    m_Community->GetId() << "'";
  if(!mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str()))
    {
    mds::DatabaseAPI::Instance()->Database->ExecuteQuery("ROLLBACK");
    mds::DatabaseAPI::Instance()->Database->Close();
    return false;
    }

  query.str(std::string());
  query << "DELETE FROM community2collection WHERE community_id='" <<
    m_Community->GetId() << "'";
  if(!mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str()))
    {
    mds::DatabaseAPI::Instance()->Database->ExecuteQuery("ROLLBACK");
    mds::DatabaseAPI::Instance()->Database->Close();
    return false;
    }

  query.str(std::string());
  query << "DELETE FROM community WHERE community_id='" <<
    m_Community->GetId() << "'";
  if(!mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str()))
    {
    mds::DatabaseAPI::Instance()->Database->ExecuteQuery("ROLLBACK");
    mds::DatabaseAPI::Instance()->Database->Close();
    return false;
    }

  query.str(std::string());
  query << "DELETE FROM dirty_resource WHERE uuid='" <<
    m_Community->GetUuid() << "'";
  if(!mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str()))
    {
    mds::DatabaseAPI::Instance()->Database->ExecuteQuery("ROLLBACK");
    mds::DatabaseAPI::Instance()->Database->Close();
    return false;
    }

  query.str(std::string());
  query << "DELETE FROM resource_uuid WHERE uuid='" <<
    m_Community->GetUuid() << "'";
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

void Community::SetObject(mdo::Object* object)
{  
  m_Community = reinterpret_cast<mdo::Community*>(object);
}

void Community::SetPath(std::string path)
{
  m_Path = path;
}

void Community::ParentPathChanged(std::string parentPath)
{
  std::string newPath = parentPath + "/" + m_Community->GetName();
  std::stringstream query;
  query << "UPDATE resource_uuid SET path='" << newPath << "' WHERE "
    "uuid='" << m_Community->GetUuid() << "'";

  mds::DatabaseAPI::Instance()->Open();
  mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str());
  mds::DatabaseAPI::Instance()->Close();

  for(std::vector<mdo::Community*>::const_iterator i =
      m_Community->GetCommunities().begin();
      i != m_Community->GetCommunities().end(); ++i)
    {
    mds::Community mdsComm;
    mdsComm.SetObject(*i);
    mdsComm.ParentPathChanged(newPath);
    }

  for(std::vector<mdo::Collection*>::const_iterator i =
      m_Community->GetCollections().begin();
      i != m_Community->GetCollections().end(); ++i)
    {
    mds::Collection mdsColl;
    mdsColl.SetObject(*i);
    mdsColl.ParentPathChanged(newPath);
    }
}

} // end namespace
