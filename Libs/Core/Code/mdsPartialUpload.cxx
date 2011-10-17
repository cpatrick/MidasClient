#include "mdsPartialUpload.h"

#include "mdsDatabaseAPI.h"

namespace mds
{

PartialUpload::PartialUpload()
{
  this->Id = 0;
  this->BitstreamId = 0;
  this->UserId = 0;
  this->ParentItem = 0;
}

PartialUpload::~PartialUpload()
{
}

bool PartialUpload::Commit()
{
  if(this->BitstreamId <= 0 || this->Token == "" || this->UserId <= 0
     || this->ParentItem <= 0)
    {
    return false;
    }
  mds::DatabaseAPI db;
  db.Open();

  std::stringstream query;
  query << "INSERT INTO partial_upload (bitstream_id, uploadtoken, user_id, "
    "item_id) VALUES ('" << this->BitstreamId << "', '"
    << this->Token << "', '"
    << this->UserId << "', '"
    << this->ParentItem << "')";

  if(!db.Database->ExecuteQuery(query.str().c_str()))
    {
    db.Close();
    return false;
    }
  db.Close();
  return true;
}

bool PartialUpload::Remove()
{
  if(this->BitstreamId <= 0)
    {
    return false;
    }
  mds::DatabaseAPI db;
  db.Open();

  std::stringstream query;
  query << "DELETE FROM partial_upload WHERE bitstream_id='" <<
    this->BitstreamId << "'";

  if(!db.Database->ExecuteQuery(query.str().c_str()))
    {
    db.Close();
    return false;
    }
  db.Close();
  return true;
}

bool PartialUpload::FetchAll(std::vector<mds::PartialUpload*>& list)
{
  mds::DatabaseAPI db;
  db.Open();

  if(!db.Database->ExecuteQuery("SELECT id, uploadtoken, bitstream_id, "
                                "user_id, item_id FROM partial_upload"))
    {
    db.Close();
    return false;
    }
  while(db.Database->GetNextRow())
    {
    mds::PartialUpload* ul = new mds::PartialUpload;
    ul->SetId(db.Database->GetValueAsInt(0));
    ul->SetToken(db.Database->GetValueAsString(1));
    ul->SetBitstreamId(db.Database->GetValueAsInt(2));
    ul->SetUserId(db.Database->GetValueAsInt(3));
    ul->SetParentItem(db.Database->GetValueAsInt(4));
    list.push_back(ul);
    }
  db.Close();
  return true;
}

bool PartialUpload::RemoveAll()
{
  mds::DatabaseAPI db;
  db.Open();

  if(!db.Database->ExecuteQuery("DELETE FROM partial_upload"))
    {
    db.Close();
    return false;
    }

  db.Close();
  return true;
}

void PartialUpload::SetId(int id)
{
  this->Id = id;
}

int PartialUpload::GetId()
{
  return this->Id;
}

void PartialUpload::SetBitstreamId(int id)
{
  this->BitstreamId = id;
}

int PartialUpload::GetBitstreamId()
{
  return this->BitstreamId;
}

void PartialUpload::SetToken(const std::string& token)
{
  this->Token = token;
}

std::string PartialUpload::GetToken()
{
  return this->Token;
}

void PartialUpload::SetUserId(int id)
{
  this->UserId = id;
}

int PartialUpload::GetUserId()
{
  return this->UserId;
}

void PartialUpload::SetParentItem(int id)
{
  this->ParentItem = id;
}

int PartialUpload::GetParentItem()
{
  return this->ParentItem;
}

}