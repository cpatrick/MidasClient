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

void Folder::SetName(const char* name)
{
  m_Name = name;
}

std::string & Folder::GetName()
{
  return m_Name;
}

// Set/Get description
void Folder::SetDescription(const char* description)
{
  m_Description = description;
}

std::string & Folder::GetDescription()
{
  return m_Description;
}

// Get the list of child folders
std::vector<Folder *> & Folder::GetFolders()
{
  return m_Folders;
}

std::vector<Item *> & Folder::GetItems()
{
  return m_Items;
}

std::string Folder::GetTypeName()
{
  return "Folder";
}

int Folder::GetResourceType()
{
  return midas3ResourceType::FOLDER;
}

void Folder::SetBitstreamCount(unsigned int count)
{
  m_BitstreamCount = count;
}

unsigned int Folder::GetBitstreamCount()
{
  return m_BitstreamCount;
}

void Folder::SetParentFolder(Folder* folder)
{
  m_ParentFolder = folder;
}

Folder * Folder::GetParentFolder()
{
  return m_ParentFolder;
}

void Folder::SetPath(const std::string& path)
{
  m_Path = path;
}

std::string & Folder::GetPath()
{
  return m_Path;
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
