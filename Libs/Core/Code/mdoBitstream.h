/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef _mdoBitstream_h_
#define _mdoBitstream_h_

#include "mdoObject.h"
#include <vector>

namespace mdo{

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
  void SetName(const char* name) { m_Name = name; }
  std::string & GetName() {return m_Name;}

  void SetLastModified(unsigned int time) { m_LastModified = time; }
  long int GetLastModified() { return m_LastModified; }

  void SetPath(std::string path) { m_Path = path; }
  std::string GetPath() { return m_Path; }

  std::string GetTypeName() { return "Bitstream"; }
  int GetResourceType() { return midasResourceType::BITSTREAM; }

  void AddLocation(Assetstore* location) { m_Locations.push_back(location); }
  void SetLocations(std::vector<Assetstore*> locations) { m_Locations = locations; }
  std::vector<Assetstore*> & GetLocations() { return m_Locations; }

  Item* GetParentItem() { return m_ParentItem; }
  void SetParentItem(Item* item) { m_ParentItem = item; }

protected:

  std::string              m_Name;
  std::string              m_Path;
  std::vector<Assetstore*> m_Locations;
  Item*                    m_ParentItem;
  unsigned int             m_LastModified;
};

} //end namespace

#endif //_mdoBitstream_h_