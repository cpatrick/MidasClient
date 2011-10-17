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
#include "mdsUpgrade.h"
#include "mdsDatabaseAPI.h"
#include "mdsVersion.h"
#include "mdoVersion.h"

namespace mds
{

bool Upgrade::UpgradeDatabase(const std::string& path, mdo::Version dbVersion)
{
  mds::DatabaseInfo::Instance()->SetPath(path);

  bool status = true;

  mdo::Version v1_8_0(1, 8, 0);
  mdo::Version v1_8_2(1, 8, 2);

  if( dbVersion <= v1_8_0 )
    {
    status &= Upgrade::Upgrade1_8_0();
    }
  if( dbVersion <= v1_8_2 )
    {
    status &= Upgrade::Upgrade1_8_2();
    }
  // Add more version upgrades here when needed

  return status;
}

bool Upgrade::Upgrade1_8_0()
{
  bool status = true;

  // Empty set of changes (first version where we have this upgrade mechanism)
  mds::Version version;

  version.SetObject(mdo::Version(1, 8, 0) );
  status &= version.Commit();

  return status;
}

bool Upgrade::Upgrade1_8_2()
{
  bool status = true;

  mds::DatabaseAPI db;

  status &= db.Open();
  std::stringstream query;
  status &= db.Database->ExecuteQuery("CREATE TABLE IF NOT EXISTS "
                                      "partial_download (id integer PRIMARY KEY AUTOINCREMENT, "
                                      "uuid character varying(60), "
                                      "path character varying(512), "
                                      "item_id integer)");

  status &= db.Database->ExecuteQuery("CREATE TABLE IF NOT EXISTS "
                                      "partial_upload (id integer PRIMARY KEY AUTOINCREMENT, "
                                      "bitstream_id integer, "
                                      "uploadtoken character varying(512), "
                                      "user_id integer, "
                                      "item_id integer)");

  status &= db.Close();

  if( status )
    {
    mds::Version version;
    version.SetObject(mdo::Version(1, 8, 2) );
    status &= version.Commit();
    }
  return status;
}

}
