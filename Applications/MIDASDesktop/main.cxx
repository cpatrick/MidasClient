/*=========================================================================
  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "MidasApplication.h"
#include "MIDASConfig.h"

#include <iostream>

int main(int argc, char **argv)
{
  MidasApplication app(argc, argv);
  std::cout << "MIDAS Desktop " << MIDAS_CLIENT_VERSION << std::endl;
  return app.exec();
}
