#!/bin/bash

# Build and run terrain test
echo "Building terrain test..."

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake ..

# Build
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# Run the test
echo ""
echo "Running terrain test..."
./bin/test_terrain

echo ""
echo "Test completed!" 