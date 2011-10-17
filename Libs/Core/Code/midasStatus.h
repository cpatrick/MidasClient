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


#ifndef MIDASSTATUS_H
#define MIDASSTATUS_H

#include "midasStandardIncludes.h"

/**
 * This class holds a status entry (a resource waiting to be pushed)
 */
class midasStatus
{
public:
  midasStatus(int id, std::string uuid, std::string name, midasDirtyAction::Action action,
              midasResourceType::ResourceType type,
              std::string path);
  ~midasStatus();

  int GetId();

  std::string GetName();

  std::string GetUuid();

  std::string GetPath();

  midasDirtyAction::Action GetDirtyAction();

  midasResourceType::ResourceType GetType();

protected:
  int                             Id;
  std::string                     Name;
  std::string                     Uuid;
  std::string                     Path;
  midasDirtyAction::Action        DirtyAction;
  midasResourceType::ResourceType Type;
};

#endif
