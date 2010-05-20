/*=========================================================================

  Program:   Dionysus
  Module:    KWDionysus.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkKWDionysusWindow.h" // This line has to be first include!
#include "vtkKWApplication.h"

#include <vtksys/SystemTools.hxx>
#include <vtksys/CommandLineArguments.hxx>
#include "metaCommand.h"
#include "kwCrypt.h"

// We define several classes in this example, and we want to be able to use
// their C++ methods as callbacks for our user interface. To do so, we created
// a library and wrapped it automatically for the Tcl language, which is used
// as a bridge between C++ objects at run-time. An initialization function is
// automatically created in this library to allow classes and C++ methods to
// be used as commands and callbacks inside the Tcl interpreter; let's *not* 
// forget to declare *and* call this function right after we initialize the Tcl
// interpreter in our application. The name of this function is built from the
// library name in lower-case (except for the first letter) and suffixed with
// "_Init" (for example: KWCallbacksExampleLib => Kwcallbacksexamplelib_Init).
// This whole process is required to use C++ methods as callbacks; it is not
// needed if you use VTK's C++ command/observer pattern directly, which is
// demonstrated in KWWidgets's C++ 'Callbacks' example.

extern "C" int Kwdionysuslib_Init(Tcl_Interp *interp);

int my_main(int argc, char *argv[])
{
  if(argc>1)
    {
    MetaCommand command;
    command.SetOption("crypt","c",false,
      "crypt a given word (used to crypt database password)");
    command.AddOptionField("crypt","word",MetaCommand::STRING,false);
 
    if(!command.Parse(argc,argv))
      {
      return 1;
      }

    if(command.GetOptionWasSet("crypt"))
      {
      std::string wordtocrypt = command.GetValueAsString("crypt","word");
      kwCrypt crypt;
      std::cout << crypt.Crypt(wordtocrypt.c_str()) << std::endl;
      }

    return 0;
    }

  // Initialize Tcl
  Tcl_Interp *interp = vtkKWApplication::InitializeTcl(argc, argv, &cerr);
  if (!interp)
    {
    cerr << "Error: InitializeTcl failed" << endl;
    return 1;
    }

  Kwdionysuslib_Init(interp);

  vtkKWApplication *app = vtkKWApplication::New();
  app->SetName("Dionysus");
  vtkKWDionysusWindow *project = vtkKWDionysusWindow::New();
  project->SetApplication(app);

  int res = project->Run(argc, argv);
  project->Delete();
 
  app->Delete();

  Tcl_DeleteInterp(interp);

  return res;
}

#if defined(_WIN32) && !defined(__CYGWIN__)
#include <windows.h>

#ifdef _MSC_VER
#include <crtdbg.h>
#endif

int __stdcall WinMain(HINSTANCE, HINSTANCE, LPSTR lpCmdLine, int)
{
#ifdef _MSC_VER
  // See if there are any mem leaks at exit time on Windows:
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF);
#endif  //  _MSC_VER
  int argc;
  char **argv;
  vtksys::SystemTools::ConvertWindowsCommandLineToUnixArguments(
    lpCmdLine, &argc, &argv);

  int ret = my_main(argc, argv);
  for (int i = 0; i < argc; i++) { delete [] argv[i]; }
  delete [] argv;
  return ret;
}
#else
int main(int argc, char *argv[])
{
  return my_main(argc, argv);
}
#endif
