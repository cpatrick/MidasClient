cmake_minimum_required(VERSION 2.4)
if(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
endif(COMMAND CMAKE_POLICY)

project(MIDASClient)

set(MIDASClient_VERSION_MAJOR 3)
set(MIDASClient_VERSION_MINOR 0)
set(MIDASClient_VERSION_PATCH 1)

option(BUILD_TESTING "Build MIDAS Client application testing" OFF)

include(CTest)
enable_testing()
configure_file(${MIDASClient_SOURCE_DIR}/CTestCustom.cmake.in
  ${MIDASClient_BINARY_DIR}/CTestCustom.cmake @ONLY)

# Let user set these options here if they choose to enable testing
if(BUILD_TESTING)
  set(dashboard_midas_server_url http://localhost/midas/api/rest CACHE STRING "The URL of the REST API for MIDAS testing")
  set(dashboard_midas_api_key NO_KEY CACHE STRING "The MIDAS API key for test dashboard")
  set(dashboard_midas_app_name test CACHE STRING "The MIDAS API name for test dashboard")
  set(dashboard_midas_email admin@localhost CACHE STRING "The MIDAS login email for test dashboard")
  site_name(dashboard_top_level_community_name)
endif(BUILD_TESTING)

set(minimum_required_qt_version "4.6")
find_package(Qt4)
mark_as_advanced(QT_QMAKE_EXECUTABLE)
if(QT4_FOUND)
  if("${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}" VERSION_LESS "${minimum_required_qt_version}")
    message(FATAL_ERROR "error: CTK requires Qt >= ${minimum_required_qt_version} -- you cannot use Qt ${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}.")
  endif()

  SET(QT_USE_QTNETWORK ON)
  SET(QT_USE_QTSCRIPT ON)
  include(${QT_USE_FILE})

else(QT4_FOUND)
  message(FATAL_ERROR "error: Qt4 was not found on your system. You probably need to set the QT_QMAKE_EXECUTABLE variable")
endif(QT4_FOUND)

add_subdirectory(Testing)
add_subdirectory(Libs)
#add_subdirectory(Applications)

# CPack package specifications
include("${MIDASClient_SOURCE_DIR}/MIDASCPack.cmake")
