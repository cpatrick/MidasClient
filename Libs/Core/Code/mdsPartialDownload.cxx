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
#include "mdsPartialDownload.h"

#include "mdsDatabaseAPI.h"

namespace mds
{

PartialDownload::PartialDownload()
{
  this->ParentItem = 0;
}

PartialDownload::~PartialDownload()
{
}

bool PartialDownload::Commit()
{
  if( this->Path == "" || this->ParentItem <= 0 || this->Uuid == "" )
    {
    return false;
    }
  mds::DatabaseAPI db;
  db.Open();
  db.Database->ExecuteQuery("BEGIN");

  std::stringstream query;
  query << "DELETE FROM partial_download WHERE path='" << this->Path << "'";

  if( !db.Database->ExecuteQuery(query.str().c_str() ) )
    {
    db.Database->ExecuteQuery("ROLLBACK");
    db.Close();
    return false;
    }

  query.str(std::string() );

  query << "INSERT INTO partial_download (path, uuid, item_id) VALUES ('"
        << this->Path << "', '"
        << this->Uuid << "', '"
        << this->ParentItem << "')";

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

bool PartialDownload::Remove()
{
  if( this->Path == "" )
    {
    return false;
    }
  mds::DatabaseAPI db;
  db.Open();

  std::stringstream query;
  query << "DELETE FROM partial_download WHERE path='" << this->Path << "'";

  if( !db.Database->ExecuteQuery(query.str().c_str() ) )
    {
    db.Close();
    return false;
    }
  db.Close();
  return true;
}

bool PartialDownload::FetchAll(std::vector<mds::PartialDownload *>& list)
{
  mds::DatabaseAPI db;

  db.Open();

  if( !db.Database->ExecuteQuery("SELECT path, uuid, item_id "
                                 "FROM partial_download") )
    {
    db.Close();
    return false;
    }
  while( db.Database->GetNextRow() )
    {
    mds::PartialDownload* dl = new mds::PartialDownload;
    dl->SetPath(db.Database->GetValueAsString(0) );
    dl->SetUuid(db.Database->GetValueAsString(1) );
    dl->SetParentItem(db.Database->GetValueAsInt(2) );
    list.push_back(dl);
    }

  db.Close();
  return true;
}

bool PartialDownload::RemoveAll()
{
  mds::DatabaseAPI db;

  db.Open();

  if( !db.Database->ExecuteQuery("DELETE FROM partial_download") )
    {
    db.Close();
    return false;
    }

  db.Close();
  return true;
}

void PartialDownload::SetId(int id)
{
  this->Id = id;
}

int PartialDownload::GetId()
{
  return this->Id;
}

void PartialDownload::SetOffset(int64 offset)
{
  this->Offset = offset;
}

int64 PartialDownload::GetOffset()
{
  return this->Offset;
}

void PartialDownload::SetPath(const std::string& path)
{
  this->Path = path;
}

std::string PartialDownload::GetPath()
{
  return this->Path;
}

void PartialDownload::SetParentItem(int parentItem)
{
  this->ParentItem = parentItem;
}

int PartialDownload::GetParentItem()
{
  return this->ParentItem;
}

void PartialDownload::SetUuid(const std::string& uuid)
{
  this->Uuid = uuid;
}

std::string PartialDownload::GetUuid()
{
  return this->Uuid;
}

}
