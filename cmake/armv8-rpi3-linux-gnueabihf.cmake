# Target system
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Cross compiler paths
set(CROSS_PATH /home/njr/opt/x-tools/armv8-rpi3-linux-gnueabihf)
set(CMAKE_C_COMPILER ${CROSS_PATH}/bin/armv8-rpi3-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER ${CROSS_PATH}/bin/armv8-rpi3-linux-gnueabihf-g++)
set(CMAKE_AR ${CROSS_PATH}/bin/armv8-rpi3-linux-gnueabihf-ar)
set(CMAKE_RANLIB ${CROSS_PATH}/bin/armv8-rpi3-linux-gnueabihf-ranlib)
set(CMAKE_STRIP ${CROSS_PATH}/bin/armv8-rpi3-linux-gnueabihf-strip)

# Target environment paths
set(CMAKE_FIND_ROOT_PATH ${CROSS_PATH}/armv8-rpi3-linux-gnueabihf/sysroot)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv8-a")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv8-a")

# Static linking
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
