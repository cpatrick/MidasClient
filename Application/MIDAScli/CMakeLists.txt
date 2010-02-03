PROJECT(MIDAScli)
cmake_minimum_required(VERSION 2.6)

SET(MIDAScli_SRCS
main.cxx
midasCLI.h
midasCLI.cxx
midasProgressReporter.h
midasProgressReporter.cxx
midasStandardIncludes.h
midasSynchronizer.h
midasSynchronizer.cxx
midasUUID.h
midasUUID.cxx
)

ADD_EXECUTABLE(MIDAScli ${MIDAScli_SRCS})
TARGET_LINK_LIBRARIES(MIDAScli MIDAS)

# Copy SQLite database definitions to bin dir
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/Application/MIDAScli/sql/MIDAS_SQLite.sql
               ${CMAKE_BINARY_DIR}/sql/MIDAS_SQLite.sql
               COPYONLY)
