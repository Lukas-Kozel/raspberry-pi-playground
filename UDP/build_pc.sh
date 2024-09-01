#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Change to the project directory
cd /home/luky/playground/raspberry_pi/UDP || { echo "Failed to change directory"; exit 1; }

#clean make
rm -rf build_pc/*

# Change to the build directory
cd "$(pwd)/build_pc" || { echo "Failed to change to build directory"; exit 1; }


# Run CMake to configure the project
cmake ..

# Build and install the project
make && make install

cd "$(pwd)/../bin/pc"
