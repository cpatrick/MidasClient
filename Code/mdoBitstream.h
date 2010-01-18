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

/** This class represent a bitstream on the MIDAS server. */
class Bitstream : public Object
{
public:

  Bitstream();
  ~Bitstream();

  // Fill the item given the id
  bool Fill(unsigned int id, const std::vector<std::string>* fields=NULL);

  // Set/Get the ID of the bitstream
  void SetId(int id) { this->m_Id = id; }
  const int GetId() {return this->m_Id; }

  // Set/Get the name of bitstream
  void SetName(const char* name) { m_Name = name; }
  const std::string GetName() {return m_Name;}
  
  // Set/Get the size of bitstream
  void SetSize(unsigned long size) { m_Size = size; }
  unsigned long GetSize() {return m_Size;}
  
  // Display all the tags and their respective names and values.
  void Print(std::ostream &os, int indent=-1);

protected:

  unsigned int   m_Id;
  std::string    m_Name;
  unsigned long  m_Size;
};

} //end namespace

#endif //_mdoBitstream_h_
