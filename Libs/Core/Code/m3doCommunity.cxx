/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "m3doCommunity.h"
#include "midasStandardIncludes.h"

#include <QString>

namespace m3do{

Community::Community()
: Folder(), m_FolderId(0)
{
}

Community::Community(Community* other)
{
  m_Id = other->GetId();
  m_FolderId = other->GetFolderId();
  m_Uuid = other->GetUuid();
  m_Path = other->GetPath();
  m_Name = other->GetName();
  m_Description = other->GetDescription();
  m_Folders = other->GetFolders();
  m_Items = other->GetItems();
}

Community::~Community()
{  
}

void Community::SetFolderId(int id)
{
  m_FolderId = id;
}

int& Community::GetFolderId()
{
  return m_FolderId;
}

std::string Community::GetTypeName()
{
  return "Community";
}

int Community::GetResourceType()
{
  return midas3ResourceType::COMMUNITY;
}

void Community::SetParentFolder(Folder* folder)
{
  (void)folder;
}

} // end namespace
