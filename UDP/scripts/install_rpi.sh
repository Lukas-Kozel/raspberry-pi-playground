#!/bin/bash

# Specify the absolute path to your project root
PROJECT_ROOT="/home/luky/playground/raspberry_pi/UDP/"

# Navigate to the project root directory
cd "$PROJECT_ROOT" || exit

# Create build directory if it doesn't exist
mkdir -p build_rpi

# Navigate to the build directory
cd build_rpi || exit

#remove previous build files
rm -rf /build_rpi *

# Run CMake with the toolchain file
cmake -DCMAKE_TOOLCHAIN_FILE=../raspberrypi.toolchain.cmake ..

# Build the project
make

# Install the binaries
make install
