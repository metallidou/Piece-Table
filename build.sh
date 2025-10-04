#!/bin/bash

# Exit immediately if a command fails
set -e

# Name of the build directory
BUILD_DIR="build"

# Create the build directory
if [ ! -d "$BUILD_DIR" ]; then
    echo "Creating build directory..."
    mkdir "$BUILD_DIR"
fi

# Move into the build directory
cd "$BUILD_DIR"

# Run CMake to generate build files
echo "Running CMake..."
cmake ..

# Build the project
echo "Building project..."
cmake --build .

# Run tests using Catch2
echo "Running tests..."
./unit_test

echo "Build completed successfully!"
