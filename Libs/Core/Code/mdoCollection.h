/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef _mdoCollection_h_
#define _mdoCollection_h_

#include <string>
#include <vector>

#include "mdoObject.h"
#include "mdoItem.h"

namespace mdo
{

class Community;

/** This class represent an collection on the MIDAS server.
 *  An collection has many bitstreams. */
class Collection : public Object
{
public:

  Collection();
  ~Collection();

  // Fill the collection given the id
  bool Load();

  void Clear();

  // Set/Get the uuid
  void SetUuid(const char* uuid)
  {
    m_Uuid = uuid;
  }
  std::string & GetUuid()
  {
    return m_Uuid;
  }

  // Set/Get the name of the collection
  void SetName(const char* name)
  {
    m_Name = name;
  }
  std::string & GetName()
  {
    return m_Name;
  }

  // Set/Get the description of the collection
  void SetDescription(const char* description)
  {
    m_Description = description;
  }
  std::string & GetDescription()
  {
    return m_Description;
  }

  // Set/Get introductory text
  void SetIntroductoryText(const char* text)
  {
    m_IntroductoryText = text;
  }
  std::string & GetIntroductoryText()
  {
    return m_IntroductoryText;
  }

  // Set/Get copyright
  void SetCopyright(const char* copyright)
  {
    m_Copyright = copyright;
  }
  std::string & GetCopyright()
  {
    return m_Copyright;
  }

  // Return the list of items
  std::vector<Item *> & GetItems()
  {
    return m_Items;
  }

  // Add an item
  void AddItem(Item* item)
  {
    m_Items.push_back(item);
  }

  std::string GetTypeName()
  {
    return "Collection";
  }
  int GetResourceType()
  {
    return midasResourceType::COLLECTION;
  }

  Community * GetParentCommunity()
  {
    return m_ParentCommunity;
  }
  void SetParentCommunity(Community* comm)
  {
    m_ParentCommunity = comm;
  }

  bool SetValue(std::string key, std::string value, bool append = false);

  void SetBitstreamCount(unsigned int count)
  {
    m_BitstreamCount = count;
  }
  unsigned int GetBitstreamCount()
  {
    return m_BitstreamCount;
  }
protected:

  friend class CollectionXMLParser;

  std::string  m_Name;
  std::string  m_Description;
  std::string  m_Copyright;
  std::string  m_IntroductoryText;
  unsigned int m_BitstreamCount;

  Community* m_ParentCommunity;

  std::vector<Item *> m_Items;
};

} // end namespace

#endif // _mdoCollection_h_
