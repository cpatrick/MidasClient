/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "midasStatus.h"

midasStatus::midasStatus(int id, std::string uuid, std::string name, midasDirtyAction::Action action,
                         midasResourceType::ResourceType type,
                         std::string path)
  : Id(id), Name(name), Uuid(uuid), Path(path), DirtyAction(action), Type(type)
{
}

midasStatus::~midasStatus()
{
}

int midasStatus::GetId()
{
  return this->Id;
}

std::string midasStatus::GetName()
{
  return this->Name;
}

std::string midasStatus::GetUuid()
{
  return this->Uuid;
}

midasDirtyAction::Action midasStatus::GetDirtyAction()
{
  return this->DirtyAction;
}

midasResourceType::ResourceType midasStatus::GetType()
{
  return this->Type;
}

std::string midasStatus::GetPath()
{
  return this->Path;
}

