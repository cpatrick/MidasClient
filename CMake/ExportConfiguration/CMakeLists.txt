# Generate the MIDASCPPConfig.cmake file in the build tree. Also configure
# one for installation. The file tells external projects how to use
# MIDASCPP.

# Help store a literal dollar in a string.  CMake 2.2 allows escaped
# dollars but we have to support CMake 2.0.
SET(DOLLAR "$")

#-----------------------------------------------------------------------------
# Settings shared between the build tree and install tree.

EXPORT_LIBRARY_DEPENDENCIES(
  ${MIDASCPP_BINARY_DIR}/MIDASCPPLibraryDepends.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/CMakeExportBuildSettings.cmake)
CMAKE_EXPORT_BUILD_SETTINGS(
  ${MIDASCPP_BINARY_DIR}/MIDASCPPBuildSettings.cmake)

#-----------------------------------------------------------------------------
# Settings specific to the build tree.

# The install-only section is empty for the build tree.
SET(MIDASCPP_CONFIG_INSTALL_ONLY)

# The "use" file.
SET(MIDASCPP_USE_FILE_CONFIG 
  ${CMAKE_CURRENT_SOURCE_DIR}/UseMIDAScpp.cmake)

# The build settings file.
SET(MIDASCPP_BUILD_SETTINGS_FILE_CONFIG 
  ${MIDASCPP_BINARY_DIR}/MIDASCPPBuildSettings.cmake)

# The library directories.
SET(MIDASCPP_LIBRARY_DIRS_CONFIG ${MIDASCPP_LIBRARY_DIR})

# The runtime directories.
SET(MIDASCPP_RUNTIME_DIRS_CONFIG ${MIDASCPP_RUNTIME_DIR})

SET(MIDASCPP_APPLICATIONS_DIRS_CONFIG ${MIDASCPP_APPLICATIONS_DIR})

# The include directories.
SET(MIDASCPP_INCLUDE_DIRS_CONFIG ${MIDASCPP_INCLUDE_DIRS})

# The library dependencies file.
SET(MIDASCPP_LIBRARY_DEPENDS_FILE 
  ${MIDASCPP_BINARY_DIR}/MIDASCPPLibraryDepends.cmake)

# The CMake macros dir.
SET(MIDASCPP_CMAKE_DIR_CONFIG ${MIDASCPP_CMAKE_DIR})

# The build configuration information.
SET(MIDASCPP_CONFIGURATION_TYPES_CONFIG ${CMAKE_CONFIGURATION_TYPES})
SET(MIDASCPP_BUILD_TYPE_CONFIG ${CMAKE_BUILD_TYPE})

# Configure MIDASCPPConfig.cmake for the build tree.
CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/MIDAScppConfig.cmake.in
  ${MIDASCPP_BINARY_DIR}/MIDAScppConfig.cmake @ONLY IMMEDIATE)

#-----------------------------------------------------------------------------
# Settings specific to the install tree.
# Get back to Seb and Brad for this one, once you specify install targets
# in your CMakeLists.txt file
