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
    m_Database->GetLog()->Error("Community::Fetch : Community not set\n");
    return false;
    }

  if(m_Community->GetId() == 0)
    {
    m_Database->GetLog()->Error("Community::Fetch : CommunityId not set\n");
    return false;
    }

  if(m_Community->IsFetched())
    {
    return true;
    }

  if(m_Community->GetUuid() == "")
    {
    m_Community->SetUuid(m_Database->GetUuid(
      midasResourceType::COMMUNITY, m_Community->GetId()).c_str());
    }

  std::stringstream query;
  query << "SELECT short_description, introductory_text, copyright_text, name "
    "FROM community WHERE community_id='" << m_Community->GetId() << "'";

  m_Database->Open();
  if(!m_Database->GetDatabase()->ExecuteQuery(query.str().c_str()))
    {
    std::stringstream text;
    text << "Community::Fetch : Query failed: " << query.str() << std::endl;
    m_Database->GetLog()->Error(text.str());
    m_Database->Close();
    return false;
    }

  while(m_Database->GetDatabase()->GetNextRow())
    {
    m_Community->SetDescription(
      m_Database->GetDatabase()->GetValueAsString(0));
    m_Community->SetIntroductoryText(
      m_Database->GetDatabase()->GetValueAsString(1));
    m_Community->SetCopyright(
      m_Database->GetDatabase()->GetValueAsString(2));
    m_Community->SetName(
      m_Database->GetDatabase()->GetValueAsString(3));
    }
  m_Community->SetFetched(true);
  m_Database->Close();
  return true;
}

