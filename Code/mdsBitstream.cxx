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
    m_Database->GetLog()->Error("Bitstream::Fetch : Bitstream not set\n");
    return false;
    }
    
  if(m_Bitstream->GetId() == 0)
    {
    m_Database->GetLog()->Error("Bitstream::Fetch : BitstreamId not set\n");
    return false;
    }

  if(m_Bitstream->IsFetched())
    {
    return true;
    }

  std::stringstream query;
  query << "SELECT size_bytes, name, internal_id FROM bitstream WHERE bitstream_id='"
    << m_Bitstream->GetId() << "'";
  m_Database->Open();
  m_Database->GetDatabase()->ExecuteQuery(query.str().c_str());

  while(m_Database->GetDatabase()->GetNextRow())
    {
    std::stringstream val;
    val << m_Database->GetDatabase()->GetValueAsInt64(0);
    m_Bitstream->SetSize(val.str());
    m_Bitstream->SetName(m_Database->GetDatabase()->GetValueAsString(1));
    m_Bitstream->SetPath(m_Database->GetDatabase()->GetValueAsString(2));
    }
  m_Bitstream->SetFetched(true);
  m_Database->Close();
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

  m_Database->Open();
  if(m_Database->GetDatabase()->ExecuteQuery(query.str().c_str()))
    {
    m_Database->Close();
    if(m_MarkDirty)
      {
      m_Database->MarkDirtyResource(m_Bitstream->GetUuid(), midasDirtyAction::MODIFIED);
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
  m_Database->GetDatabase()->Open(m_Database->GetDatabasePath().c_str());
  m_Database->GetDatabase()->ExecuteQuery("BEGIN");
  std::stringstream query;
  query << "DELETE FROM bitstream WHERE bitstream_id='" << m_Bitstream->GetId() << "'";
  if(!m_Database->GetDatabase()->ExecuteQuery(query.str().c_str()))
    {
    m_Database->GetDatabase()->ExecuteQuery("ROLLBACK");
    m_Database->GetDatabase()->Close();
    return false;
    }

  query.str(std::string());
  query << "DELETE FROM item2bitstream WHERE bitstream_id='" <<
    m_Bitstream->GetId() << "'";
  if(!m_Database->GetDatabase()->ExecuteQuery(query.str().c_str()))
    {
    m_Database->GetDatabase()->ExecuteQuery("ROLLBACK");
    m_Database->GetDatabase()->Close();
    return false;
    }

  query.str(std::string());
  query << "DELETE FROM dirty_resource WHERE uuid='" <<
    m_Bitstream->GetUuid() << "'";
  if(!m_Database->GetDatabase()->ExecuteQuery(query.str().c_str()))
    {
    m_Database->GetDatabase()->ExecuteQuery("ROLLBACK");
    m_Database->GetDatabase()->Close();
    return false;
    }

  query.str(std::string());
  query << "DELETE FROM resource_uuid WHERE uuid='" <<
    m_Bitstream->GetUuid() << "'";
  if(!m_Database->GetDatabase()->ExecuteQuery(query.str().c_str()))
    {
    m_Database->GetDatabase()->ExecuteQuery("ROLLBACK");
    m_Database->GetDatabase()->Close();
    return false;
    }
  m_Database->GetDatabase()->ExecuteQuery("COMMIT");
  m_Database->GetDatabase()->Close();
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

  m_Database->Open();
  m_Database->GetDatabase()->ExecuteQuery(query.str().c_str());
  m_Database->Close();
}

} // end namespace
