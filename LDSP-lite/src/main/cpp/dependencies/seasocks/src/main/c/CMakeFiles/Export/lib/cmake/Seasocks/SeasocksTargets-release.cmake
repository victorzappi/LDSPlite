#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Seasocks::seasocks" for configuration "Release"
set_property(TARGET Seasocks::seasocks APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Seasocks::seasocks PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libseasocks.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS Seasocks::seasocks )
list(APPEND _IMPORT_CHECK_FILES_FOR_Seasocks::seasocks "${_IMPORT_PREFIX}/lib/libseasocks.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
