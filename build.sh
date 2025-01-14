#!/bin/bash

# Exit on any error
set -e

# Default values
BUILD_MODE="dev"  # dev is default
JOBS=$(nproc)
CLEAN=0
BUILD_DIR="build-arm"

# Help message
show_help() {
    echo "Usage: $0 [options]"
    echo
    echo "Options:"
    echo "  -h, --help       Show this help message"
    echo "  --debug          Build with debug symbols, minimal optimization"
    echo "  --dev            Development build: optimize for compile speed (default)"
    echo "  --release        Release build: optimize for size and speed"
    echo "  -j, --jobs N     Number of parallel jobs (default: number of CPU cores)"
    echo "  -c, --clean      Clean build directory before building"
    echo
    echo "Build Modes:"
    echo "  debug   - Debug symbols, assertions, minimal optimization (-O0 -g)"
    echo "  dev     - Fast compilation, some optimization (-O1 -g)"
    echo "  release - Maximum optimization, stripped (-O3 -flto -s)"
    echo
    echo "Examples:"
    echo "  $0 --debug -j 4      # Debug build with 4 parallel jobs"
    echo "  $0 --release         # Optimized release build"
    exit 1
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            ;;
        --debug)
            BUILD_MODE="debug"
            shift
            ;;
        --dev)
            BUILD_MODE="dev"
            shift
            ;;
        --release)
            BUILD_MODE="release"
            shift
            ;;
        -j|--jobs)
            JOBS="$2"
            shift 2
            ;;
        -c|--clean)
            CLEAN=1
            shift
            ;;
        *)
            echo "Unknown option: $1"
            show_help
            ;;
    esac
done

# Set build type and flags based on mode
case $BUILD_MODE in
    debug)
        CMAKE_BUILD_TYPE="Debug"
        COMMON_FLAGS="-O0 -g3 -fno-omit-frame-pointer"
        MATRIX_CFLAGS="-O0 -g3 -fno-omit-frame-pointer -fPIC -march=armv8-a"
        MATRIX_CXXFLAGS="-O0 -g3 -fno-omit-frame-pointer -fPIC -march=armv8-a -fno-exceptions -std=c++11"
        ;;
    dev)
        CMAKE_BUILD_TYPE="RelWithDebInfo"
        COMMON_FLAGS="-O1 -g"
        MATRIX_CFLAGS="-O1 -g -fPIC -march=armv8-a"
        MATRIX_CXXFLAGS="-O1 -g -fPIC -march=armv8-a -fno-exceptions -std=c++11"
        ;;
    release)
        CMAKE_BUILD_TYPE="Release"
        COMMON_FLAGS="-O3 -flto -march=armv8-a"
        LINKER_FLAGS="-s"
        MATRIX_CFLAGS="-O3 -flto -fPIC -march=armv8-a"
        MATRIX_CXXFLAGS="-O3 -flto -fPIC -march=armv8-a -fno-exceptions -std=c++11"
        ;;
esac

# Clean if requested
if [ $CLEAN -eq 1 ]; then
    echo "Cleaning build directory..."
    rm -rf $BUILD_DIR
fi

# Create build directory if it doesn't exist
mkdir -p $BUILD_DIR
cd $BUILD_DIR

echo "Configuring build..."
echo "Build mode: $BUILD_MODE"
echo "Build type: $CMAKE_BUILD_TYPE"
echo "Parallel jobs: $JOBS"

# Configure with CMake
cmake \
    -DCMAKE_BUILD_TYPE="$CMAKE_BUILD_TYPE" \
    -DCMAKE_TOOLCHAIN_FILE="../cmake/armv8-rpi3-linux-gnueabihf.cmake" \
    -DCMAKE_C_FLAGS_DEBUG="$COMMON_FLAGS" \
    -DCMAKE_CXX_FLAGS_DEBUG="$COMMON_FLAGS" \
    -DCMAKE_C_FLAGS_RELWITHDEBINFO="$COMMON_FLAGS" \
    -DCMAKE_CXX_FLAGS_RELWITHDEBINFO="$COMMON_FLAGS" \
    -DCMAKE_C_FLAGS_RELEASE="$COMMON_FLAGS" \
    -DCMAKE_CXX_FLAGS_RELEASE="$COMMON_FLAGS" \
    -DMATRIX_CFLAGS="$MATRIX_CFLAGS" \
    -DMATRIX_CXXFLAGS="$MATRIX_CXXFLAGS" \
    ${LINKER_FLAGS:+-DCMAKE_EXE_LINKER_FLAGS="$LINKER_FLAGS"} \
    ..

# Build
echo "Building..."
make -j$JOBS

echo "Build complete!"
echo "Binary location: $(pwd)/bin/fclock"
