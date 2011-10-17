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

  // Set/Get name
  void SetName(const char* name)
  {
    m_Name = name;
  }

  std::string & GetName()
  {
    return m_Name;
  }

  void Clear();

  std::string GetTypeName()
  {
    return "Bitstream";
  }

  int GetResourceType()
  {
    return midas3ResourceType::BITSTREAM;
  }

  void SetParentItem(Item* item)
  {
    m_ParentItem = item;
  }

  Item * GetParentItem()
  {
    return m_ParentItem;
  }

  void SetPath(const std::string& path)
  {
    m_Path = path;
  }

  std::string & GetPath()
  {
    return m_Path;
  }

  void SetChecksum(const std::string& checksum)
  {
    m_Checksum = checksum;
  }

  std::string & GetChecksum()
  {
    return m_Checksum;
  }

  void SetLastModified(unsigned int stamp)
  {
    m_LastModified = stamp;
  }

  unsigned int & GetLastModified()
  {
    return m_LastModified;
  }

  void SetCreationDate(const std::string& date)
  {
    m_CreationDate = date;
  }

  std::string & GetCreationDate()
  {
    return m_CreationDate;
  }

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
