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


#include "midasStdOutLog.h"
#include <iostream>

midasStdOutLog::midasStdOutLog()
{
}

midasStdOutLog::~midasStdOutLog()
{
}

void midasStdOutLog::Error(std::string text)
{
  std::cerr << text;
}

void midasStdOutLog::Message(std::string text)
{
  std::cout << text;
}

void midasStdOutLog::Status(std::string text)
{
  (void)text;
  // no-op in the CLI view until we get a curses view :)
}

