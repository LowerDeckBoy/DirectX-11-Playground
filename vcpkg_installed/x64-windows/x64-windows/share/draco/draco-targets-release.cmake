get_filename_component(VCPKG_IMPORT_PREFIX "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "draco::draco" for configuration "Release"
set_property(TARGET draco::draco APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(draco::draco PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${VCPKG_IMPORT_PREFIX}/lib/draco.lib"
  )

list(APPEND _cmake_import_check_targets draco::draco )
list(APPEND _cmake_import_check_files_for_draco::draco "${VCPKG_IMPORT_PREFIX}/lib/draco.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
