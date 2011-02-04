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

  mdoBitstream.h
  mdoBitstream.cxx
  mdoCollection.h
  mdoCollection.cxx
  mdoCommunity.h
  mdoCommunity.cxx
  mdoDicom.h
  mdoDicom.cxx
  mdoItem.h
  mdoItem.cxx
  mdoObject.h
  mdoProxy.h
  mdoProxy.cxx
  mdoProxyInterface.h
  mdoProxyInterface.cxx

  mwsObject.h
  mwsBitstream.h
  mwsBitstream.cxx
  mwsCollection.h
  mwsCollection.cxx
  mwsCommunity.h
  mwsCommunity.cxx
  mwsNewResources.h
  mwsNewResources.cxx
  mwsTreePath.h
  mwsTreePath.cxx
  mwsDicom.h
  mwsDicom.cxx
  mwsItem.h
  mwsItem.cxx
  mwsSearch.h
  mwsSearch.cxx
  mwsRestAPI.h
  mwsRestAPI.cxx
  mwsRestXMLParser.h
  mwsRestXMLParser.cxx
  mwsWebAPI.h
  mwsWebAPI.cxx
  mwsSettings.h
  mwsSettings.cxx
 
  mdsSQLiteDatabase.h
  mdsSQLiteDatabase.cxx
  mdsCommunity.h
  mdsCommunity.cxx
  mdsCollection.h
  mdsCollection.cxx
  mdsItem.h
  mdsItem.cxx
  mdsBitstream.h
  mdsBitstream.cxx
  mdsObject.h
  mdsResourceUpdateHandler.h
)

set(MIDASSynch_SRCS
  midasAgreementHandler.h
  midasAuthenticator.cxx
  midasAuthenticator.h
  midasDatabaseProxy.cxx
  midasDatabaseProxy.h
  midasDotProgressReporter.cxx
  midasDotProgressReporter.h
  midasFileOverwriteHandler.h
  midasLog.h
  midasLogAware.h
  midasProgressReporter.h
  midasStandardIncludes.h
  midasStatus.cxx
  midasStatus.h
  midasStdOutLog.cxx
  midasStdOutLog.h
  midasSynchronizer.cxx
  midasSynchronizer.h
  midasUtils.cxx
  midasUtils.h
)

set(midas_db "${MIDASCPP_BINARY_DIR}/midas.db")

# NOTE: runtime database creation requires a re-configure and re-build to get the latest table defs
message(STATUS "Configuring SQLite table definitions into C++...")
file(WRITE "${MIDASCPP_BINARY_DIR}/Code/midasTableDefs.h" "namespace mds {\nconst char* getTableDefs()\n{\n  return ")
file(STRINGS "${MIDASCPP_SOURCE_DIR}/Code/sql/MIDAS_SQLite.sql" sql_defs)

foreach(sql_def ${sql_defs})
  string(STRIP "${sql_def}" sql_def)
  # Ignore comment lines
  if(NOT sql_def MATCHES "^--")
    string(REPLACE "\"" "\\\"" sql_def "${sql_def}")
    file(APPEND "${MIDASCPP_BINARY_DIR}/Code/midasTableDefs.h" "  \"${sql_def} \"\n")
  endif()
endforeach()

file(APPEND "${MIDASCPP_BINARY_DIR}/Code/midasTableDefs.h" ";\n}\n}\n")
include_directories("${MIDASCPP_BINARY_DIR}/Code")
message(STATUS "Finishing configuring SQLite table definitions")

# Add target to build the database from definitions at build time
add_custom_command (
  OUTPUT ${midas_db}
  DEPENDS "${MIDASCPP_SOURCE_DIR}/Code/sql/MIDAS_SQLite.sql"
  COMMAND sqlite3 -init "${MIDASCPP_SOURCE_DIR}/Code/sql/MIDAS_SQLite.sql" ${midas_db} "SELECT * FROM resource_uuid;"
  WORKING_DIRECTORY "${MIDASCPP_BINARY_DIR}"
  COMMENT "Creating SQLite database..."
)
add_custom_target (
  sqlite_db ALL
  DEPENDS sqlite3 ${midas_db}
)

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
install (FILES ${midas_db} DESTINATION "." COMPONENT Runtime)

#-----------------------------------------------------------------------------
#
add_library(MIDAS ${MIDASRest_SRCS} ${MIDASSynch_SRCS})
add_dependencies(MIDAS reconfigure_tabledefs)
target_link_libraries(MIDAS cmcurl ITKEXPAT sqlite ${MIDASCPP_KWSYS_LIBRARY})
