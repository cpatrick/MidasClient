PROJECT(MIDAScli)
cmake_minimum_required(VERSION 2.6)

SET(MIDAScli_SRCS
main.cxx
midasCLI.h
midasCLI.cxx
midasDotProgressReporter.h
midasDotProgressReporter.cxx
midasProgressReporter.h
midasStandardIncludes.h
midasSynchronizer.h
midasSynchronizer.cxx
midasUUID.h
midasUUID.cxx
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
