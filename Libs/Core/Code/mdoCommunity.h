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

namespace mdo
{

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
  void SetId(int id);
  int GetId();

  // Set/Get the uuid
  void SetUuid(const char* uuid);
  std::string & GetUuid();

  // Set/Get name
  void SetName(const char* name);
  std::string & GetName();

  // Set/Get description
  void SetDescription(const char* description);
  std::string & GetDescription();

  // Set/Get introductory text
  void SetIntroductoryText(const char* text);
  std::string & GetIntroductoryText();

  // Set/Get copyright
  void SetCopyright(const char* copyright);
  std::string & GetCopyright();

  // Set/Get links
  void SetLinks(const char* links);
  std::string & GetLinks();

  // Get the list of sub-communities
  std::vector<Community *> & GetCommunities();

  // Get the list of collection
  std::vector<Collection *> & GetCollections();

  // Set the parent
  void SetParentCommunity(Community* comm);
  Community * GetParentCommunity();

  // Add a sub community
  void AddCommunity(Community* community);

  // Add a collection
  void AddCollection(Collection* collection);

  void Clear();

  std::string GetTypeName();
  int GetResourceType();

  bool SetValue(std::string key, std::string value, bool append = false);

  void SetBitstreamCount(unsigned int count);
  unsigned int GetBitstreamCount();

protected:

  friend class CommunityXMLParser;

  std::string  m_Name;
  std::string  m_Description;
  std::string  m_Copyright;
  std::string  m_IntroductoryText;
  std::string  m_Links;
  unsigned int m_BitstreamCount;

  Community* m_ParentCommunity;

  std::vector<Community *>  m_Communities;
  std::vector<Collection *> m_Collections;
};

} // end namespace

#endif // _mdoCommunity_h_
