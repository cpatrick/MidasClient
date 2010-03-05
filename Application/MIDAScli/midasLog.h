/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef MIDASLOG_H
#define MIDASLOG_H

#include "midasStandardIncludes.h"

/**
 * Class for logging messages
 */
class midasLog
{
  public:
    midasLog(std::ostream* out, std::ostream* err) : Out(out), Err(err) {}
    //Default constructor simply uses cout and cerr
    midasLog() : Out(&std::cout), Err(&std::cerr) {}
    ~midasLog() {}

    std::ostream& Error() { return *(this->Err); }
    std::ostream& Message() { return *(this->Out); }
  private:
    std::ostream* Out;
    std::ostream* Err;
};

#endif