cmake_minimum_required(VERSION 3.20)

project(
  ArtemisUserLibrary
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ============================================================
# User settings
# ============================================================

# CMake target name
set(USER_LIBRARY_NAME "tutorial")

# Namespace used by ROOT dictionary
set(USER_LIBRARY_NAMESPACE "art")

# Output shared-object name:
#
#   "SSD"     -> libSSD.so
#   "CribSSD" -> libCribSSD.so
#
set(USER_LIBRARY_OUTPUT_NAME "tutorial")

# ============================================================
# ROOT
# ============================================================

find_package(ROOT REQUIRED CONFIG)

# ============================================================
# ARTEMIS
# ============================================================

find_program(
  ARTEMIS_CONFIG
  NAMES artemis-config
  REQUIRED
)

execute_process(
  COMMAND ${ARTEMIS_CONFIG} --cflags
  OUTPUT_VARIABLE ARTEMIS_CFLAGS_RAW
  OUTPUT_STRIP_TRAILING_WHITESPACE
  COMMAND_ERROR_IS_FATAL ANY
)

execute_process(
  COMMAND ${ARTEMIS_CONFIG} --libs
  OUTPUT_VARIABLE ARTEMIS_LIBS_RAW
  OUTPUT_STRIP_TRAILING_WHITESPACE
  COMMAND_ERROR_IS_FATAL ANY
)

separate_arguments(
  ARTEMIS_CFLAGS
  NATIVE_COMMAND
  "${ARTEMIS_CFLAGS_RAW}"
)

separate_arguments(
  ARTEMIS_LIBS
  NATIVE_COMMAND
  "${ARTEMIS_LIBS_RAW}"
)

# ============================================================
# Discover source files
#
# Only *.cc directly under this directory are used.
# Subdirectories such as unused/ are ignored automatically.
# ============================================================

file(
  GLOB USER_LIBRARY_SOURCES
  CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
)

set(USER_LIBRARY_HEADERS "")
set(USER_LIBRARY_CLASSES "")

foreach(source IN LISTS USER_LIBRARY_SOURCES)

  get_filename_component(
    class_name
    "${source}"
    NAME_WE
  )

  set(
    header
    "${CMAKE_CURRENT_SOURCE_DIR}/${class_name}.h"
  )

  if(NOT EXISTS "${header}")
    message(
      FATAL_ERROR
      "Header not found for ${class_name}: ${header}"
    )
  endif()

  list(APPEND USER_LIBRARY_HEADERS "${header}")

  list(
    APPEND
    USER_LIBRARY_CLASSES
    "${USER_LIBRARY_NAMESPACE}::${class_name}"
  )

endforeach()

# ============================================================
# Generate LinkDef automatically
# ============================================================

set(
  USER_LIBRARY_LINKDEF
  "${CMAKE_CURRENT_BINARY_DIR}/LinkDef.h"
)

file(
  WRITE
  "${USER_LIBRARY_LINKDEF}"
  "#ifndef USER_LIBRARY_LINKDEF_H\n"
  "#define USER_LIBRARY_LINKDEF_H\n"
  "\n"
  "#ifdef __CLING__\n"
  "\n"
  "#pragma link off all globals;\n"
  "#pragma link off all classes;\n"
  "#pragma link off all functions;\n"
  "\n"
)

foreach(class IN LISTS USER_LIBRARY_CLASSES)

  file(
    APPEND
    "${USER_LIBRARY_LINKDEF}"
    "#pragma link C++ class ${class}+;\n"
  )

endforeach()

file(
  APPEND
  "${USER_LIBRARY_LINKDEF}"
  "\n"
  "#endif\n"
  "\n"
  "#endif\n"
)

# ============================================================
# ROOT dictionary
# ============================================================

set(
  USER_LIBRARY_DICTIONARY
  "G__${USER_LIBRARY_NAME}"
)

ROOT_GENERATE_DICTIONARY(
  ${USER_LIBRARY_DICTIONARY}

  ${USER_LIBRARY_HEADERS}

  LINKDEF
    ${USER_LIBRARY_LINKDEF}

  OPTIONS
    ${ARTEMIS_CFLAGS}
)

# ============================================================
# Shared library
# ============================================================

add_library(
  ${USER_LIBRARY_NAME}
  SHARED

  ${USER_LIBRARY_SOURCES}

  "${CMAKE_CURRENT_BINARY_DIR}/${USER_LIBRARY_DICTIONARY}.cxx"
)

set_target_properties(
  ${USER_LIBRARY_NAME}
  PROPERTIES
    OUTPUT_NAME "${USER_LIBRARY_OUTPUT_NAME}"
)

target_include_directories(
  ${USER_LIBRARY_NAME}
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_compile_options(
  ${USER_LIBRARY_NAME}
  PRIVATE
    ${ARTEMIS_CFLAGS}
    -Wall
    -Wextra
)

target_link_libraries(
  ${USER_LIBRARY_NAME}
  PRIVATE
    ${ARTEMIS_LIBS}

    ROOT::Core
    ROOT::RIO
    ROOT::Tree
    ROOT::TreePlayer
    ROOT::Geom
)

# ============================================================
# Information
# ============================================================

message(STATUS "")
message(STATUS "ARTEMIS user library")
message(STATUS "  Target:    ${USER_LIBRARY_NAME}")
message(STATUS "  Output:    lib${USER_LIBRARY_OUTPUT_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}")
message(STATUS "  Namespace: ${USER_LIBRARY_NAMESPACE}")

message(STATUS "")
message(STATUS "Classes:")

foreach(class IN LISTS USER_LIBRARY_CLASSES)
  message(STATUS "  ${class}")
endforeach()

message(STATUS "")
