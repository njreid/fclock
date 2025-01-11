cmake_minimum_required(VERSION 3.10)
project(fclock VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Find required system libraries
find_library(CONFUSE_LIBRARY NAMES confuse)
find_library(PTHREAD_LIBRARY NAMES pthread)
find_library(RT_LIBRARY NAMES rt)
find_library(MATH_LIBRARY NAMES m)

if(NOT CONFUSE_LIBRARY)
    message(FATAL_ERROR "libconfuse not found")
endif()

# Add rpi-rgb-led-matrix as an external project
include(ExternalProject)
ExternalProject_Add(rpi-rgb-led-matrix
    GIT_REPOSITORY    https://github.com/hzeller/rpi-rgb-led-matrix.git
    GIT_TAG          master
    SOURCE_DIR       ${CMAKE_CURRENT_SOURCE_DIR}/external/rpi-rgb-led-matrix
    CONFIGURE_COMMAND ""
    BUILD_COMMAND    make -C ${CMAKE_CURRENT_SOURCE_DIR}/external/rpi-rgb-led-matrix/lib
    BUILD_IN_SOURCE  1
    INSTALL_COMMAND  ""
)

# Create imported target for rpi-rgb-led-matrix
add_library(rgbmatrix STATIC IMPORTED GLOBAL)

# We need to create the directories before setting properties
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/external/rpi-rgb-led-matrix/include)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/external/rpi-rgb-led-matrix/lib)

set_target_properties(rgbmatrix PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/external/rpi-rgb-led-matrix/lib/librgbmatrix.a
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/external/rpi-rgb-led-matrix/include
)
add_dependencies(rgbmatrix rpi-rgb-led-matrix)

# Add the src subdirectory
add_subdirectory(src)
