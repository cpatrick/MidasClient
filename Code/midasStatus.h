/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef MIDASSTATUS_H
#define MIDASSTATUS_H

#include "midasStandardIncludes.h"

/**
 * This class holds a status entry (a resource waiting to be pushed)
 */
class midasStatus
{
  public:
    midasStatus(std::string uuid, std::string name,
      midasDirtyAction::Action action, midasResourceType::ResourceType type,
      std::string path)
      : Uuid(uuid), Name(name), DirtyAction(action), Type(type), Path(path) {}
    ~midasStatus() {}

    std::string GetName();
    std::string GetUuid();
    std::string GetPath();
    midasDirtyAction::Action GetDirtyAction();
    midasResourceType::ResourceType GetType();
  protected:
    std::string Name;
    std::string Uuid;
    std::string Path;
    midasDirtyAction::Action DirtyAction;
    midasResourceType::ResourceType Type;
};

#endif
