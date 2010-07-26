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

include_directories(
  "${MIDASCPP_BINARY_DIR}/Code"
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

  metaCommand.h
  metaCommand.cxx
 
  mdsSQLiteDatabase.h
  mdsSQLiteDatabase.cxx
  mdsItem.h
  mdsItem.cxx
  mdsBitstream.h
  mdsBitstream.cxx
  mdsObject.h
)

set(MIDASSynch_SRCS
  midasAuthenticator.cxx
  midasAuthenticator.h
  midasDatabaseProxy.cxx
  midasDatabaseProxy.h
  midasDotProgressReporter.cxx
  midasDotProgressReporter.h
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
message(STATUS "Configuring SQLite table definitions into source...")
file(WRITE "${MIDASCPP_BINARY_DIR}/Code/midasTableDefs.h" "const char* getTableDefs()\n{\n  return ")
file(STRINGS "${MIDASCPP_SOURCE_DIR}/Code/sql/MIDAS_SQLite.sql" sql_defs)

foreach(sql_def ${sql_defs})
  # Ignore comment lines
  if(NOT sql_def MATCHES "^[ \t]*--")
    # Escape quotes
    string(REPLACE "\"" "\\\"" sql_def_escaped "${sql_def}")
    file(APPEND "${MIDASCPP_BINARY_DIR}/Code/midasTableDefs.h" "  \"${sql_def_escaped}\"\n")
  endif()
endforeach()

file(APPEND "${MIDASCPP_BINARY_DIR}/Code/midasTableDefs.h" ";\n}\n")
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

install (FILES ${midas_db} DESTINATION ".")

#-----------------------------------------------------------------------------
#
add_library(MIDAS ${MIDASRest_SRCS} ${MIDASSynch_SRCS}) 
target_link_libraries(MIDAS cmcurl ITKEXPAT sqlite kwsys)
