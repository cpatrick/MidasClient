/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mdsCommunity.h"
#include "mdoCommunity.h"
#include "midasStandardIncludes.h"

namespace mds{

/** Constructor */
Community::Community()
{
  m_Community = NULL;
}
  
/** Destructor */
Community::~Community()
{
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

  if(m_Community->GetUuid() == "")
    {
    m_Community->SetUuid(m_Database->GetUuid(
      midasResourceType::COMMUNITY, m_Community->GetId()).c_str());
    }
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
  return true;
}

bool Community::Delete()
{
  return true;
}

void Community::SetObject(mdo::Object* object)
{  
  m_Community = reinterpret_cast<mdo::Community*>(object);
}

} // end namespace
