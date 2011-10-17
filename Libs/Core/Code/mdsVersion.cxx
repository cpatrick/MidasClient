/******************************************************************************
 * Copyright 2011 Kitware Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/
#include "mdsVersion.h"
#include "mdsDatabaseAPI.h"

namespace mds
{

Version::Version()
{
}

Version::~Version()
{
}

void Version::SetObject(mdo::Version object)
{
  m_Version = object;
}

bool Version::Commit()
{
  if( m_Version.Name == "" )
    {
    return false;
    }
  mds::DatabaseAPI db;
  db.Open();
  db.Database->ExecuteQuery("BEGIN");

  std::stringstream query;
  query << "DELETE FROM version WHERE name='" << m_Version.Name << "'";

  if( !db.Database->ExecuteQuery(query.str().c_str() ) )
    {
    db.Database->ExecuteQuery("ROLLBACK");
    db.Close();
    return false;
    }

  query.str(std::string() );

  query << "INSERT INTO version (name, major, minor, patch) VALUES ('"
        << m_Version.Name << "', '" << m_Version.Major << "', '"
        << m_Version.Minor << "', '" << m_Version.Patch << "')";

  if( !db.Database->ExecuteQuery(query.str().c_str() ) )
    {
    db.Database->ExecuteQuery("ROLLBACK");
    db.Close();
    return false;
    }
  db.Database->ExecuteQuery("COMMIT");
  db.Close();
  return true;
}

bool Version::Fetch()
{
  if( m_Version.Name == "" )
    {
    return false;
    }

  mds::DatabaseAPI db;
  db.Open();

  std::stringstream query;
  query << "SELECT major, minor, patch FROM version WHERE name='"
        << m_Version.Name << "'";

  if( !db.Database->ExecuteQuery(query.str().c_str() ) )
    {
    db.Close();
    return false;
    }

  if( db.Database->GetNextRow() )
    {
    m_Version.Major = db.Database->GetValueAsInt(0);
    m_Version.Minor = db.Database->GetValueAsInt(1);
    m_Version.Patch = db.Database->GetValueAsInt(2);
    db.Close();
    return true;
    }
  else
    {
    return false;
    }
}

}
