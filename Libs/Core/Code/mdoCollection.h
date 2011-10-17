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
  void SetUuid(const char* uuid);
  std::string & GetUuid();

  // Set/Get the name of the collection
  void SetName(const char* name);
  std::string & GetName();

  // Set/Get the description of the collection
  void SetDescription(const char* description);
  std::string & GetDescription();

  // Set/Get introductory text
  void SetIntroductoryText(const char* text);
  std::string & GetIntroductoryText();

  // Set/Get copyright
  void SetCopyright(const char* copyright);
  std::string & GetCopyright();

  // Return the list of items
  std::vector<Item *> & GetItems();

  // Add an item
  void AddItem(Item* item);

  std::string GetTypeName();
  int GetResourceType();

  Community * GetParentCommunity();
  void SetParentCommunity(Community* comm);

  bool SetValue(std::string key, std::string value, bool append = false);

  void SetBitstreamCount(unsigned int count);
  unsigned int GetBitstreamCount();

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
