/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef _m3doItem_h_
#define _m3doItem_h_

#include <string>
#include <vector>

#include "mdoObject.h"

namespace m3do{

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
  void SetName(const char* name) { m_Name = name; }
  std::string& GetName() { return m_Name; }

  // Set/Get description
  void SetDescription(const char* description) { m_Description = description; }
  std::string& GetDescription() { return m_Description; }

  // Get the list of child bitstreams
  std::vector<Bitstream*>& GetBitstreams() { return m_Bitstreams; }
  // Add a bitstream
  void AddBitstream(Bitstream* bitstream);

  // Set a metadata value by key
  bool SetValue(std::string key, std::string value, bool append = false);

  void Clear();

  std::string GetTypeName() { return "Item"; }
  int GetResourceType() { return midas3ResourceType::ITEM; }

  void SetBitstreamCount(unsigned int count) { m_BitstreamCount = count; }
  unsigned int GetBitstreamCount() { return m_BitstreamCount; }

  void SetParentFolder(Folder* folder) { m_ParentFolder = folder; }
  Folder* GetParentFolder() { return m_ParentFolder; }

  void SetPath(const std::string& path) { m_Path = path; }
  std::string& GetPath() { return m_Path; }

protected:

  std::string  m_Name;
  std::string  m_Path;
  std::string  m_Description;
  unsigned int m_BitstreamCount;

  Folder* m_ParentFolder;

  std::vector<Bitstream*> m_Bitstreams;
};

} //end namespace

#endif
