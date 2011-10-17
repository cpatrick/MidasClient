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

#include "mdoItem.h"
#include "mdoCollection.h"
#include "midasStandardIncludes.h"

#include <QString>

namespace mdo
{

/** Constructor */
Item::Item()
{
  m_Id = 0;
  m_BitstreamCount = 0;
  m_ParentCollection = NULL;
}

/** Destructor */
Item::~Item()
{
}

/** Load */
bool Item::Load()
{
  return m_Proxy->Load();
}

void Item::Clear()
{
  this->m_Title = "";
  this->m_Abstract = "";
  this->m_Description = "";
  this->m_Uuid = "";
  this->m_Parent = "";
  this->m_HasAgreement = "";
  this->m_Size = "";
  this->m_Keywords.clear();
  this->m_Authors.clear();
  this->m_Bitstreams.clear();
}

std::string Item::GetAuthorsString()
{
  std::string output;
  bool        first = true;
  for( std::vector<std::string>::iterator i = m_Authors.begin();
       i != m_Authors.end(); ++i )
    {
    if( !first )
      {
      output += " / ";
      }
    else
      {
      first = false;
      }

    output += *i;
    }
  return output;
}

std::string Item::GetKeywordsString()
{
  std::string output;
  bool        first = true;
  for( std::vector<std::string>::iterator i = m_Keywords.begin();
       i != m_Keywords.end(); ++i )
    {
    if( !first )
      {
      output += " / ";
      }
    else
      {
      first = false;
      }

    output += *i;
    }
  return output;
}

bool Item::SetValue(std::string key, std::string value, bool append)
{
  QString keyStr = key.c_str();

  keyStr = keyStr.toUpper();
  key = keyStr.toStdString();

  if( key == "TITLE" )
    {
    if( append )
      {
      m_Title += value;
      }
    else
      {
      m_Title = value;
      }
    return true;
    }
  if( key == "ABSTRACT" )
    {
    if( append )
      {
      m_Abstract += value;
      }
    else
      {
      m_Abstract = value;
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
  if( key == "AUTHORS" )
    {
    if( append )
      {
      this->AddAuthor(value);
      }
    else
      {
      midasUtils::Tokenize(value, m_Authors, "/", true);
      }
    return true;
    }
  if( key == "KEYWORDS" )
    {
    if( append )
      {
      this->AddKeyword(value);
      }
    else
      {
      midasUtils::Tokenize(value, m_Keywords, "/", true);
      }
    return true;
    }

  return false;
}

void Item::SetTitle(const char* title)
{
  m_Title = title;
}

std::string & Item::GetTitle()
{
  return m_Title;
}

std::string & Item::GetName()
{
  return m_Title;
}

void Item::SetDescription(const char* desc)
{
  m_Description = desc;
}
std::string & Item::GetDescription()
{
  return m_Description;
}

// Set/Get the abstract of the item
void Item::SetAbstract(const char* abstract)
{
  m_Abstract = abstract;
}

std::string & Item::GetAbstract()
{
  return m_Abstract;
}

void Item::SetAuthors(std::vector<std::string> authors)
{
  m_Authors = authors;
}

void Item::AddAuthor(std::string author)
{
  m_Authors.push_back(author);
}

std::vector<std::string> & Item::GetAuthors()
{
  return m_Authors;
}

void Item::SetKeywords(std::vector<std::string> keywords)
{
  m_Keywords = keywords;
}

void Item::AddKeyword(std::string keyword)
{
  m_Keywords.push_back(keyword);
}

std::vector<std::string> & Item::GetKeywords()
{
  return m_Keywords;
}

// Get the list of bitstreams
const std::vector<Bitstream *> & Item::GetBitstreams()
{
  return m_Bitstreams;
}

// Add a bitstream to the list
void Item::AddBitstream(Bitstream* bitstream)
{
  m_Bitstreams.push_back(bitstream);
}

void Item::SetBitstreamCount(unsigned int count)
{
  m_BitstreamCount = count;
}

unsigned int Item::GetBitstreamCount()
{
  return m_BitstreamCount;
}

std::string Item::GetTypeName()
{
  return "Item";
}

int Item::GetResourceType()
{
  return midasResourceType::ITEM;
}

Collection * Item::GetParentCollection()
{
  return m_ParentCollection;
}

void Item::SetParentCollection(Collection* coll)
{
  m_ParentCollection = coll;
}

} // end namespace
