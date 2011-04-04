/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mdsBitstream.h"
#include "mdoBitstream.h"
#include "midasStandardIncludes.h"

namespace mds{

/** Constructor */
Bitstream::Bitstream()
{
  m_Bitstream = 0;
}
  
/** Destructor */
Bitstream::~Bitstream()
{
}

/** Fetch the object */
bool Bitstream::Fetch()
{
  if(!m_Bitstream)
    {
    mds::DatabaseAPI::Instance()->GetLog()->Error("Bitstream::Fetch : Bitstream not set\n");
    return false;
    }
    
  if(m_Bitstream->GetId() == 0)
    {
    mds::DatabaseAPI::Instance()->GetLog()->Error("Bitstream::Fetch : BitstreamId not set\n");
    return false;
    }

  if(m_Bitstream->IsFetched())
    {
    return true;
    }

  std::stringstream query;
  query << "SELECT size_bytes, name, internal_id FROM bitstream WHERE bitstream_id='"
    << m_Bitstream->GetId() << "'";
  mds::DatabaseAPI::Instance()->Open();
  mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str());

  while(mds::DatabaseAPI::Instance()->Database->GetNextRow())
    {
    std::stringstream val;
    val << mds::DatabaseAPI::Instance()->Database->GetValueAsInt64(0);
    m_Bitstream->SetSize(val.str());
    m_Bitstream->SetName(mds::DatabaseAPI::Instance()->Database->GetValueAsString(1));
    m_Bitstream->SetPath(mds::DatabaseAPI::Instance()->Database->GetValueAsString(2));
    }
  m_Bitstream->SetFetched(true);
  mds::DatabaseAPI::Instance()->Close();
  return true;
}

/** Commit the object */
bool Bitstream::Commit()
{
  std::stringstream query;
  query << "UPDATE bitstream SET "
    << "size_bytes='" << m_Bitstream->GetSize() << "', "
    << "name='" << midasUtils::EscapeForSQL(m_Bitstream->GetName()) << "', "
    << "last_modified='" << m_Bitstream->GetLastModified()
    << "' WHERE bitstream_id='" << m_Bitstream->GetId() << "'";

  mds::DatabaseAPI::Instance()->Open();
  if(mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str()))
    {
    mds::DatabaseAPI::Instance()->Close();
    if(m_MarkDirty)
      {
      mds::DatabaseAPI::Instance()->MarkDirtyResource(m_Bitstream->GetUuid(), midasDirtyAction::MODIFIED);
      }
    return true;
    }
  return false;
}

bool Bitstream::FetchTree()
{
  return true;
}

bool Bitstream::Delete(bool deleteOnDisk)
{
  mds::DatabaseAPI::Instance()->Database->Open(mds::DatabaseAPI::Instance()->GetDatabasePath().c_str());
  mds::DatabaseAPI::Instance()->Database->ExecuteQuery("BEGIN");
  std::stringstream query;
  query << "DELETE FROM bitstream WHERE bitstream_id='" << m_Bitstream->GetId() << "'";
  if(!mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str()))
    {
    mds::DatabaseAPI::Instance()->Database->ExecuteQuery("ROLLBACK");
    mds::DatabaseAPI::Instance()->Database->Close();
    return false;
    }

  query.str(std::string());
  query << "DELETE FROM item2bitstream WHERE bitstream_id='" <<
    m_Bitstream->GetId() << "'";
  if(!mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str()))
    {
    mds::DatabaseAPI::Instance()->Database->ExecuteQuery("ROLLBACK");
    mds::DatabaseAPI::Instance()->Database->Close();
    return false;
    }

  query.str(std::string());
  query << "DELETE FROM dirty_resource WHERE uuid='" <<
    m_Bitstream->GetUuid() << "'";
  if(!mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str()))
    {
    mds::DatabaseAPI::Instance()->Database->ExecuteQuery("ROLLBACK");
    mds::DatabaseAPI::Instance()->Database->Close();
    return false;
    }

  query.str(std::string());
  query << "DELETE FROM resource_uuid WHERE uuid='" <<
    m_Bitstream->GetUuid() << "'";
  if(!mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str()))
    {
    mds::DatabaseAPI::Instance()->Database->ExecuteQuery("ROLLBACK");
    mds::DatabaseAPI::Instance()->Database->Close();
    return false;
    }
  mds::DatabaseAPI::Instance()->Database->ExecuteQuery("COMMIT");
  mds::DatabaseAPI::Instance()->Database->Close();
  if(deleteOnDisk)
    {
    kwsys::SystemTools::RemoveFile(this->m_Path.c_str());
    }
  return true;
}

// Add the object
void Bitstream::SetObject(mdo::Object* object)
{  
  m_Bitstream = reinterpret_cast<mdo::Bitstream*>(object);
}

void Bitstream::SetPath(std::string path)
{
  m_Path = path;
}

void Bitstream::ParentPathChanged(std::string parentPath)
{
  std::string newPath = parentPath + "/" + m_Bitstream->GetName();
  std::stringstream query;
  query << "UPDATE resource_uuid SET path='" << newPath << "' WHERE "
    "uuid='" << m_Bitstream->GetUuid() << "'";

  mds::DatabaseAPI::Instance()->Open();
  mds::DatabaseAPI::Instance()->Database->ExecuteQuery(query.str().c_str());
  mds::DatabaseAPI::Instance()->Close();
}

} // end namespace
