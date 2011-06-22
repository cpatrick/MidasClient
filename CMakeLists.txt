cmake_minimum_required(VERSION 2.4)
if(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
endif(COMMAND CMAKE_POLICY)

project(MIDASClient)

include(CTest)
configure_file(${MIDASClient_SOURCE_DIR}/CTestCustom.cmake.in
  ${MIDASClient_BINARY_DIR}/CTestCustom.cmake @ONLY)
enable_testing()

#-----------------------------------------------------------------------------
# Superbuild Option - Enabled by default
#
OPTION(MIDASClient_SUPERBUILD "Build CTK and the projects it depends on via SuperBuild.cmake." ON)
MARK_AS_ADVANCED(MIDASClient_SUPERBUILD)

if(MIDASClient_SUPERBUILD)
  include("${CMAKE_CURRENT_SOURCE_DIR}/SuperBuild.cmake")
  return()
endif()

add_subdirectory(Application)
add_subdirectory(Testing)

include("${MIDASClient_SOURCE_DIR}/MIDASCPack.cmake")
