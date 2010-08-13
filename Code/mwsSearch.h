/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _mwsSearch_h_
#define _mwsSearch_h_

#include <string>
#include <vector>

namespace mdo {
  class Object;
}

namespace mws {

/** This class can be used to search */
class Search
{
public:
  static std::vector<mdo::Object*> SearchServer(std::vector<std::string> tokens);
};

} //end namespace

#endif //_mwsSearch_h_
