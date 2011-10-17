CONFIGURE_FILE(
  ${CTKDataManagementMIDASClientWidgets_SOURCE_DIR}/UseCTKDataManagementMIDASClientWidgets.cmake.in
  ${CTKDataManagementMIDASClientWidgets_BINARY_DIR}/UseCTKDataManagementMIDASClientWidgets.cmake COPYONLY)

# Library directory
SET(CTKDataManagementMIDASClientWidgets_LIBRARY_DIRS_CONFIG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Include directories
SET(CTKDataManagementMIDASClientWidgets_INCLUDE_DIRS_CONFIG 
  ${CTKDataManagementMIDASClientWidgets_SOURCE_DIR}
  ${CTKDataManagementMIDASClientWidgets_SOURCE_DIR}/GUI
  ${CTKDataManagementMIDASClientWidgets_BINARY_DIR}
  )

# Use file
SET(CTKDataManagementMIDASClientWidgets_USE_FILE_CONFIG ${CTKDataManagementMIDASClientWidgets_BINARY_DIR}/UseCTKDataManagementMIDASClientWidgets.cmake)

# Configure config file
CONFIGURE_FILE(
  ${CTKDataManagementMIDASClientWidgets_SOURCE_DIR}/CTKDataManagementMIDASClientWidgetsConfig.cmake.in
  ${CTKDataManagementMIDASClientWidgets_BINARY_DIR}/CTKDataManagementMIDASClientWidgetsConfig.cmake @ONLY)
