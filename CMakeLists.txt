cmake_minimum_required(VERSION 2.4)
if(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
endif(COMMAND CMAKE_POLICY)

project(MIDASClient)

option(BUILD_TESTING "Build MIDAS Client application testing" OFF)

include(CTest)
configure_file(${MIDASClient_SOURCE_DIR}/CTestCustom.cmake.in
  ${MIDASClient_BINARY_DIR}/CTestCustom.cmake @ONLY)
enable_testing()

# Let user set these options here if they choose to enable testing
if(BUILD_TESTING)
  set(dashboard_midas_server_url http://localhost/midas/api/rest CACHE STRING "The URL of the REST API for MIDAS testing")
  set(dashboard_midas_api_key NO_KEY CACHE STRING "The MIDAS API key for test dashboard")
  set(dashboard_midas_app_name test CACHE STRING "The MIDAS API name for test dashboard")
  set(dashboard_midas_email admin@localhost CACHE STRING "The MIDAS login email for test dashboard")
endif(BUILD_TESTING)

#-----------------------------------------------------------------------------
# Superbuild Option - Enabled by default
#
option(MIDASClient_SUPERBUILD "Build MIDASClient and the projects it depends on via SuperBuild.cmake." ON)
mark_as_advanced(MIDASClient_SUPERBUILD)

if(MIDASClient_SUPERBUILD)
  include("${CMAKE_CURRENT_SOURCE_DIR}/SuperBuild.cmake")
  return()
endif()

add_subdirectory(Testing)
add_subdirectory(Application)

# CPack package specifications
include(${MIDASClient_BINARY_DIR}/../CTK-build/CTK-build/Libs/DataManagement/MIDASClient/Core/CTKDataManagementMIDASClientCoreConfig.cmake)
set(MIDASClient_VERSION_MAJOR ${CTKDataManagementMIDASClientCore_VERSION_MAJOR})
set(MIDASClient_VERSION_MINOR ${CTKDataManagementMIDASClientCore_VERSION_MINOR})
set(MIDASClient_VERSION_PATCH ${CTKDataManagementMIDASClientCore_VERSION_PATCH})
include("${MIDASClient_SOURCE_DIR}/MIDASCPack.cmake")
