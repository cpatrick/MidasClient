project(MIDASRest)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

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
  mdoProxy.h
  mdoProxy.cxx
  mdoProxyInterface.h
  mdoProxyInterface.cxx

  mwsBitstream.h
  mwsBitstream.cxx
  mwsCollection.h
  mwsCollection.cxx
  mwsCommunity.h
  mwsCommunity.cxx
  mwsDicom.h
  mwsDicom.cxx
  mwsItem.h
  mwsItem.cxx
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
  midasProgressReporter.h
  midasStandardIncludes.h
  midasStatus.cxx
  midasStatus.h
  midasSynchronizer.cxx
  midasSynchronizer.h
  midasUtils.cxx
  midasUtils.h
)

SET(midas_db "${CMAKE_BINARY_DIR}/midas.db")

# Add target to build the database from definitions at build time
ADD_CUSTOM_COMMAND (
  OUTPUT ${midas_db}
  DEPENDS "${CMAKE_SOURCE_DIR}/Code/Synchronizer/sql/MIDAS_SQLite.sql"
  COMMAND sqlite3 -init "${CMAKE_SOURCE_DIR}/Code/Synchronizer/sql/MIDAS_SQLite.sql" ${midas_db} "SELECT * FROM resource_uuid;"
  WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
  COMMENT "Creating SQLite database..."
)
ADD_CUSTOM_TARGET (
  sqlite_db ALL
  DEPENDS sqlite3 ${midas_db}
)


#-----------------------------------------------------------------------------
#
add_library(MIDAS ${MIDASRest_SRCS} ${MIDASSynch_SRCS}) 
target_link_libraries(MIDAS cmcurl ITKEXPAT sqlite kwsys)


