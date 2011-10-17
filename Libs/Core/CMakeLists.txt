set(MIDASClient_VERSION_STRING "${MIDASClient_VERSION_MAJOR}.${MIDASClient_VERSION_MINOR}.${MIDASClient_VERSION_PATCH}")

# Version info
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/Code/MIDASConfig.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/Code/MIDASConfig.h"
  @ONLY
)

# --------------------------------------------------------------------------
# Sources
set(MIDASClientCore_SRCS
  Code/mdoAssetstore.cxx
  Code/mdoBitstream.cxx
  Code/mdoCollection.cxx
  Code/mdoCommunity.cxx
  Code/mdoItem.cxx
  Code/mdoObject.cxx
  Code/mdoVersion.cxx
  Code/mdoProxy.cxx
  Code/mdoProxyInterface.cxx

  Code/mwsBitstream.cxx
  Code/mwsCollection.cxx
  Code/mwsCommunity.cxx
  Code/mwsNewResources.cxx
  Code/mwsTreePath.cxx
  Code/mwsItem.cxx
  Code/mwsObject.cxx
  Code/mwsSearch.cxx
  Code/mwsRestAPI.cxx
  Code/mwsRestResponseParser.cxx
  Code/mwsWebAPI.cxx

  Code/mdsSQLiteDatabase.cxx
  Code/mdsDatabaseAPI.cxx
  Code/mdsDatabaseInfo.cxx
  Code/mdsCommunity.cxx
  Code/mdsCollection.cxx
  Code/mdsItem.cxx
  Code/mdsObject.cxx
  Code/mdsBitstream.cxx
  Code/mdsVersion.cxx
  Code/mdsUpgrade.cxx
  Code/mdsPartialDownload.cxx
  Code/mdsPartialUpload.cxx
  Code/mdsSQLiteDatabase.cxx

  Code/midasAuthenticator.cxx
  Code/midasDotProgressReporter.cxx
  Code/midasStatus.cxx
  Code/midasStdOutLog.cxx
  Code/midasSynchronizer.cxx
  Code/midasUtils.cxx
  Code/midasLogAware.cxx
  Code/midasProgressReporter.cxx

  Code/m3doCommunity.cxx
  Code/m3doFolder.cxx
  Code/m3doItem.cxx
  Code/m3doBitstream.cxx

  Code/m3wsFolder.cxx
  Code/m3wsItem.cxx
  Code/m3wsBitstream.cxx

  Code/m3dsFolder.cxx
  Code/m3dsItem.cxx
  Code/m3dsBitstream.cxx
)

set(MIDASClientCore_MOC_SRCS
  Code/mwsRestAPI.h
)
QT4_WRAP_CPP(MIDASClientCore_SRCS ${MIDASClientCore_MOC_SRCS})

set(midas_db "${CMAKE_CURRENT_BINARY_DIR}/midas.db")

# Configure in the source and binary directory variables so we can call using scripting mode
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/CMake/MIDASClientDatabaseConfigure.cmake.in"
               "${CMAKE_CURRENT_BINARY_DIR}/MIDASClientDatabaseConfigure.cmake"
               @ONLY)
# Configure in the source and binary directory variables so we can call using scripting mode
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/CMake/MIDAS3ClientDatabaseConfigure.cmake.in"
               "${CMAKE_CURRENT_BINARY_DIR}/MIDAS3ClientDatabaseConfigure.cmake"
               @ONLY)

if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/Code/mdsTableDefs.h")
  include("${CMAKE_CURRENT_BINARY_DIR}/MIDASClientDatabaseConfigure.cmake")
endif()

if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/Code/m3dsTableDefs.h")
  include("${CMAKE_CURRENT_BINARY_DIR}/MIDAS3ClientDatabaseConfigure.cmake")
endif()

include_directories(
  "${CMAKE_CURRENT_SOURCE_DIR}/Code"
  "${CMAKE_CURRENT_BINARY_DIR}/Code"
  "${MIDASClient_SOURCE_DIR}/Libs/External/sqlite"
)

# Must reconfigure table def header when we change the SQL definitions
add_custom_command(
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/Code/mdsTableDefs.h"
  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Code/sql/MIDAS_SQLite.sql"
  COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/MIDASClientDatabaseConfigure.cmake"
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)
add_custom_target(
  reconfigure_tabledefs ALL
  DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/Code/mdsTableDefs.h"
)

# Must reconfigure table def header when we change the SQL definitions
add_custom_command(
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/Code/m3dsTableDefs.h"
  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Code/sql/MIDAS3_SQLite.sql"
  COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/MIDAS3ClientDatabaseConfigure.cmake"
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)
add_custom_target(
  reconfigure_tabledefs3 ALL
  DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/Code/m3dsTableDefs.h"
)

add_library(MIDASClientCore ${MIDASClientCore_SRCS})

target_link_libraries(MIDASClientCore
  sqlite
  ${QT_QTCORE_LIBRARY}
  ${QT_QTGUI_LIBRARY}
  ${QT_QTSCRIPT_LIBRARY}
  ${QT_QTNETWORK_LIBRARY}
)
add_dependencies(MIDASClientCore reconfigure_tabledefs)

include(GenerateConfig.cmake)
