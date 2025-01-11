#!/bin/bash

# Exit on any error
set -e

echo "Cleaning build artifacts..."

# Remove build directory
if [ -d "build" ]; then
    echo "Removing build directory..."
    rm -rf build
fi

# Remove external dependencies
if [ -d "external" ]; then
    echo "Removing external dependencies..."
    rm -rf external
fi

# Remove any stray object files
echo "Removing object files..."
find . -name "*.o" -type f -delete

# Remove any stray static libraries
echo "Removing static libraries..."
find . -name "*.a" -type f -delete

# Remove any stray shared libraries
echo "Removing shared libraries..."
find . -name "*.so" -type f -delete

# Remove any CMake cache files
echo "Removing CMake cache files..."
find . -name "CMakeCache.txt" -type f -delete
find . -name "CMakeFiles" -type d -exec rm -rf {} +
find . -name "cmake_install.cmake" -type f -delete

echo "Clean complete!"
