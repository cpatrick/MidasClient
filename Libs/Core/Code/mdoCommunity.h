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
  int GetId() {return this->m_Id; }

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
  std::vector<Community*>& GetCommunities() { return m_Communities; }
  
  // Get the list of collection
  std::vector<Collection*>& GetCollections() { return m_Collections; }
 
  // Set the parent
  void SetParentCommunity(Community* comm) { m_ParentCommunity = comm; }
  Community* GetParentCommunity() { return m_ParentCommunity; }
 
  // Add a sub community
  void AddCommunity(Community* community);
   
  // Add a collection
  void AddCollection(Collection* collection);
  
  void Clear();

  std::string GetTypeName() { return "Community"; }
  int GetResourceType() { return midasResourceType::COMMUNITY; }

  bool SetValue(std::string key, std::string value, bool append = false);

  void SetBitstreamCount(unsigned int count) {m_BitstreamCount = count;}
  unsigned int GetBitstreamCount() {return m_BitstreamCount;}
  
protected:

  friend class CommunityXMLParser;
  
  std::string  m_Name;
  std::string  m_Description;
  std::string  m_Copyright;
  std::string  m_IntroductoryText;
  std::string  m_Links;
  unsigned int m_BitstreamCount;
   
  Community*   m_ParentCommunity;
  
  std::vector<Community*>    m_Communities;
  std::vector<Collection*>   m_Collections;
};

} //end namespace

#endif //_mdoCommunity_h_
