PROJECT(MIDAScli)
cmake_minimum_required(VERSION 2.6)

SET(MIDAScli_SRCS
main.cxx
midasAuthenticator.cxx
midasAuthenticator.h
midasCLI.cxx
midasCLI.h
midasProgressReporter.h
midasDatabaseProxy.cxx
midasDatabaseProxy.h
midasDotProgressReporter.cxx
midasDotProgressReporter.h
midasStandardIncludes.h
midasSynchronizer.cxx
midasSynchronizer.h
midasUtils.cxx
midasUtils.h
)

ADD_EXECUTABLE(MIDAScli ${MIDAScli_SRCS})
TARGET_LINK_LIBRARIES(MIDAScli MIDAS)

SET(midas_db "${CMAKE_BINARY_DIR}/midas.db")

# Add target to build the database from definitions at build time
ADD_CUSTOM_COMMAND (
  OUTPUT ${midas_db}
  DEPENDS "${CMAKE_SOURCE_DIR}/Application/MIDAScli/sql/MIDAS_SQLite.sql"
  COMMAND sqlite3 -init "${CMAKE_SOURCE_DIR}/Application/MIDAScli/sql/MIDAS_SQLite.sql" ${midas_db}
  WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
  COMMENT "Creating SQLite database..."
)
ADD_CUSTOM_TARGET (
  sqlite_db ALL
  DEPENDS sqlite3 ${midas_db}
)
