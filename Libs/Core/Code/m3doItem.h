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


#ifndef _m3doItem_h_
#define _m3doItem_h_

#include <string>
#include <vector>

#include "mdoObject.h"

namespace m3do
{

class Folder;
class Bitstream;

/** This class represent a MIDAS 3 item */
class Item : public mdo::Object
{
public:

  Item();
  Item(Item* other);
  ~Item();

  bool Load();

  bool LoadTree();

  // Set/Get name
  void SetName(const char* name)
  {
    m_Name = name;
  }
  std::string & GetName()
  {
    return m_Name;
  }

  // Set/Get description
  void SetDescription(const char* description)
  {
    m_Description = description;
  }
  std::string & GetDescription()
  {
    return m_Description;
  }

  // Get the list of child bitstreams
  std::vector<Bitstream *> & GetBitstreams()
  {
    return m_Bitstreams;
  }
  // Add a bitstream
  void AddBitstream(Bitstream* bitstream);

  // Set a metadata value by key
  bool SetValue(std::string key, std::string value, bool append = false);

  void Clear();

  std::string GetTypeName()
  {
    return "Item";
  }
  int GetResourceType()
  {
    return midas3ResourceType::ITEM;
  }

  void SetBitstreamCount(unsigned int count)
  {
    m_BitstreamCount = count;
  }
  unsigned int GetBitstreamCount()
  {
    return m_BitstreamCount;
  }

  void SetParentFolder(Folder* folder)
  {
    m_ParentFolder = folder;
  }
  Folder * GetParentFolder()
  {
    return m_ParentFolder;
  }

  void SetPath(const std::string& path)
  {
    m_Path = path;
  }
  std::string & GetPath()
  {
    return m_Path;
  }
protected:

  std::string  m_Name;
  std::string  m_Path;
  std::string  m_Description;
  unsigned int m_BitstreamCount;

  Folder* m_ParentFolder;

  std::vector<Bitstream *> m_Bitstreams;
};

} // end namespace

#endif
