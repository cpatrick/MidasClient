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


#ifndef _mdoItem_h_
#define _mdoItem_h_

#include <string>
#include <vector>

#include "mdoObject.h"
#include "mdoBitstream.h"

namespace mdo
{

class Collection;

/** This class represent an item on the MIDAS server.
 *  An item has many bitstreams. */
class Item : public Object
{
public:

  Item();
  ~Item();

  // Load
  bool Load();

  void Clear();

  // Set/Get the title of the item
  void SetTitle(const char* title);
  std::string & GetTitle();

  std::string & GetName();

  // Set/Get the description of the item
  void SetDescription(const char* desc);
  std::string & GetDescription();

  // Set/Get the abstract of the item
  void SetAbstract(const char* abstract);
  std::string & GetAbstract();

  void SetAuthors(std::vector<std::string> authors);
  void AddAuthor(std::string author);
  std::vector<std::string> & GetAuthors();
  std::string GetAuthorsString();

  void SetKeywords(std::vector<std::string> keywords);
  void AddKeyword(std::string keyword);
  std::vector<std::string> & GetKeywords();
  std::string GetKeywordsString();

  // Get the list of bitstreams
  const std::vector<Bitstream *> & GetBitstreams();

  // Add a bitstream to the list
  void AddBitstream(Bitstream* bitstream);

  void SetBitstreamCount(unsigned int count);
  unsigned int GetBitstreamCount();

  std::string GetTypeName();
  int GetResourceType();

  Collection * GetParentCollection();
  void SetParentCollection(Collection* coll);

  bool SetValue(std::string key, std::string value, bool append = false);

protected:

  friend class ItemXMLParser;

  std::string              m_Title;
  std::string              m_Abstract;
  std::string              m_Description;
  std::vector<std::string> m_Authors;
  std::vector<std::string> m_Keywords;
  std::vector<Bitstream *> m_Bitstreams;
  unsigned int             m_BitstreamCount;

  Collection* m_ParentCollection;
};

} // end namespace

#endif // _mdoItem_h_
