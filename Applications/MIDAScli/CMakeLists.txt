project(MIDAScli)
cmake_minimum_required(VERSION 2.6)

set(MIDAScli_SRCS
  main.cxx
  midasCLI.cxx
  midasCLIApplication.cxx
)

set(MIDAScli_MOC_SRCS
  midasCLIApplication.h
)

QT4_WRAP_CPP(MIDAScli_SRCS ${MIDAScli_MOC_SRCS})

include(${MIDASClient_BINARY_DIR}/Libs/Core/MIDASClientCoreConfig.cmake)

include_directories(${MIDASClientCore_INCLUDE_DIRS})
link_directories(${MIDASClientCore_LIBRARY_DIRS})

add_executable(MIDAScli ${MIDAScli_SRCS})

target_link_libraries(MIDAScli
  ${MIDASClientCore_LIBRARIES}
)

#install(TARGETS MIDAScli DESTINATION bin COMPONENT Runtime)
