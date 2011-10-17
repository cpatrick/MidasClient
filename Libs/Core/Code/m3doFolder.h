/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef _m3doFolder_h_
#define _m3doFolder_h_

#include <string>
#include <vector>

#include "mdoObject.h"

namespace m3do{

class Item;

/** This class represent a MIDAS 3 community */
class Folder : public mdo::Object
{
public:

  Folder(Folder* other); //copy constructor
  Folder();
  virtual ~Folder();

  // Load the folder
  virtual bool Load();
  virtual bool LoadTree();

  // Set/Get name
  virtual void SetName(const char* name) { m_Name = name; }
  virtual std::string & GetName() { return m_Name; }
  
  // Set/Get description
  virtual void SetDescription(const char* description) { m_Description = description; }
  virtual std::string & GetDescription() { return m_Description; }
  
  // Get the list of child folders
  virtual std::vector<Folder*>& GetFolders() { return m_Folders; }

  // Get the list of child items
  virtual std::vector<Item*>& GetItems() { return m_Items; }

  // Set a metadata value by key
  virtual bool SetValue(std::string key, std::string value, bool append = false);
   
  // Add a folder
  virtual void AddFolder(Folder* folder);

  // Add an item
  virtual void AddItem(Item* item);
  
  virtual void Clear();

  virtual std::string GetTypeName() { return "Folder"; }
  virtual int GetResourceType() { return midas3ResourceType::FOLDER; }

  virtual void SetBitstreamCount(unsigned int count) { m_BitstreamCount = count; }
  virtual unsigned int GetBitstreamCount() { return m_BitstreamCount; }

  virtual void SetParentFolder(Folder* folder) { m_ParentFolder = folder; }
  virtual Folder* GetParentFolder() { return m_ParentFolder; }

  virtual void SetPath(const std::string& path) { m_Path = path; }
  virtual std::string& GetPath() { return m_Path; }
  
protected:
  
  std::string  m_Name;
  std::string  m_Path;
  std::string  m_Description;
  unsigned int m_BitstreamCount;

  Folder* m_ParentFolder;
  
  std::vector<Folder*> m_Folders;
  std::vector<Item*> m_Items;
};

} //end namespace

#endif
