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
int midasDatabaseProxy::AddResource(int type, std::string uuid,
  std::string path, std::string name, int parentType, int parentId)
{
  if(!this->ResourceExists(uuid))
    {
    int id = -1;
    switch(type)
      {
      case MIDAS_RESOURCE_BITSTREAM:
        id = this->InsertBitstream(path, name);
        break;
      case MIDAS_RESOURCE_COLLECTION:
        id = this->InsertCollection(name);
        break;
      case MIDAS_RESOURCE_COMMUNITY:
        id = this->InsertCommunity(name);
        break;
      case MIDAS_RESOURCE_ITEM:
        id = this->InsertItem(name);
        break;
      default:
        break;
      }
    if(id > 0)
      {
      this->InsertResourceRecord(type, id, path, uuid);
      if(parentId > 0)
        {
        this->AddChild(parentType, parentId, type, id);
        }
      }
    return id;
    }
  else
    {
    return this->GetIdForUuid(uuid);
    }
}

//-------------------------------------------------------------------------
int midasDatabaseProxy::GetIdForUuid(std::string uuid)
{
  std::stringstream query;
  query << "SELECT resource_id FROM resource_uuid WHERE uuid='"
    << uuid << "'";
  this->Database->ExecuteQuery(query.str().c_str());

  return this->Database->GetNextRow() ? this->Database->GetValueAsInt(0) : -1;
}

//-------------------------------------------------------------------------
bool midasDatabaseProxy::AddChild(int parentType, int parentId,
                                  int childType, int childId)
{
  std::stringstream query;
  query << "INSERT INTO ";
  std::string parent, child, parentCol, childCol;

  switch(parentType)
    {
    case MIDAS_RESOURCE_COLLECTION:
      parent = "collection";
      break;
    case MIDAS_RESOURCE_COMMUNITY:
      parent = "community";
      break;
    case MIDAS_RESOURCE_ITEM:
      parent = "item";
      break;
    default:
      return false;
    }

  switch(childType)
    {
    case MIDAS_RESOURCE_BITSTREAM:
      child = "bitstream";
      break;
    case MIDAS_RESOURCE_COLLECTION:
      child = "collection";
      break;
    case MIDAS_RESOURCE_COMMUNITY:
      child = "community";
      break;
    case MIDAS_RESOURCE_ITEM:
      child = "item";
      break;
    default:
      return false;
    }

  //special case for community2community
  if(parent == "community" && child == "community")
    {
    parentCol = "parent_comm";
    childCol = "child_comm";
    }
  else
    {
    parentCol = parent;
    childCol = child;
    }
  query << parent << "2" << child << " (" << parentCol << "_id, " << childCol
    << "_id) VALUES ('" << parentId << "', '" << childId << "')";
  return this->Database->ExecuteQuery(query.str().c_str());
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

//-------------------------------------------------------------------------
int midasDatabaseProxy::InsertCollection(std::string name)
{
  std::stringstream query;
  query << "INSERT INTO collection (name) VALUES ('" << name << "')";
  this->Database->ExecuteQuery(query.str().c_str());
  return this->Database->GetLastInsertId();
}

//-------------------------------------------------------------------------
int midasDatabaseProxy::InsertCommunity(std::string name)
{
  std::stringstream query;
  query << "INSERT INTO community (name) VALUES ('" << name << "')";
  this->Database->ExecuteQuery(query.str().c_str());
  return this->Database->GetLastInsertId();
}

//-------------------------------------------------------------------------
int midasDatabaseProxy::InsertItem(std::string name)
{
  std::stringstream query;
  query << "INSERT INTO item (title) VALUES ('" << name << "')";
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
  this->Database->ExecuteQuery("DELETE FROM community2community");
  this->Database->ExecuteQuery("DELETE FROM community2collection");
  this->Database->ExecuteQuery("DELETE FROM collection2item");
  this->Database->ExecuteQuery("DELETE FROM item2bitstream");
}