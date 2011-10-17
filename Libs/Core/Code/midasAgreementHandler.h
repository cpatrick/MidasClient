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


#ifndef MIDASAGREEMENTHANDLER_H
#define MIDASAGREEMENTHANDLER_H

#include "midasStandardIncludes.h"

class midasSynchronizer;

/**
 * Abstract class for handling a license agreement on a resource
 */
class midasAgreementHandler
{
public:
  midasAgreementHandler()
  {
  }
  virtual ~midasAgreementHandler()
  {
  }
  virtual bool HandleAgreement(midasSynchronizer* synch) = 0;

};

#endif
