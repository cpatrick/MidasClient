/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "midasStatus.h"

std::string midasStatus::GetName()
{
  return this->Name;
}

std::string midasStatus::GetUUID()
{
  return this->UUID;
}
    
midasDirtyAction::Action midasStatus::GetDirtyAction()
{
  return this->DirtyAction;
}

midasResourceType::ResourceType midasStatus::GetType()
{
  return this->Type;
}