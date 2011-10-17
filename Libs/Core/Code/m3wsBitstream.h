/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _m3wsBitstream_h_
#define _m3wsBitstream_h_

#include <string>
#include <vector>

#include "mwsObject.h"

namespace m3do{
  class Bitstream;
}

namespace m3ws{

/** This class represent an Bitstream on a MIDAS 3 server. */
class Bitstream : public mws::Object
{
public:

  Bitstream();
  ~Bitstream();

  bool Fetch();
  bool FetchTree();
  bool FetchParent();
  bool Delete();
  bool Download();
  bool Upload();
  bool Commit();

  void SetObject(mdo::Object* object);
  void ResolveParents();
     
protected:

  m3do::Bitstream* m_Bitstream;
};

} //end namespace

#endif //_m3wsFolder_h_
