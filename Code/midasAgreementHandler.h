/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

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
  midasAgreementHandler() {}
  virtual ~midasAgreementHandler() {}
  virtual bool HandleAgreement(midasSynchronizer* synch) = 0;

};

#endif
