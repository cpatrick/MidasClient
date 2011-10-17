/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mdoCommunity.h"
#include "midasStandardIncludes.h"

#include <QString>

namespace mdo
{

/** Constructor */
Community::Community()
{
  m_Id = 0;
  m_BitstreamCount = 0;
  m_ParentCommunity = NULL;
}

/** Destructor */
Community::~Community()
{
  // Clean the arrays
  std::vector<Community *>::iterator itCom = m_Communities.begin();
  while( itCom != m_Communities.end() )
    {
    Community* com = *itCom;
    itCom++;
    delete com;
    }

  m_Communities.clear();

  std::vector<Collection *>::iterator itCol = m_Collections.begin();
  while( itCol != m_Collections.end() )
    {
    Collection* col = *itCol;
    itCol++;
    delete col;
    }

  m_Collections.clear();
}

/** Add a sub community */
void Community::AddCommunity(Community* community)
{
  m_Communities.push_back(community);
}

/** Add a collection */
void Community::AddCollection(Collection* collection)
{
  m_Collections.push_back(collection);
}

void Community::Clear()
{
  this->m_Name = "";
  this->m_Description = "";
  this->m_IntroductoryText = "";
  this->m_Copyright = "";
  this->m_Links = "";
  this->m_Uuid = "";
  this->m_Parent = "";
  this->m_HasAgreement = "";
  this->m_Size = "";
}

/** Load */
bool Community::Load()
{
  return m_Proxy->Load();
}

/** Fill the full tree with community and collection */
bool Community::LoadTree()
{
  return m_Proxy->LoadTree();
}

bool Community::SetValue(std::string key, std::string value, bool append)
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

} // end namespace
