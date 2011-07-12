project(MIDAScli)
cmake_minimum_required(VERSION 2.6)

set(MIDAScli_SRCS
  main.cxx
  midasCLI.cxx
  midasCLI.h
)

include(${MIDASClient_BINARY_DIR}/../CTK-build/CTK-build/Libs/DataManagement/MIDASClient/Core/CTKDataManagementMIDASClientCoreConfig.cmake)

include_directories(${CTKDataManagementMIDASClientCore_INCLUDE_DIRS})
link_directories(${CTKDataManagementMIDASClientCore_LIBRARY_DIRS})

add_executable(MIDAScli ${MIDAScli_SRCS})

target_link_libraries(MIDAScli
  ${CTKDataManagementMIDASClientCore_LIBRARIES}
)

install(TARGETS MIDAScli DESTINATION bin COMPONENT Runtime)
