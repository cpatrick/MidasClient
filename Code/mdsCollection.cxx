/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mdsCollection.h"
#include "mdoCollection.h"
#include "midasStandardIncludes.h"

namespace mds{

/** Constructor */
Collection::Collection()
{
  m_Collection = NULL;
}
  
/** Destructor */
Collection::~Collection()
{
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

  if(m_Collection->GetUuid() == "")
    {
    m_Collection->SetUuid(m_Database->GetUuid(
      midasResourceType::COLLECTION, m_Collection->GetId()).c_str());
    }
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

} // end namespace
