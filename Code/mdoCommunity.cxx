/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mdoCommunity.h"
#include "mdoCollection.h"
#include <sstream>
#include <iostream>

namespace mdo{

/** Constructor */
Community::Community()
{
  m_Id = 0;
  m_ParentCommunity = NULL;
  fetched = false;
}
  
/** Destructor */
Community::~Community()
{
  // Clean the arrays
  std::vector<Community*>::iterator itCom = m_Communities.begin();
  while(itCom != m_Communities.end())
    {
    Community* com = *itCom;
    itCom++;
    delete com;
    }
  m_Communities.clear();  
  
  std::vector<Collection*>::iterator itCol = m_Collections.begin();
  while(itCol != m_Collections.end())
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
//------------------------------------------------------------------------
void Community::Print(std::ostream &os, int indent)
{
  os << "Level: " << indent << "\n";
  os << "Id: " << m_Id << "\n";
  os << "Name: " << m_Name.c_str() << "\n";
  os << "Description: " << m_Description.c_str() << "\n";
  os << "# of collections: " << m_Collections.size() << "\n";
  // Display all the collections in the list
  for(unsigned int i=0; i < m_Collections.size(); i++)
    {
    m_Collections[i]->Print(os, indent+1);
    }  
    
  // Display all the communities in the list  
  os << "# of communities: " << m_Communities.size() << "\n"; 
  for(unsigned int i=0; i < m_Communities.size(); i++)
    {
    m_Communities[i]->Print(os, indent+1);
    }
}

} // end namespace