/** Commit */
bool Community::Commit()
{
  if(!m_Community)
    {
    m_Database->GetLog()->Error("Community::Commit : Community not set\n");
    return false;
    }

  if(m_Community->GetId() == 0)
    {
    m_Database->GetLog()->Error("Community::Commit : CommunityId not set\n");
    return false;
    }

  if(m_Community->GetUuid() == "")
    {
    m_Community->SetUuid(m_Database->GetUuid(
      midasResourceType::COMMUNITY, m_Community->GetId()).c_str());
    }

  std::string path = m_Database->GetRecordByUuid(m_Community->GetUuid()).Path;
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

      m_Database->Open();
      m_Database->GetDatabase()->ExecuteQuery(pathQuery.str().c_str());
      m_Database->Close();

      this->FetchTree();

      for(std::vector<mdo::Community*>::const_iterator i =
          m_Community->GetCommunities().begin();
          i != m_Community->GetCommunities().end(); ++i)
        {
        mds::Community mdsComm;
        mdsComm.SetDatabase(m_Database);
        mdsComm.SetObject(*i);
        mdsComm.ParentPathChanged(newPath);
        }

      for(std::vector<mdo::Collection*>::const_iterator i =
          m_Community->GetCollections().begin();
          i != m_Community->GetCollections().end(); ++i)
        {
        mds::Collection mdsColl;
        mdsColl.SetDatabase(m_Database);
        mdsColl.SetObject(*i);
        mdsColl.ParentPathChanged(newPath);
        }
      }
    else
      {
      m_Database->GetLog()->Error("Community::Commit : could not rename "
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

  m_Database->Open();
  if(m_Database->GetDatabase()->ExecuteQuery(query.str().c_str()))
    {
    m_Database->Close();
    if(m_MarkDirty)
      {
      m_Database->MarkDirtyResource(m_Community->GetUuid(),
        midasDirtyAction::MODIFIED);
      }
    return true;
    }
  std::stringstream text;
  text << "Community::Commit : Query failed: " << query.str() << std::endl;
  m_Database->GetLog()->Error(text.str());
  m_Database->Close();
  return false;
}

bool Community::FetchTree()
{
  if(!m_Community)
    {
    m_Database->GetLog()->Error("Community::FetchTree : Community not set\n");
    return false;
    }

  if(m_Community->GetId() == 0)
    {
    m_Database->GetLog()->Error("Community::FetchTree : CommunityId not set\n");
    return false;
    }

  if(m_Community->GetUuid() == "")
    {
    m_Community->SetUuid(m_Database->GetUuid(
      midasResourceType::COMMUNITY, m_Community->GetId()).c_str());
    }

  m_Community->SetDirty(m_Database->IsResourceDirty(m_Community->GetUuid()));

  std::stringstream query;
  query << "SELECT community.community_id, community.name, resource_uuid.uuid "
    "FROM community, resource_uuid WHERE resource_uuid.resource_type_id='"
    << midasResourceType::COMMUNITY << "' AND resource_uuid.resource_id="
    "community.community_id AND community.community_id IN "
    "(SELECT child_comm_id FROM community2community WHERE parent_comm_id="
    << m_Community->GetId() << ") ORDER BY community.name ASC";
  m_Database->Open();
  m_Database->GetDatabase()->ExecuteQuery(query.str().c_str());

  std::vector<mdo::Community*> childCommunities;
  while(m_Database->GetDatabase()->GetNextRow())
    {
    mdo::Community* community = new mdo::Community;
    community->SetId(m_Database->GetDatabase()->GetValueAsInt(0));
    community->SetName(m_Database->GetDatabase()->GetValueAsString(1));
    community->SetUuid(m_Database->GetDatabase()->GetValueAsString(2));
    childCommunities.push_back(community);
    m_Community->AddCommunity(community);
    }
  m_Database->Close();

  if(m_Recurse)
    {
    for(std::vector<mdo::Community*>::iterator i = childCommunities.begin();
        i != childCommunities.end(); ++i)
      {
      mds::Community mdsComm;
      mdsComm.SetObject(*i);
      mdsComm.SetDatabase(m_Database);
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
    << " ORDER BY collection.name ASC";
  m_Database->Open();
  m_Database->GetDatabase()->ExecuteQuery(query.str().c_str());

  std::vector<mdo::Collection*> collections;
  while(m_Database->GetDatabase()->GetNextRow())
    {
    mdo::Collection* collection = new mdo::Collection;
    collection->SetId(m_Database->GetDatabase()->GetValueAsInt(0));
    collection->SetName(m_Database->GetDatabase()->GetValueAsString(1));
    collection->SetUuid(m_Database->GetDatabase()->GetValueAsString(2));
    collections.push_back(collection);
    m_Community->AddCollection(collection);
    }
  m_Database->Close();

  if(m_Recurse)
    {
    for(std::vector<mdo::Collection*>::iterator i = collections.begin();
        i != collections.end(); ++i)
      {
      mds::Collection mdsColl;
      mdsColl.SetObject(*i);
      mdsColl.SetDatabase(m_Database);
      if(!mdsColl.FetchTree())
        {
        return false;
        }
      }
    }
  return true;
}

bool Community::Delete()
{
  std::vector<int> children;
  std::stringstream query;
  query << "SELECT child_comm_id FROM community2community WHERE "
    "parent_comm_id='" << m_Community->GetId() << "'";
  m_Database->GetDatabase()->Open(m_Database->GetDatabasePath().c_str());
  if(!m_Database->GetDatabase()->ExecuteQuery(query.str().c_str()))
    {
    m_Database->GetDatabase()->Close();
    return false;
    }

  while(m_Database->GetDatabase()->GetNextRow())
    {
    children.push_back(m_Database->GetDatabase()->GetValueAsInt(0));
    }
  m_Database->GetDatabase()->Close();
  bool ok = true;
  for(std::vector<int>::iterator i = children.begin();
      i != children.end(); ++i)
    {
    mds::Community mdsComm;
    mdo::Community* comm = new mdo::Community;
    comm->SetId(*i);
    mdsComm.SetObject(comm);
    mdsComm.SetDatabase(m_Database);
    ok &= mdsComm.Delete();
    delete comm;

    if(!ok)
      {
      return false;
      }
    }

  query.str(std::string());
  query << "SELECT collection_id FROM community2collection WHERE "
    "community_id='" << m_Community->GetId() << "'";
  m_Database->GetDatabase()->Open(m_Database->GetDatabasePath().c_str());
  m_Database->GetDatabase()->ExecuteQuery(query.str().c_str());

  children.clear();
  while(m_Database->GetDatabase()->GetNextRow())
    {
    children.push_back(m_Database->GetDatabase()->GetValueAsInt(0)); 
    }
  m_Database->GetDatabase()->Close();
  for(std::vector<int>::iterator i = children.begin();
      i != children.end(); ++i)
    {
    mds::Collection mdsColl;
    mdo::Collection* coll = new mdo::Collection;
    coll->SetId(*i);
    mdsColl.SetObject(coll);
    mdsColl.SetDatabase(m_Database);
    ok &= mdsColl.Delete();
    delete coll;

    if(!ok)
      {
      return false;
      }
    }

  m_Database->GetDatabase()->Open(m_Database->GetDatabasePath().c_str());
  m_Database->GetDatabase()->ExecuteQuery("BEGIN");
  query.str(std::string());
  query << "DELETE FROM community2community WHERE parent_comm_id='" <<
    m_Community->GetId() << "'";
  if(!m_Database->GetDatabase()->ExecuteQuery(query.str().c_str()))
    {
    m_Database->GetDatabase()->ExecuteQuery("ROLLBACK");
    m_Database->GetDatabase()->Close();
    return false;
    }

  query.str(std::string());
  query << "DELETE FROM community2collection WHERE community_id='" <<
    m_Community->GetId() << "'";
  if(!m_Database->GetDatabase()->ExecuteQuery(query.str().c_str()))
    {
    m_Database->GetDatabase()->ExecuteQuery("ROLLBACK");
    m_Database->GetDatabase()->Close();
    return false;
    }

  query.str(std::string());
  query << "DELETE FROM community WHERE community_id='" <<
    m_Community->GetId() << "'";
  if(!m_Database->GetDatabase()->ExecuteQuery(query.str().c_str()))
    {
    m_Database->GetDatabase()->ExecuteQuery("ROLLBACK");
    m_Database->GetDatabase()->Close();
    return false;
    }
  m_Database->GetDatabase()->ExecuteQuery("COMMIT");
  m_Database->GetDatabase()->Close();
  return true;
}

void Community::SetObject(mdo::Object* object)
{  
  m_Community = reinterpret_cast<mdo::Community*>(object);
}

void Community::ParentPathChanged(std::string parentPath)
{
  std::string newPath = parentPath + "/" + m_Community->GetName();
  std::stringstream query;
  query << "UPDATE resource_uuid SET path='" << newPath << "' WHERE "
    "uuid='" << m_Community->GetUuid() << "'";

  m_Database->Open();
  m_Database->GetDatabase()->ExecuteQuery(query.str().c_str());
  m_Database->Close();

  for(std::vector<mdo::Community*>::const_iterator i =
      m_Community->GetCommunities().begin();
      i != m_Community->GetCommunities().end(); ++i)
    {
    mds::Community mdsComm;
    mdsComm.SetObject(*i);
    mdsComm.SetDatabase(m_Database);
    mdsComm.ParentPathChanged(newPath);
    }

  for(std::vector<mdo::Collection*>::const_iterator i =
      m_Community->GetCollections().begin();
      i != m_Community->GetCollections().end(); ++i)
    {
    mds::Collection mdsColl;
    mdsColl.SetObject(*i);
    mdsColl.SetDatabase(m_Database);
    mdsColl.ParentPathChanged(newPath);
    }
}

} // end namespace
