cmake_minimum_required(VERSION 3.10)

# Set cross-compilation environment
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Specify the cross compilers
set(CMAKE_C_COMPILER /home/njr/opt/x-tools/armv8-rpi3-linux-gnueabihf/bin/armv8-rpi3-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER /home/njr/opt/x-tools/armv8-rpi3-linux-gnueabihf/bin/armv8-rpi3-linux-gnueabihf-g++)

# Set the target environment path
set(CMAKE_FIND_ROOT_PATH /home/njr/opt/x-tools/armv8-rpi3-linux-gnueabihf/armv8-rpi3-linux-gnueabihf/sysroot)

# Search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# Search for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

project(fclock VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Set static linking flags
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(BUILD_SHARED_LIBS OFF)

# Create external project directories
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/external/libconfuse)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/external/libconfuse/install)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/external/libconfuse/install/include)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/external/libconfuse/install/lib)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/external/rpi-rgb-led-matrix)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/external/rpi-rgb-led-matrix/include)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/external/rpi-rgb-led-matrix/lib)

include(ExternalProject)

# Add libconfuse as an external project
ExternalProject_Add(libconfuse-build
    GIT_REPOSITORY    https://github.com/martinh/libconfuse.git
    GIT_TAG          v3.3
    SOURCE_DIR       ${CMAKE_CURRENT_SOURCE_DIR}/external/libconfuse
    CONFIGURE_COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/external/libconfuse && ./autogen.sh && env CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER} "CFLAGS=-O3 -fPIC -march=armv8-a" "CXXFLAGS=-O3 -fPIC -march=armv8-a" "LDFLAGS=-static-libgcc -static-libstdc++" ./configure --host=armv8-rpi3-linux-gnueabihf --enable-static --disable-shared --prefix=${CMAKE_CURRENT_SOURCE_DIR}/external/libconfuse/install
    BUILD_COMMAND    make -C ${CMAKE_CURRENT_SOURCE_DIR}/external/libconfuse
    INSTALL_COMMAND  make -C ${CMAKE_CURRENT_SOURCE_DIR}/external/libconfuse install
    BUILD_IN_SOURCE  1
)

# Add rpi-rgb-led-matrix as an external project
ExternalProject_Add(rpi-rgb-led-matrix-build
    GIT_REPOSITORY    https://github.com/hzeller/rpi-rgb-led-matrix.git
    GIT_TAG          master
    SOURCE_DIR       ${CMAKE_CURRENT_SOURCE_DIR}/external/rpi-rgb-led-matrix
    CONFIGURE_COMMAND ""
    BUILD_COMMAND    cd ${CMAKE_CURRENT_SOURCE_DIR}/external/rpi-rgb-led-matrix/lib && make "CC=${CMAKE_C_COMPILER}" "CXX=${CMAKE_CXX_COMPILER}" "CFLAGS=${MATRIX_CFLAGS}" "CXXFLAGS=${MATRIX_CXXFLAGS}" "LDFLAGS=-static-libgcc" "LDEXTRA=-static-libstdc++"
    BUILD_IN_SOURCE  1
    INSTALL_COMMAND  ""
)

# Create imported targets after the external projects are built
add_library(confuse STATIC IMPORTED GLOBAL)
add_dependencies(confuse libconfuse-build)
set_target_properties(confuse PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/external/libconfuse/install/lib/libconfuse.a
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/external/libconfuse/install/include
)

add_library(rgbmatrix STATIC IMPORTED GLOBAL)
add_dependencies(rgbmatrix rpi-rgb-led-matrix-build)
set_target_properties(rgbmatrix PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/external/rpi-rgb-led-matrix/lib/librgbmatrix.a
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/external/rpi-rgb-led-matrix/include
)

# Add the src subdirectory
add_subdirectory(src)
