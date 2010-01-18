/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mdoBitstream.h"
#include <sstream>
#include <iostream>

namespace mdo{

/** Constructor */
Bitstream::Bitstream()
{
  m_Id = 0;
  m_Size = 0;
  m_Name = "";
}
  
/** Destructor */
Bitstream::~Bitstream()
{
}


//--------------------------------------------------------------------------------------------------
void Bitstream::Print(std::ostream &os, int indent)
{
  os << "Id: " << m_Id << "\n";
  os << "Name: " << m_Name.c_str() << "\n";
  os << "Size: " << m_Size << "\n";
}

} // end namespace