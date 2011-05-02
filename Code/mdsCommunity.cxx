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
  mds::DatabaseAPI db;
  if(!m_Community)
    {
    db.GetLog()->Error("Community::Fetch : Community not set\n");
    return false;
    }

  if(m_Community->GetId() == 0)
    {
    db.GetLog()->Error("Community::Fetch : CommunityId not set\n");
    return false;
    }

  if(m_Community->IsFetched())
    {
    return true;
    }

  if(m_Community->GetUuid() == "")
    {
    m_Community->SetUuid(db.GetUuid(
      midasResourceType::COMMUNITY, m_Community->GetId()).c_str());
    }

  std::stringstream query;
  query << "SELECT short_description, introductory_text, copyright_text, name "
    "FROM community WHERE community_id='" << m_Community->GetId() << "'";

  db.Open();
  if(!db.Database->ExecuteQuery(query.str().c_str()))
    {
    std::stringstream text;
    text << "Community::Fetch : Query failed: " << query.str() << std::endl;
    db.GetLog()->Error(text.str());
    db.Close();
    return false;
    }

  while(db.Database->GetNextRow())
    {
    m_Community->SetDescription(
      db.Database->GetValueAsString(0));
    m_Community->SetIntroductoryText(
      db.Database->GetValueAsString(1));
    m_Community->SetCopyright(
      db.Database->GetValueAsString(2));
    m_Community->SetName(
      db.Database->GetValueAsString(3));
    }
  m_Community->SetFetched(true);
  db.Close();
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
  mds::DatabaseAPI db;
  if(!m_Community)
    {
    db.GetLog()->Error("Community::Commit : Community not set\n");
    return false;
    }

  if(m_Community->GetId() == 0)
    {
    db.GetLog()->Error("Community::Commit : CommunityId not set\n");
    return false;
    }

  if(m_Community->GetUuid() == "")
    {
    m_Community->SetUuid(db.GetUuid(
      midasResourceType::COMMUNITY, m_Community->GetId()).c_str());
    }

  std::string path = db.GetRecordByUuid(m_Community->GetUuid()).Path;
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

      db.Open();
      db.Database->ExecuteQuery(pathQuery.str().c_str());
      db.Close();

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
      db.GetLog()->Error("Community::Commit : could not rename "
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

  db.Open();
  if(db.Database->ExecuteQuery(query.str().c_str()))
    {
    db.Close();
    if(m_MarkDirty)
      {
      db.MarkDirtyResource(m_Community->GetUuid(),
        midasDirtyAction::MODIFIED);
      }
    return true;
    }
  std::stringstream text;
  text << "Community::Commit : Query failed: " << query.str() << std::endl;
  db.GetLog()->Error(text.str());
  db.Close();
  return false;
}

