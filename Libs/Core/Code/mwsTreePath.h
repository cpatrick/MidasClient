/******************************************************************************
 * Copyright 2011 Kitware Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#ifndef _mwsTreePath_h_
#define _mwsTreePath_h_

#include <string>
#include <vector>

namespace mws
{

/** This  */
class TreePath
{
public:
  static std::vector<std::string> PathToRoot(std::string uuid);

  static std::vector<std::string> PathFromRoot(std::string uuid);

private:
  static std::vector<std::string> PathInternal(std::string uuid, std::string url);

};

} // end namespace

#endif // _mwsTreePath_h_
