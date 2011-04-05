#include "mdsDatabaseInfo.h"

namespace mds {

/** Singleton */
DatabaseInfo* DatabaseInfo::m_Instance = NULL; 

/** Return the instance as a singleton */
DatabaseInfo* DatabaseInfo::Instance()
{
  if (m_Instance != NULL)
    {
    return m_Instance; 
    }
  else 
    {
    m_Instance = new DatabaseInfo();
    return m_Instance;
    }
}

/** Constructor */
DatabaseInfo::DatabaseInfo()
{
  m_Path = "";
  m_ResourceUpdateHandler = NULL;
}

/** Destructor */
DatabaseInfo::~DatabaseInfo()
{
}

void DatabaseInfo::SetPath(const std::string& path)
{
  m_Path = path;
}

std::string DatabaseInfo::GetPath()
{
  return m_Path;
}

void DatabaseInfo::SetResourceUpdateHandler(ResourceUpdateHandler* handler)
{
  this->m_ResourceUpdateHandler = handler;
}

ResourceUpdateHandler* DatabaseInfo::GetResourceUpdateHandler()
{
  return this->m_ResourceUpdateHandler;
}

} //end namespace
