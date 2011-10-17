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


#ifndef _m3doFolder_h_
#define _m3doFolder_h_

#include <string>
#include <vector>

#include "mdoObject.h"

namespace m3do
{

class Item;

/** This class represent a MIDAS 3 community */
class Folder : public mdo::Object
{
public:

  Folder(Folder* other); // copy constructor
  Folder();
  virtual ~Folder();

  // Load the folder
  virtual bool Load();

  virtual bool LoadTree();

  // Set/Get name
  virtual void SetName(const char* name);
  virtual std::string & GetName();

  // Set/Get description
  virtual void SetDescription(const char* description);
  virtual std::string & GetDescription();

  // Get the list of child folders
  virtual std::vector<Folder *> & GetFolders();

  // Get the list of child items
  virtual std::vector<Item *> & GetItems();

  // Set a metadata value by key
  virtual bool SetValue(std::string key, std::string value, bool append = false);

  // Add a folder
  virtual void AddFolder(Folder* folder);

  // Add an item
  virtual void AddItem(Item* item);

  virtual void Clear();

  virtual std::string GetTypeName();
  virtual int GetResourceType();

  virtual void SetBitstreamCount(unsigned int count);
  virtual unsigned int GetBitstreamCount();

  virtual void SetParentFolder(Folder* folder);
  virtual Folder * GetParentFolder();

  virtual void SetPath(const std::string& path);
  virtual std::string & GetPath();

protected:

  std::string  m_Name;
  std::string  m_Path;
  std::string  m_Description;
  unsigned int m_BitstreamCount;

  Folder* m_ParentFolder;

  std::vector<Folder *> m_Folders;
  std::vector<Item *>   m_Items;
};

} // end namespace

#endif
