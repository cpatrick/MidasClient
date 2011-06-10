cmake_minimum_required(VERSION 2.4)
if(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
endif(COMMAND CMAKE_POLICY)

project(MIDASClient)

include(CTest)
configure_file(${MIDASClient_SOURCE_DIR}/CTestCustom.cmake.in
  ${MIDASClient_BINARY_DIR}/CTestCustom.cmake @ONLY)
enable_testing()

include(ExternalProject)

ExternalProject_Add(CTK
      SOURCE_DIR "${MIDASClient_BINARY_DIR}/CTK"
      BINARY_DIR CTK-build
      PREFIX CTK-cmake
      GIT_REPOSITORY "git://github.com/zachmullen/CTK.git"
      GIT_TAG "origin/add-midascpp-library"
      CMAKE_GENERATOR "${CMAKE_GENERATOR}"
      INSTALL_COMMAND ""
      CMAKE_ARGS
        -DCTK_LIB_DataManagement/MIDASClient/Core:BOOL=ON
        -DCTK_LIB_DataManagement/MIDASClient/Widgets:BOOL=ON
        -DCTK_LIB_PluginFramework:BOOL=OFF
        -DGIT_EXECUTABLE:FILEPATH=${GIT_EXECUTABLE}
      DEPENDS
      )

#INCLUDE(${CMAKE_ROOT}/Modules/CMakeBackwardCompatibilityC.cmake)
#INCLUDE(${CMAKE_ROOT}/Modules/CMakeBackwardCompatibilityCXX.cmake)

add_subdirectory(Application)
add_subdirectory(Testing)

include("${MIDASClient_SOURCE_DIR}/MIDASCPack.cmake")
