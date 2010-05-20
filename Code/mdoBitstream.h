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

/** This class represent a bitstream on the MIDAS server. */
class Bitstream : public Object
{
public:

  Bitstream();
  ~Bitstream();

  // Fill the item given the id
  bool Fill(unsigned int id, const std::vector<std::string>* fields=NULL);
  void Clear();

  // Set/Get the ID of the parent
  void SetParent(std::string id) { m_Parent = id; }
  std::string & GetParent() {return m_Parent;}
  const int GetParentId() {return atoi(m_Parent.c_str());}

  // Set/Get the name of bitstream
  void SetName(const char* name) { m_Name = name; }
  std::string & GetName() {return m_Name;}
  
  // Set/Get the size of bitstream
  void SetSize(std::string size) { m_Size = size; }
  std::string & GetSize() {return m_Size;}
  
  // Display all the tags and their respective names and values.
  void Print(std::ostream &os, int indent=-1);

  std::string GetTypeName() { return "Bitstream"; }

  Item* GetParentItem() { return m_ParentItem; }
  void SetParentItem(Item* item) { m_ParentItem = item; }

protected:

  std::string  m_Parent;
  std::string  m_Name;
  std::string  m_Size;
  Item*        m_ParentItem;
};

} //end namespace

#endif //_mdoBitstream_h_
