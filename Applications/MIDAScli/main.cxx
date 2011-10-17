/*=========================================================================
  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// MIDAScli is a command line application to interact with MIDAS as a command line

#include "midasCLIApplication.h"

int main(int argc, char **argv)
{
  midasCLIApplication app(argc, argv);
  return app.exec();
}
