project(FetchMIDAS)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# --------------------------------------------------------------------------
# Find Slicer3

if(NOT Slicer3_SOURCE_DIR)
  find_package(Slicer3 REQUIRED)
  include(${Slicer3_USE_FILE})
  slicer3_set_default_install_prefix_for_external_projects()
endif(NOT Slicer3_SOURCE_DIR)

#---------------------------------------------------------------------------
# Include subdirectories
add_subdirectory( MIDAScpp )

# --------------------------------------------------------------------------
# Include dirs

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${Slicer3_Libs_INCLUDE_DIRS}
  ${Slicer3_Base_INCLUDE_DIRS}
  ${Volumes_SOURCE_DIR}
  ${Volumes_BINARY_DIR}
  )

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/vtkFetchMIDASConfigure.h.in 
  ${CMAKE_CURRENT_BINARY_DIR}/vtkFetchMIDASConfigure.h
  )

file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
install(FILES 
  ${headers} 
  "${CMAKE_CURRENT_BINARY_DIR}/vtkFetchMIDASConfigure.h"
  DESTINATION ${Slicer3_INSTALL_MODULES_INCLUDE_DIR}/${PROJECT_NAME} COMPONENT Development
  )

# --------------------------------------------------------------------------
# Sources
 
set(FetchMIDAS_SRCS
  vtkFetchMIDASLogic.cxx
  vtkFetchMIDASGUI.cxx
  vtkMRMLFetchMIDASNode.cxx
  vtkFetchMIDASIcons.cxx
  vtkFetchMIDASMulticolumnWidget.cxx
  vtkFetchMIDASQueryTermWidget.cxx
  vtkFetchMIDASFlatResourceWidget.cxx
  vtkFetchMIDASResourceUploadWidget.cxx
  vtkFetchMIDASTagViewWidget.cxx
  vtkFetchMIDASParser.cxx
  vtkFetchMIDASParserHID.cxx
  vtkFetchMIDASParserXND.cxx
  vtkFetchMIDASWriter.cxx
  vtkFetchMIDASWriterXND.cxx
  vtkFetchMIDASWebServicesClient.cxx
  vtkFetchMIDASWebServicesClientMIDAS.cxx
  vtkFetchMIDASServer.cxx
  vtkFetchMIDASServerCollection.cxx
  vtkFetchMIDASHandler.cxx
  )


# --------------------------------------------------------------------------
# Wrapping

include("${VTK_CMAKE_DIR}/vtkWrapTcl.cmake")
vtk_wrap_tcl3(FetchMIDAS FetchMIDAS_TCL_SRCS "${FetchMIDAS_SRCS}" "")

# --------------------------------------------------------------------------
# Add Loadable Module support

generatelm(FetchMIDAS_SRCS FetchMIDAS.txt)

# --------------------------------------------------------------------------
# Build and install the library

set(lib_name FetchMIDAS)
add_library(${lib_name}
  ${FetchMIDAS_SRCS}
  ${FetchMIDAS_TCL_SRCS}
  )
slicer3_set_modules_output_path(${lib_name})

target_link_libraries(${lib_name}
  ${Slicer3_Libs_LIBRARIES}
  ${Slicer3_Base_LIBRARIES}
  ${KWWidgets_LIBRARIES}
  ${ITK_LIBRARIES}
  Volumes
  MIDAScpp_lib
  )

slicer3_install_modules(${lib_name})

# --------------------------------------------------------------------------
# Testing

if(BUILD_TESTING)
#  subdirs(Testing)
endif(BUILD_TESTING)

# --------------------------------------------------------------------------
# Install support files

# Should those paths change, make sure to update 
# - Tcl/FetchMIDASXMLUtilities.tcl

# Tcl files

file(GLOB TCLFILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Tcl/*.tcl")

foreach (file ${TCLFILES})
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/${file}
    ${CMAKE_BINARY_DIR}/${Slicer3_INSTALL_MODULES_LIB_DIR}/${PROJECT_NAME}/${file}
    COPYONLY)
endforeach(file)

install(
  FILES ${TCLFILES}
  DESTINATION ${Slicer3_INSTALL_MODULES_LIB_DIR}/${PROJECT_NAME}/Tcl
  )
