#!/bin/bash

# Specify the absolute path to your project root
PROJECT_ROOT="/home/luky/playground/raspberry_pi/UDP/"

# Navigate to the project root directory
cd "$PROJECT_ROOT" || exit

# Create build directory if it doesn't exist
mkdir -p build_pc

# Navigate to the build directory
cd build_pc || exit

#remove previous build files
rm -rf /build_pc *

# Run CMake
cmake  ..

# Build the project
make

# Install the binaries
make install
