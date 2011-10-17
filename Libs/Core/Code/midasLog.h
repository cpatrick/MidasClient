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


#ifndef MIDASLOG_H
#define MIDASLOG_H

#include "midasStandardIncludes.h"

/**
 * Abstract class for logging messages
 */
class midasLog
{
public:
  midasLog()
  {
  }
  virtual ~midasLog()
  {
  }

  virtual void Error(std::string text) = 0;

  virtual void Message(std::string text) = 0;

  virtual void Status(std::string text) = 0;

};

#endif
