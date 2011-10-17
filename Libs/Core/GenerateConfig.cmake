CONFIGURE_FILE(
  ${CTKDataManagementMIDASClientCore_SOURCE_DIR}/UseCTKDataManagementMIDASClientCore.cmake.in
  ${CTKDataManagementMIDASClientCore_BINARY_DIR}/UseCTKDataManagementMIDASClientCore.cmake COPYONLY)

# Library directory
SET(CTKDataManagementMIDASClientCore_LIBRARY_DIRS_CONFIG
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  ${sqlite_LIBRARY_DIRS}
)

# Include directories
SET(CTKDataManagementMIDASClientCore_INCLUDE_DIRS_CONFIG 
  ${CTKDataManagementMIDASClientCore_SOURCE_DIR}/Code
  ${CTKDataManagementMIDASClientCore_BINARY_DIR}/Code
  ${QT_INCLUDES}
  ${sqlite_INCLUDE_DIRS}
  )

# Libraries
SET(CTKDataManagementMIDASClientCore_LIBRARIES_CONFIG
  CTKDataManagementMIDASClientCore
  ${sqlite_LIBRARIES}
  ${QT_QTCORE_LIBRARY}
  ${QT_QTGUI_LIBRARY}
  ${QT_QTSCRIPT_LIBRARY}
  ${QT_QTNETWORK_LIBRARY}
)

# Use file
SET(CTKDataManagementMIDASClientCore_USE_FILE_CONFIG ${CTKDataManagementMIDASClientCore_BINARY_DIR}/UseCTKDataManagementMIDASClientCore.cmake)

# Configure config file
CONFIGURE_FILE(
  ${CTKDataManagementMIDASClientCore_SOURCE_DIR}/CTKDataManagementMIDASClientCoreConfig.cmake.in
  ${CTKDataManagementMIDASClientCore_BINARY_DIR}/CTKDataManagementMIDASClientCoreConfig.cmake @ONLY)
  