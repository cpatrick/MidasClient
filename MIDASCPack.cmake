IF(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
  
  # Set the options file that needs to be included inside CMakeCPackOptions.cmake
  #SET(QT_DIALOG_CPACK_OPTIONS_FILE ${CMake_BINARY_DIR}/Source/QtDialog/QtDialogCPack.cmake)
  CONFIGURE_FILE("${MIDASCPP_SOURCE_DIR}/CMakeCPackOptions.cmake.in"
    "${MIDASCPP_BINARY_DIR}/CMakeCPackOptions.cmake" @ONLY)
  SET(CPACK_PROJECT_CONFIG_FILE "${MIDASCPP_BINARY_DIR}/CMakeCPackOptions.cmake")
  SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "MIDAS Desktop Application")
  SET(CPACK_PACKAGE_VENDOR "Kitware")
  #SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Copyright.txt")
  #SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/Copyright.txt")
  SET(CPACK_PACKAGE_VERSION_MAJOR "${MIDASCPP_VERSION_MAJOR}")
  SET(CPACK_PACKAGE_VERSION_MINOR "${MIDASCPP_VERSION_MINOR}")
  SET(CPACK_PACKAGE_VERSION_PATCH "${MIDASCPP_VERSION_PATCH}")
  SET(CPACK_PACKAGE_INSTALL_DIRECTORY "MIDASDesktop ${MIDASCPP_VERSION_MAJOR}.${MIDASCPP_VERSION_MINOR}")
  SET(CPACK_PACKAGE_EXECUTABLES "MIDASDesktop" "MIDAS Desktop")
  SET(CPACK_SOURCE_PACKAGE_FILE_NAME
    "midasdesktop-${MIDASCPP_VERSION_MAJOR}.${MIDASCPP_VERSION_MINOR}.${MIDASCPP_VERSION_PATCH}")
  
  IF(NOT DEFINED CPACK_SYSTEM_NAME)
    # make sure package is not Cygwin-unknown, for Cygwin just
    # cygwin is good for the system name
    IF("${CMAKE_SYSTEM_NAME}" STREQUAL "CYGWIN")
      SET(CPACK_SYSTEM_NAME Cygwin)
    ELSE("${CMAKE_SYSTEM_NAME}" STREQUAL "CYGWIN")
      SET(CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})
    ENDIF("${CMAKE_SYSTEM_NAME}" STREQUAL "CYGWIN")
  ENDIF(NOT DEFINED CPACK_SYSTEM_NAME)
  IF(${CPACK_SYSTEM_NAME} MATCHES Windows)
    IF(CMAKE_CL_64)
      SET(CPACK_SYSTEM_NAME win64-${CMAKE_SYSTEM_PROCESSOR})
    ELSE(CMAKE_CL_64)
      SET(CPACK_SYSTEM_NAME win32-${CMAKE_SYSTEM_PROCESSOR})
    ENDIF(CMAKE_CL_64)
  ENDIF(${CPACK_SYSTEM_NAME} MATCHES Windows)
  IF(NOT DEFINED CPACK_PACKAGE_FILE_NAME)
    # if the CPACK_PACKAGE_FILE_NAME is not defined by the cache
    # default to source package - system, on cygwin system is not 
    # needed
    IF(CYGWIN)
      SET(CPACK_PACKAGE_FILE_NAME "${CPACK_SOURCE_PACKAGE_FILE_NAME}")
    ELSE(CYGWIN)
      SET(CPACK_PACKAGE_FILE_NAME 
        "${CPACK_SOURCE_PACKAGE_FILE_NAME}-${CPACK_SYSTEM_NAME}")
    ENDIF(CYGWIN)
  ENDIF(NOT DEFINED CPACK_PACKAGE_FILE_NAME)
  #SET(CPACK_PACKAGE_CONTACT "cmake@cmake.org")

# cygwin specific packaging stuff
  IF(CYGWIN)
    
    # if we are on cygwin and have cpack, then force the 
    # doc, data and man dirs to conform to cygwin style directories
    SET(MIDASCPP_DOC_DIR "/share/doc/${CPACK_PACKAGE_FILE_NAME}")
    SET(MIDASCPP_DATA_DIR "/share/${CPACK_PACKAGE_FILE_NAME}")
    SET(MIDASCPP_MAN_DIR "/share/man")
    # let the user know we just forced these values
    MESSAGE(STATUS "Setup for Cygwin packaging")
    MESSAGE(STATUS "Override cache MIDASCPP_DOC_DIR = ${MIDASCPP_DOC_DIR}")
    MESSAGE(STATUS "Override cache MIDASCPP_DATA_DIR = ${MIDASCPP_DATA_DIR}")
    MESSAGE(STATUS "Override cache MIDASCPP_MAN_DIR = ${MIDASCPP_MAN_DIR}")
    
    # setup the cygwin package name
    SET(CPACK_PACKAGE_NAME midascpp)
    # setup the name of the package for cygwin cmake-2.4.3
    SET(CPACK_PACKAGE_FILE_NAME
      "${CPACK_PACKAGE_NAME}-${MIDASCPP_VERSION_MAJOR}.${MIDASCPP_VERSION_MINOR}.${MIDASCPP_VERSION_PATCH}")
    # the source has the same name as the binary
    SET(CPACK_SOURCE_PACKAGE_FILE_NAME ${CPACK_PACKAGE_FILE_NAME})
    # Create a cygwin version number in case there are changes for cygwin
    # that are not reflected upstream in CMake
    SET(CPACK_CYGWIN_PATCH_NUMBER 1)
    # include the sub directory cmake file for cygwin that
    # configures some files and adds some install targets
    # this file uses some of the package file name variables
  ENDIF(CYGWIN)
  # include CPack model once all variables are set
  INCLUDE(CPack)
ENDIF(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")