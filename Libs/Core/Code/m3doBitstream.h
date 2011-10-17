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


#ifndef _m3doBitstream_h_
#define _m3doBitstream_h_

#include <string>
#include <vector>

#include "mdoObject.h"

namespace m3do
{

class Item;

/** This class represent a MIDAS 3 bitstream */
class Bitstream : public mdo::Object
{
public:

  Bitstream();
  Bitstream(Bitstream* other);
  ~Bitstream();

  bool Load();

  bool LoadTree();

  /// Set/Get name
  void SetName(const char* name);
  std::string & GetName();

  void Clear();

  std::string GetTypeName();
  int GetResourceType();

  void SetParentItem(Item* item);
  Item * GetParentItem();

  void SetPath(const std::string& path);
  std::string & GetPath();

  void SetChecksum(const std::string& checksum);
  std::string & GetChecksum();

  void SetLastModified(unsigned int stamp);
  unsigned int & GetLastModified();

  void SetCreationDate(const std::string& date);
  std::string & GetCreationDate();

protected:

  std::string  m_Name;
  std::string  m_Path;
  std::string  m_Checksum;
  std::string  m_CreationDate;
  unsigned int m_LastModified;
  unsigned int m_BitstreamCount;

  Item* m_ParentItem;
};

} // end namespace

#endif
