/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "m3doFolder.h"
#include "m3doItem.h"
#include "midasStandardIncludes.h"

#include <QString>

namespace m3do
{

Folder::Folder()
{
  m_Id = 0;
  m_BitstreamCount = 0;
  m_ParentFolder = NULL;
}

// copy constructor
Folder::Folder(Folder* other)
{
  m_Id = other->GetId();
  m_Uuid = other->GetUuid();
  m_ParentFolder = other->GetParentFolder();
  m_Path = other->GetPath();
  m_Name = other->GetName();
  m_Description = other->GetDescription();
  m_Folders = other->GetFolders();
  m_Items = other->GetItems();
}

Folder::~Folder()
{
  // Delete the subtree
  std::vector<Folder *>::iterator itF = m_Folders.begin();
  while( itF != m_Folders.end() )
    {
    Folder* f = *itF;
    itF++;
    delete f;
    }

  std::vector<Item *>::iterator itI = m_Items.begin();
  while( itI != m_Items.end() )
    {
    Item* i = *itI;
    itI++;
    delete i;
    }
}

void Folder::AddFolder(Folder* folder)
{
  m_Folders.push_back(folder);
}

void Folder::AddItem(Item* item)
{
  m_Items.push_back(item);
}

void Folder::Clear()
{
  this->m_Name = "";
  this->m_Description = "";
  this->m_Uuid = "";
  this->m_HasAgreement = "";
  this->m_Size = "";
}

/** Load */
bool Folder::Load()
{
  return m_Proxy->Load();
}

/** Fill the full tree with Folder and collection */
bool Folder::LoadTree()
{
  return m_Proxy->LoadTree();
}

bool Folder::SetValue(std::string key, std::string value, bool append)
{
  QString keyStr = key.c_str();

  keyStr = keyStr.toUpper();
  key = keyStr.toStdString();

  if( key == "NAME" )
    {
    if( append )
      {
      m_Name += value;
      }
    else
      {
      m_Name = value;
      }
    return true;
    }
  if( key == "DESCRIPTION" )
    {
    if( append )
      {
      m_Description += value;
      }
    else
      {
      m_Description = value;
      }
    return true;
    }

  return false;
}

} // end namespace
