/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _mwsTreePath_h_
#define _mwsTreePath_h_

#include <string>
#include <vector>

namespace mws {

/** This  */
class TreePath
{
public:
  static std::vector<std::string> PathToRoot(std::string uuid);
  static std::vector<std::string> PathFromRoot(std::string uuid);
private:
  static std::vector<std::string> PathInternal(std::string uuid, std::string url);
};

} //end namespace

#endif //_mwsTreePath_h_
