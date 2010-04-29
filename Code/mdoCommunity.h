/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef _mdoCommunity_h_
#define _mdoCommunity_h_

#include <string>
#include <vector>

#include "mdoObject.h"
#include "mdoCollection.h"

namespace mdo{

/** This class represent an community on the MIDAS server. 
 *  An community has many bitstreams. */
class Community : public Object
{
public:

  Community();
  ~Community();

  // Load the community
  bool Load();
  bool LoadTree();
    
  // Set/Get ID
  void SetId(int id) { this->m_Id = id; }
  const int GetId() {return this->m_Id; }

  // Set/Get the ID of the parent
  void SetParent(std::string id) { m_Parent = id; }
  std::string & GetParent() {return m_Parent;}
  const int GetParentId() {return atoi(m_Parent.c_str());}

  // Set/Get the uuid
  void SetUuid(const char* uuid) { m_Uuid = uuid; }
  std::string & GetUuid() {return m_Uuid;}

  // Set/Get name
  void SetName(const char* name) { m_Name = name; }
  std::string & GetName() {return m_Name;}
  
  // Set/Get description
  void SetDescription(const char* description) { m_Description = description; }
  std::string & GetDescription() {return m_Description;}
  
  // Set/Get introductory text
  void SetIntroductoryText(const char* text) { m_IntroductoryText = text; }
  std::string & GetIntroductoryText() {return m_IntroductoryText;}
  
  // Set/Get copyright
  void SetCopyright(const char* copyright) { m_Copyright = copyright; }
  std::string & GetCopyright() {return m_Copyright;}
  
  // Set/Get links
  void SetLinks(const char* links) { m_Links = links; }
  std::string & GetLinks() {return m_Links;}
  
  // Get the list of sub-communities
  const std::vector<Community*>& GetCommunities() { return m_Communities; }
  
  // Get the list of collection
  const std::vector<Collection*>& GetCollections() { return m_Collections; }
  
  // Display all the tags and their respective names and values.
  void Print(std::ostream &os, int indent=-1);
 
  // Set the parent
  void SetParent(Community* community);
 
  // Add a sub community
  void AddCommunity(Community* community);
   
  // Add a collection
  void AddCollection(Collection* collection);
  
  // Get the parent
  Community* GetParentCommunity() {return m_ParentComm;}
  
protected:

  friend class CommunityXMLParser;
  
  unsigned int m_Id;
  std::string  m_Parent;
  std::string  m_Name;
  std::string  m_Description;
  std::string  m_Copyright;
  std::string  m_IntroductoryText;
  std::string  m_Links;
  std::string  m_Uuid;
   
  Community*   m_ParentComm; // pointer the parent community
  
  std::vector<Community*>    m_Communities;
  std::vector<Collection*>   m_Collections;
};

} //end namespace

#endif //_mdoCommunity_h_
