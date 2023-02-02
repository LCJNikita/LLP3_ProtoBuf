#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "nanopb::protobuf-nanopb" for configuration "Debug"
set_property(TARGET nanopb::protobuf-nanopb APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(nanopb::protobuf-nanopb PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libprotobuf-nanopbd.so.0"
  IMPORTED_SONAME_DEBUG "libprotobuf-nanopbd.so.0"
  )

list(APPEND _IMPORT_CHECK_TARGETS nanopb::protobuf-nanopb )
list(APPEND _IMPORT_CHECK_FILES_FOR_nanopb::protobuf-nanopb "${_IMPORT_PREFIX}/lib/libprotobuf-nanopbd.so.0" )

# Import target "nanopb::protobuf-nanopb-static" for configuration "Debug"
set_property(TARGET nanopb::protobuf-nanopb-static APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(nanopb::protobuf-nanopb-static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libprotobuf-nanopbd.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS nanopb::protobuf-nanopb-static )
list(APPEND _IMPORT_CHECK_FILES_FOR_nanopb::protobuf-nanopb-static "${_IMPORT_PREFIX}/lib/libprotobuf-nanopbd.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
