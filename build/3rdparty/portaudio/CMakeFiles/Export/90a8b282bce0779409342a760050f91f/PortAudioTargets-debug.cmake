#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "PortAudio::portaudio" for configuration "Debug"
set_property(TARGET PortAudio::portaudio APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(PortAudio::portaudio PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libportaudio.19.8.dylib"
  IMPORTED_SONAME_DEBUG "@rpath/libportaudio.2.dylib"
  )

list(APPEND _cmake_import_check_targets PortAudio::portaudio )
list(APPEND _cmake_import_check_files_for_PortAudio::portaudio "${_IMPORT_PREFIX}/lib/libportaudio.19.8.dylib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
