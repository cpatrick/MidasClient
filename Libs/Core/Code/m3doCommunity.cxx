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

#include "m3doCommunity.h"
#include "midasStandardIncludes.h"

#include <QString>

namespace m3do
{

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

int & Community::GetFolderId()
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
