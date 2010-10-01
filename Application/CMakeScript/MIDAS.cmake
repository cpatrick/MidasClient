# - Configure a project for downloading test data from a MIDAS server
# Include this module in the top CMakeLists.txt file of a project to
# enable downloading test data from MIDAS. Requires CTest module.
#   project(MyProject)
#   ...
#   include(CTest)
#   include(MIDAS.cmake)
#
# To use this module, set the following variable in your script:
#   MIDAS_REST_URL - URL of the MIDAS server's REST API
# Other optional variables:
#   MIDAS_DATA_DIR         - Where to place downloaded files
#                          - Defaults to PROJECT_BINARY_DIR/MIDAS_Data
#   MIDAS_KEY_DIR          - Where the key files are located
#                          - Defaults to PROJECT_SOURCE_DIR/MIDAS_Keys
#   MIDAS_DOWNLOAD_TIMEOUT - Timeout for download stage (default 0)
#
# Then call the following macro: 
#  midas_add_test(<testName> <keyFile> <program> [args...])
#   testName: Name of the test
#   keyFile: Point this to the ".md5" file you downloaded from MIDAS
#   program: The executable to be run after the download is complete
#   args: Optional args to the program.  If an arg is of the form
#         MIDAS{foo.ext.md5}
# EXAMPLE:
#  midas_add_test(someTest test.php.md5 php % arg1)
#   At test time, this would download the full content of test.php
#   from the MIDAS server, then run php on it, assuming you had placed
#   test.php.md5 (the key file you got from MIDAS corresponding to test.php)
#   in the current source directory.
#=============================================================================
# Copyright 2010 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================

function(midas_add_test testName)
  if(NOT DEFINED MIDAS_REST_URL)
    message(FATAL_ERROR "You must set MIDAS_REST_URL to the URL of the MIDAS REST API.")
  endif(NOT DEFINED MIDAS_REST_URL)

  if(NOT DEFINED MIDAS_KEY_DIR)
    set(MIDAS_KEY_DIR "${PROJECT_SOURCE_DIR}/MIDAS_Keys")
  endif(NOT DEFINED MIDAS_KEY_DIR)

  if(NOT DEFINED MIDAS_DATA_DIR)
    set(MIDAS_DATA_DIR "${PROJECT_BINARY_DIR}/MIDAS_Data")
  endif(NOT DEFINED MIDAS_DATA_DIR)
  file(MAKE_DIRECTORY "${MIDAS_DATA_DIR}/FetchScripts")

  if(NOT DEFINED MIDAS_DOWNLOAD_TIMEOUT)
    set(MIDAS_DOWNLOAD_TIMEOUT_STR "")
  else(NOT DEFINED MIDAS_DOWNLOAD_TIMEOUT)
    set(MIDAS_DOWNLOAD_TIMEOUT_STR "TIMEOUT ${MIDAS_DOWNLOAD_TIMEOUT}")
  endif(NOT DEFINED MIDAS_DOWNLOAD_TIMEOUT)

  # Substitute the downloaded file argument(s)
  foreach(arg ${ARGN})
    if(arg MATCHES "^MIDAS{.*}$")
      string(REGEX MATCH "{.*}$" keyFile "${arg}")
      string(REGEX REPLACE "^{" "" keyFile "${keyFile}")
      string(REGEX REPLACE "}$" "" keyFile "${keyFile}")
      midas_find_alg("${keyFile}")

      # Resolve file location
      if(EXISTS "${MIDAS_KEY_DIR}/${keyFile}")
        set(realKeyFile "${MIDAS_KEY_DIR}/${keyFile}")
      elseif(EXISTS "${MIDAS_KEY_DIR}/${testName}/${keyFile}")
        set(realKeyFile "${MIDAS_KEY_DIR}/${testName}/${keyFile}")
      else(EXISTS "${MIDAS_KEY_DIR}/${keyFile}")
        message(FATAL_ERROR "MIDAS key file ${MIDAS_KEY_DIR}/${keyFile} does not exist.")
      endif(EXISTS "${MIDAS_KEY_DIR}/${keyFile}")

      # Obtain the checksum
      file(READ ${realKeyFile} checksum)

      # Write the test script file for downloading
      file(WRITE "${MIDAS_DATA_DIR}/FetchScripts/download_${checksum}.cmake"
  "message(STATUS \"Data is here: ${MIDAS_REST_URL}/midas.bitstream.by.hash?hash=${checksum}&algorithm=${hash_alg}\")
if(NOT EXISTS \"${MIDAS_DATA_DIR}/${checksum}\")
  file(DOWNLOAD ${MIDAS_REST_URL}/midas.bitstream.by.hash?hash=${checksum}&algorithm=${hash_alg} \"${MIDAS_DATA_DIR}/${checksum}\" ${MIDAS_DOWNLOAD_TIMEOUT_STR} STATUS status)
  list(GET status 0 exitCode)
  list(GET status 1 errMsg)
  if(NOT exitCode EQUAL 0)
    file(REMOVE \"${MIDAS_DATA_DIR}/${checksum}\")
    message(FATAL_ERROR \"Error downloading ${checksum}: \${errMsg}\")
  endif(NOT exitCode EQUAL 0)
endif(NOT EXISTS \"${MIDAS_DATA_DIR}/${checksum}\")

execute_process(COMMAND \"${CMAKE_COMMAND}\" -E md5sum \"${MIDAS_DATA_DIR}/${checksum}\" OUTPUT_VARIABLE output)
string(SUBSTRING \${output} 0 32 computedChecksum)

if(NOT computedChecksum STREQUAL ${checksum})
  file(REMOVE \"${MIDAS_DATA_DIR}/${checksum}\")
  message(FATAL_ERROR \"Error: Computed checksum (\${computedChecksum}) did not match expected (${checksum})\")
endif(NOT computedChecksum STREQUAL ${checksum})
")
      list(APPEND downloadScripts download_${checksum}.cmake)
      list(APPEND testArgs "${MIDAS_DATA_DIR}/${checksum}")
    else(arg MATCHES "^MIDAS{.*}$")
      list(APPEND testArgs ${arg})
    endif(arg MATCHES "^MIDAS{.*}$")
  endforeach(arg)

  file(WRITE "${MIDAS_DATA_DIR}/FetchScripts/${testName}_fetchData.cmake"
       "#This is an auto generated file -- do not edit\n\n")
  foreach(downloadScript ${downloadScripts})
    file(APPEND "${MIDAS_DATA_DIR}/FetchScripts/${testName}_fetchData.cmake" "include(${downloadScript})\n")
  endforeach(downloadScript)

  add_test(${testName}_fetchData "${CMAKE_COMMAND}" -P "${MIDAS_DATA_DIR}/FetchScripts/${testName}_fetchData.cmake")
  # Finally, create the test
  add_test(${testName} ${testArgs})
  set_tests_properties(${testName} PROPERTIES DEPENDS ${testName}_fetchData)
endfunction(midas_add_test)


# Set hash_alg and base_filename
macro(midas_find_alg keyFile)
  # Split up the checksum extension from the real filename
  string(REGEX MATCH "\\.[^\\.]*$" hash_alg "${keyFile}")
  string(REGEX REPLACE "\\.[^\\.]*$" "" base_filename "${keyFile}")
  string(REPLACE "." "" hash_alg "${hash_alg}")
  string(TOUPPER "${hash_alg}" hash_alg)
endmacro(midas_find_alg)