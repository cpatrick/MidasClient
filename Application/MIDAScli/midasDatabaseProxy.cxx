/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "midasDatabaseProxy.h"

midasDatabaseProxy::midasDatabaseProxy(std::string database)
{
  this->Database = new mds::SQLiteDatabase();
  this->DatabasePath = database;
}

midasDatabaseProxy::~midasDatabaseProxy()
{
  delete this->Database;
}

//-------------------------------------------------------------------------
std::string midasDatabaseProxy::GetResourceLocation(std::string uuid)
{
  std::stringstream query;
  query << "SELECT path FROM resource_uuid WHERE uuid='" << uuid << "'";

  this->Database->ExecuteQuery(query.str().c_str());
  std::string result;

  if(this->Database->GetNextRow())
    {
    result = this->Database->GetValueAsString(0);
    }
  return result;
}

//-------------------------------------------------------------------------
int midasDatabaseProxy::InsertBitstream(std::string path, std::string name)
{
  std::stringstream query;
  query << "INSERT INTO bitstream (location, internal_id, name) VALUES ('1','"
    << path << "', '" << name << "')";
  this->Database->ExecuteQuery(query.str().c_str());
  return this->Database->GetLastInsertId();
}

int midasDatabaseProxy::InsertCollection(std::string name)
{
  std::stringstream query;
  query << "INSERT INTO collection (name) VALUES ('" << name << "')";
  this->Database->ExecuteQuery(query.str().c_str());
  return this->Database->GetLastInsertId();
}

int midasDatabaseProxy::InsertCommunity(std::string name)
{
  std::stringstream query;
  query << "INSERT INTO community (name) VALUES ('" << name << "')";
  this->Database->ExecuteQuery(query.str().c_str());
  return this->Database->GetLastInsertId();
}

int midasDatabaseProxy::InsertItem(std::string name)
{
  std::stringstream query;
  query << "INSERT INTO item (name) VALUES ('" << name << "')";
  this->Database->ExecuteQuery(query.str().c_str());
  return this->Database->GetLastInsertId();
}

//-------------------------------------------------------------------------
void midasDatabaseProxy::InsertResourceRecord(int type, int id,
                                              std::string path,
                                              std::string uuid)
{
  std::stringstream query;
  query << "INSERT INTO resource_uuid (resource_type_id, resource_id, path, "
    "uuid) VALUES ('" << type << "', '" << id << "', '" << path << "', '" 
    << uuid << "')";
  this->Database->ExecuteQuery(query.str().c_str());
}

//-------------------------------------------------------------------------
bool midasDatabaseProxy::ResourceExists(std::string uuid)
{
  std::stringstream query;
  query << "SELECT * FROM resource_uuid WHERE uuid='" << uuid << "'";
  this->Database->ExecuteQuery(query.str().c_str());
  
  return this->Database->GetNextRow();
}

//-------------------------------------------------------------------------
bool midasDatabaseProxy::Open()
{
  return this->Database->Open(this->DatabasePath.c_str());
}

//-------------------------------------------------------------------------
bool midasDatabaseProxy::Close()
{
  return this->Database->Close();
}

//-------------------------------------------------------------------------
void midasDatabaseProxy::Clean()
{
  std::stringstream selectQuery;
  selectQuery << "SELECT path FROM resource_uuid;";
  this->Database->ExecuteQuery(selectQuery.str().c_str());

  while(this->Database->GetNextRow())
    {
    const char* path = this->Database->GetValueAsString(0);

    if(kwsys::SystemTools::FileExists(path))
      {
      if(kwsys::SystemTools::FileIsDirectory(path))
        {
        kwsys::SystemTools::RemoveADirectory(path);
        }
      else
        {
        kwsys::SystemTools::RemoveFile(path);
        }
      }
    }

  this->Database->ExecuteQuery("DELETE FROM resource_uuid");
  this->Database->ExecuteQuery("DELETE FROM bitstream");
  this->Database->ExecuteQuery("DELETE FROM collection");
  this->Database->ExecuteQuery("DELETE FROM community");
  this->Database->ExecuteQuery("DELETE FROM item");
}
