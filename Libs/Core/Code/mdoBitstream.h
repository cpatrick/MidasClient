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


#ifndef _mdoBitstream_h_
#define _mdoBitstream_h_

#include "mdoObject.h"
#include <vector>

namespace mdo
{

class Item;
class Assetstore;

/** This class represent a bitstream on the MIDAS server. */
class Bitstream : public Object
{
public:

  Bitstream();
  ~Bitstream();

  void Clear();

  // Set/Get the name of bitstream
  void SetName(const char* name);
  std::string & GetName();

  void SetLastModified(unsigned int time);
  long int GetLastModified();

  void SetPath(std::string path);
  std::string GetPath();

  std::string GetTypeName();
  int GetResourceType();

  void AddLocation(Assetstore* location);
  void SetLocations(std::vector<Assetstore *> locations);
  std::vector<Assetstore *> & GetLocations();

  Item * GetParentItem();
  void SetParentItem(Item* item);
protected:

  std::string               m_Name;
  std::string               m_Path;
  std::vector<Assetstore *> m_Locations;
  Item*                     m_ParentItem;
  unsigned int              m_LastModified;
};

} // end namespace

#endif // _mdoBitstream_h_
