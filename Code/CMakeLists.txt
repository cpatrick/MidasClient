project(MIDASRest)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)


if(NOT CMAKE_BUILD_CURL_SHARED)
  add_definitions(-DCURL_STATICLIB)
endif(NOT CMAKE_BUILD_CURL_SHARED)

# Version info
configure_file(
  "${MIDASCPP_SOURCE_DIR}/Code/MIDASConfig.h.in"
  "${MIDASCPP_BINARY_DIR}/Code/MIDASConfig.h"
  @ONLY
)

# --------------------------------------------------------------------------
# Sources
set(MIDASRest_SRCS
  mdoBitstream.cxx
  mdoCollection.cxx
  mdoCommunity.cxx
  mdoItem.cxx
  mdoVersion.cxx
  mdoProxy.cxx
  mdoProxyInterface.cxx

  mwsBitstream.cxx
  mwsCollection.cxx
  mwsCommunity.cxx
  mwsNewResources.cxx
  mwsTreePath.cxx
  mwsItem.cxx
  mwsSearch.cxx
  mwsRestAPI.cxx
  mwsRestXMLParser.cxx
  mwsWebAPI.cxx

  mdsSQLiteDatabase.cxx
  mdsDatabaseAPI.cxx
  mdsDatabaseInfo.cxx
  mdsCommunity.cxx
  mdsCollection.cxx
  mdsItem.cxx
  mdsBitstream.cxx
  mdsVersion.cxx
  mdsUpgrade.cxx
)

set(MIDASSynch_SRCS
  midasAuthenticator.cxx
  midasDotProgressReporter.cxx
  midasStatus.cxx
  midasStdOutLog.cxx
  midasSynchronizer.cxx
  midasUtils.cxx
)

set(midas_db "${MIDASCPP_BINARY_DIR}/midas.db")

# NOTE: runtime database creation requires a re-configure and re-build to get the latest table defs
message(STATUS "Configuring SQLite table definitions into C++...")
file(WRITE "${MIDASCPP_BINARY_DIR}/Code/mdsTableDefs.h" "namespace mdsUpgrade {\nconst char* getTableDefs()\n{\n  return ")
file(STRINGS "${MIDASCPP_SOURCE_DIR}/Code/sql/MIDAS_SQLite.sql" sql_defs)

foreach(sql_def ${sql_defs})
  string(STRIP "${sql_def}" sql_def)
  # Ignore comment lines
  if(NOT sql_def MATCHES "^--")
    string(REPLACE "\"" "\\\"" sql_def "${sql_def}")
    file(APPEND "${MIDASCPP_BINARY_DIR}/Code/mdsTableDefs.h" "  \"${sql_def} \"\n")
  endif()
endforeach()

file(APPEND "${MIDASCPP_BINARY_DIR}/Code/mdsTableDefs.h"
     "\n  \"INSERT INTO version(name, major, minor, patch) VALUES ('MIDASClient', '${MIDASCPP_VERSION_MAJOR}', '${MIDASCPP_VERSION_MINOR}', '${MIDASCPP_VERSION_PATCH}');\"")
file(APPEND "${MIDASCPP_BINARY_DIR}/Code/mdsTableDefs.h" ";\n}\n}\n")
include_directories("${MIDASCPP_BINARY_DIR}/Code")
message(STATUS "Finishing configuring SQLite table definitions")

# Must reconfigure table def header when we change the SQL definitions
add_custom_command (
  OUTPUT "${MIDASCPP_BINARY_DIR}/Code/midasTableDefs.h"
  DEPENDS "${MIDASCPP_SOURCE_DIR}/Code/sql/MIDAS_SQLite.sql"
  COMMAND "${CMAKE_COMMAND}" "${MIDASCPP_SOURCE_DIR}"
  WORKING_DIRECTORY "${MIDASCPP_BINARY_DIR}"
)
add_custom_target (
  reconfigure_tabledefs ALL
  DEPENDS "${MIDASCPP_BINARY_DIR}/Code/midasTableDefs.h"
)

#-----------------------------------------------------------------------------
#
add_library(MIDAS ${MIDASRest_SRCS} ${MIDASSynch_SRCS})
add_dependencies(MIDAS reconfigure_tabledefs)
target_link_libraries(MIDAS
  cmcurl
  ITKEXPAT
  sqlite
  ${MIDASCPP_KWSYS_LIBRARY}
  ${QT_QTCORE_LIBRARY}
  ${QT_QTGUI_LIBRARY}
  ${QT_QTMAIN_LIBRARY}
)