bool Community::FetchTree()
{
  mds::DatabaseAPI db;
  if(!m_Community)
    {
    db.GetLog()->Error("Community::FetchTree : Community not set\n");
    return false;
    }

  if(m_Community->GetId() == 0)
    {
    db.GetLog()->Error("Community::FetchTree : CommunityId not set\n");
    return false;
    }

  if(m_Community->GetUuid() == "")
    {
    m_Community->SetUuid(db.GetUuid(
      midasResourceType::COMMUNITY, m_Community->GetId()).c_str());
    }

  m_Community->SetDirty(db.IsResourceDirty(m_Community->GetUuid()));

  std::stringstream query;
  query << "SELECT community.community_id, community.name, resource_uuid.uuid "
    "FROM community, resource_uuid WHERE resource_uuid.resource_type_id='"
    << midasResourceType::COMMUNITY << "' AND resource_uuid.resource_id="
    "community.community_id AND community.community_id IN "
    "(SELECT child_comm_id FROM community2community WHERE parent_comm_id="
    << m_Community->GetId() << ") ORDER BY community.name COLLATE NOCASE ASC";
  db.Open();
  db.Database->ExecuteQuery(query.str().c_str());

  std::vector<mdo::Community*> childCommunities;
  while(db.Database->GetNextRow())
    {
    mdo::Community* community = new mdo::Community;
    community->SetId(db.Database->GetValueAsInt(0));
    community->SetName(db.Database->GetValueAsString(1));
    community->SetUuid(db.Database->GetValueAsString(2));
    childCommunities.push_back(community);
    m_Community->AddCommunity(community);
    }
  db.Close();
  for(std::vector<mdo::Community*>::const_iterator i =
      m_Community->GetCommunities().begin();
      i != m_Community->GetCommunities().end(); ++i)
    {
    (*i)->SetDirty(db.IsResourceDirty((*i)->GetUuid()));
    }

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
  db.Open();
  db.Database->ExecuteQuery(query.str().c_str());

  std::vector<mdo::Collection*> collections;
  while(db.Database->GetNextRow())
    {
    mdo::Collection* collection = new mdo::Collection;
    collection->SetId(db.Database->GetValueAsInt(0));
    collection->SetName(db.Database->GetValueAsString(1));
    collection->SetUuid(db.Database->GetValueAsString(2));
    collections.push_back(collection);
    m_Community->AddCollection(collection);
    }
  db.Close();
  for(std::vector<mdo::Collection*>::const_iterator i =
      m_Community->GetCollections().begin();
      i != m_Community->GetCollections().end(); ++i)
    {
    (*i)->SetDirty(db.IsResourceDirty((*i)->GetUuid()));
    }

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
  mds::DatabaseAPI db;
  std::vector<int> children;
  std::stringstream query;
  query << "SELECT child_comm_id FROM community2community WHERE "
    "parent_comm_id='" << m_Community->GetId() << "'";
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
    mds::Community mdsComm;
    mdo::Community* comm = new mdo::Community;
    comm->SetId(*i);
    comm->SetUuid(db.GetUuid(midasResourceType::COMMUNITY, *i).c_str());
    mdsComm.SetObject(comm);
    mdsComm.SetPath(db.GetRecordByUuid(comm->GetUuid()).Path);
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
  db.Open();
  db.Database->ExecuteQuery(query.str().c_str());

  children.clear();
  while(db.Database->GetNextRow())
    {
    children.push_back(db.Database->GetValueAsInt(0)); 
    }
  db.Database->Close();
  for(std::vector<int>::iterator i = children.begin();
      i != children.end(); ++i)
    {
    mds::Collection mdsColl;
    mdo::Collection* coll = new mdo::Collection;
    coll->SetId(*i);
    coll->SetUuid(db.GetUuid(midasResourceType::COLLECTION, *i).c_str());
    mdsColl.SetObject(coll);
    mdsColl.SetPath(db.GetRecordByUuid(coll->GetUuid()).Path);
    ok &= mdsColl.Delete(deleteOnDisk);
    delete coll;

    if(!ok)
      {
      return false;
      }
    }

  db.Open();
  db.Database->ExecuteQuery("BEGIN");
  query.str(std::string());
  query << "DELETE FROM community2community WHERE parent_comm_id='" <<
    m_Community->GetId() << "' OR child_comm_id='" <<
    m_Community->GetId() << "'";
  if(!db.Database->ExecuteQuery(query.str().c_str()))
    {
    db.Database->ExecuteQuery("ROLLBACK");
    db.Database->Close();
    return false;
    }

  query.str(std::string());
  query << "DELETE FROM community2collection WHERE community_id='" <<
    m_Community->GetId() << "'";
  if(!db.Database->ExecuteQuery(query.str().c_str()))
    {
    db.Database->ExecuteQuery("ROLLBACK");
    db.Database->Close();
    return false;
    }

  query.str(std::string());
  query << "DELETE FROM community WHERE community_id='" <<
    m_Community->GetId() << "'";
  if(!db.Database->ExecuteQuery(query.str().c_str()))
    {
    db.Database->ExecuteQuery("ROLLBACK");
    db.Database->Close();
    return false;
    }

  query.str(std::string());
  query << "DELETE FROM dirty_resource WHERE uuid='" <<
    m_Community->GetUuid() << "'";
  if(!db.Database->ExecuteQuery(query.str().c_str()))
    {
    db.Database->ExecuteQuery("ROLLBACK");
    db.Database->Close();
    return false;
    }

  query.str(std::string());
  query << "DELETE FROM resource_uuid WHERE uuid='" <<
    m_Community->GetUuid() << "'";
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
  mds::DatabaseAPI db;
  std::string newPath = parentPath + "/" + m_Community->GetName();
  std::stringstream query;
  query << "UPDATE resource_uuid SET path='" << newPath << "' WHERE "
    "uuid='" << m_Community->GetUuid() << "'";

  db.Open();
  db.Database->ExecuteQuery(query.str().c_str());
  db.Close();

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
