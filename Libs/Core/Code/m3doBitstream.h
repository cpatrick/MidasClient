/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

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
