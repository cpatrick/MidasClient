
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
    -DGITCOMMAND:FILEPATH=${GIT_EXECUTABLE}
  DEPENDS
  )

ExternalProject_Add(MIDASClient-configure
  DOWNLOAD_COMMAND ""
  CMAKE_GENERATOR "${CMAKE_GENERATOR}"
  CMAKE_ARGS
    -DMIDASClient_SUPERBUILD:BOOL=OFF
    -DBUILD_TESTING:BOOL=${BUILD_TESTING}
    -Ddashboard_midas_server_url=${dashboard_midas_server_url}
    -Ddashboard_midas_api_key=${dashboard_midas_api_key}
    -Ddashboard_midas_app_name=${dashboard_midas_app_name}
    -Ddashboard_midas_email=${dashboard_midas_email}
  SOURCE_DIR ${MIDASClient_SOURCE_DIR}
  BINARY_DIR ${MIDASClient_BINARY_DIR}/MIDASClient-build
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  DEPENDS
    CTK
  )

ExternalProject_Add(MIDASClient-build
    DOWNLOAD_COMMAND ""
    CMAKE_GENERATOR "${CMAKE_GENERATOR}"
    SOURCE_DIR ${MIDASClient_SOURCE_DIR}
    BINARY_DIR MIDASClient-build
    INSTALL_COMMAND ""
    DEPENDS
      "MIDASClient-configure"
    )

#-----------------------------------------------------------------------------
# Custom target allowing to drive the build of MIDASClient project itself
#
ADD_CUSTOM_TARGET(MIDASClient
  COMMAND ${CMAKE_COMMAND} --build ${MIDASClient_BINARY_DIR}/MIDASClient-build
  WORKING_DIRECTORY ${MIDASClient_BINARY_DIR}/MIDASClient-build
  )
