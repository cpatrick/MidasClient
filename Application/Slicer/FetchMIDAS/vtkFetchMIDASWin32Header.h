#ifndef __vtkFetchMIDASWin32Header_h
#define __vtkFetchMIDASWin32Header_h

#include <vtkFetchMIDASConfigure.h>

#if defined(WIN32) && !defined(VTKSLICER_STATIC)
#if defined(FetchMIDAS_EXPORTS)
#define VTK_FETCHMIDAS_EXPORT __declspec( dllexport )
#else
#define VTK_FETCHMIDAS_EXPORT __declspec( dllimport )
#endif
#else
#define VTK_FETCHMIDAS_EXPORT
#endif

#endif
