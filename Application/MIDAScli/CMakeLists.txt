PROJECT(MIDAScli)
cmake_minimum_required(VERSION 2.6)

SET(MIDAScli_SRCS
main.cxx
midasCLI.cxx
midasCLI.h
)

ADD_EXECUTABLE(MIDAScli ${MIDAScli_SRCS})
TARGET_LINK_LIBRARIES(MIDAScli MIDAS)

SET(midas_db "${CMAKE_BINARY_DIR}/midas.db")
