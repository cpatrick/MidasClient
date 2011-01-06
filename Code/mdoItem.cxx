/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mdoItem.h"
#include "mdoCollection.h"
#include <sstream>
#include <iostream>
#include "midasStandardIncludes.h"

namespace mdo{

/** Constructor */
Item::Item()
{
  m_Id = 0;
  m_ParentCollection = NULL;
  fetched = false;
}
  
/** Destructor */
Item::~Item()
{
  delete m_ParentCollection;
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
  this->m_HasAgreement = "0";
  this->m_Keywords.clear();
  this->m_Authors.clear();
  this->m_Bitstreams.clear();
}

std::string Item::GetAuthorsString()
{
  std::string output;
  bool first = true;

  for(std::vector<std::string>::iterator i = m_Authors.begin();
      i != m_Authors.end(); ++i)
    {
    if(!first)
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
  bool first = true;

  for(std::vector<std::string>::iterator i = m_Keywords.begin();
      i != m_Keywords.end(); ++i)
    {
    if(!first)
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
  key = kwsys::SystemTools::UpperCase(key);

  if(key == "TITLE")
    {
    if(append)
      {
      m_Title += value;
      }
    else
      {
      m_Title = value;
      }
    return true;
    }
  if(key == "ABSTRACT")
    {
    if(append)
      {
      m_Abstract += value;
      }
    else
      {
      m_Abstract = value;
      }
    return true;
    }
  if(key == "DESCRIPTION")
    {
    if(append)
      {
      m_Description += value;
      }
    else
      {
      m_Description = value;
      }
    return true;
    }
  if(key == "AUTHORS")
    {
    if(append)
      {
      this->AddAuthor(value);
      }
    else
      {
      midasUtils::Tokenize(value, m_Authors, "/", true);
      }
    return true;
    }
  if(key == "KEYWORDS")
    {
    if(append)
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

} // end namespace
