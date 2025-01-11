#!/bin/bash

# Exit on any error
set -e

# Default values
BUILD_TYPE="Release"
JOBS=$(nproc)
CLEAN=0

# Help message
show_help() {
    echo "Usage: $0 [options]"
    echo
    echo "Options:"
    echo "  -h, --help       Show this help message"
    echo "  -d, --debug      Build in Debug mode (default: Release)"
    echo "  -j, --jobs N     Number of parallel jobs (default: number of CPU cores)"
    echo "  -c, --clean      Clean build directory before building"
    echo
    echo "Example:"
    echo "  $0 --debug -j 4  # Build in debug mode with 4 parallel jobs"
    exit 1
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            ;;
        -d|--debug)
            BUILD_TYPE="Debug"
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

# Clean if requested
if [ $CLEAN -eq 1 ]; then
    echo "Cleaning build directory..."
    rm -rf build
fi

# Create build directory if it doesn't exist
mkdir -p build
cd build

echo "Configuring build..."
echo "Build type: $BUILD_TYPE"
echo "Parallel jobs: $JOBS"

# Configure
cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE ..

# Build
echo "Building..."
make -j$JOBS

echo "Build complete!"
echo "Binary location: $(pwd)/bin/fclock"
