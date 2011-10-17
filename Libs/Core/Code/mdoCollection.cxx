/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mdoCollection.h"
#include "mdoCommunity.h"
#include "midasStandardIncludes.h"

#include <QString>

namespace mdo
{

/** Constructor */
Collection::Collection()
{
  m_Id = 0;
  m_BitstreamCount = 0;
  m_ParentCommunity = NULL;
}

/** Destructor */
Collection::~Collection()
{
  // Clean the items
  std::vector<Item *>::iterator itItem = m_Items.begin();
  while( itItem != m_Items.end() )
    {
    Item* item = *itItem;
    itItem++;
    delete item;
    }

  m_Items.clear();
}

/** Load */
bool Collection::Load()
{
  return m_Proxy->Load();
}

void Collection::Clear()
{
  this->m_Name = "";
  this->m_Description = "";
  this->m_IntroductoryText = "";
  this->m_Copyright = "";
  this->m_Uuid = "";
  this->m_Parent = "";
  this->m_HasAgreement = "";
  this->m_Size = "";
}

bool Collection::SetValue(std::string key, std::string value, bool append)
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
  if( key == "COPYRIGHT" )
    {
    if( append )
      {
      m_Copyright += value;
      }
    else
      {
      m_Copyright = value;
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
  if( key == "INTRODUCTORY" || key == "INTRODUCTORYTEXT" )
    {
    if( append )
      {
      m_IntroductoryText += value;
      }
    else
      {
      m_IntroductoryText = value;
      }
    return true;
    }

  return false;
}

// Set/Get the uuid
void Collection::SetUuid(const char* uuid)
{
  m_Uuid = uuid;
}

std::string & Collection::GetUuid()
{
  return m_Uuid;
}

// Set/Get the name of the collection
void Collection::SetName(const char* name)
{
  m_Name = name;
}

std::string & Collection::GetName()
{
  return m_Name;
}

// Set/Get the description of the collection
void Collection::SetDescription(const char* description)
{
  m_Description = description;
}
std::string & Collection::GetDescription()
{
  return m_Description;
}

// Set/Get introductory text
void Collection::SetIntroductoryText(const char* text)
{
  m_IntroductoryText = text;
}
std::string & Collection::GetIntroductoryText()
{
  return m_IntroductoryText;
}

// Set/Get copyright
void Collection::SetCopyright(const char* copyright)
{
  m_Copyright = copyright;
}

std::string & Collection::GetCopyright()
{
  return m_Copyright;
}

// Return the list of items
std::vector<Item *> & Collection::GetItems()
{
  return m_Items;
}

// Add an item
void Collection::AddItem(Item* item)
{
  m_Items.push_back(item);
}

std::string Collection::GetTypeName()
{
  return "Collection";
}

int Collection::GetResourceType()
{
  return midasResourceType::COLLECTION;
}

Community * Collection::GetParentCommunity()
{
  return m_ParentCommunity;
}

void Collection::SetParentCommunity(Community* comm)
{
  m_ParentCommunity = comm;
}

void Collection::SetBitstreamCount(unsigned int count)
{
  m_BitstreamCount = count;
}

unsigned int Collection::GetBitstreamCount()
{
  return m_BitstreamCount;
}

} // end namespace
