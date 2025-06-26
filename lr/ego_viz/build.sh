#!/bin/bash

# Build script for Ego Viz Godot extension

set -e

echo "Building Ego Viz extension..."

# Create build directory if it doesn't exist
mkdir -p build

# Configure with CMake
cd build
cmake ..

# Build the project
make -j4

echo "Build complete! Extension should be in godot/ directory."
echo "You can now open the godot/ folder in Godot to test the extension." 