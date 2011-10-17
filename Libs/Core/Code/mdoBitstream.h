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
