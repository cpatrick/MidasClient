/*=========================================================================
  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "MidasApplication.h"
#include "MidasClientGlobal.h"

#include <iostream>
#include <QDir>
#include "mwsSettings.h"
#include "DionysusUI.h"
#include "Utils.h"
#include "mwsRestAPI.h"
#include "Logger.h"

int main(int argc, char **argv)
{
  MidasApplication app(argc, argv);
  std::cout << MIDAS_CLIENT_VERSION_STR << std::endl;
  return app.exec();
}
