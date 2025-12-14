#!/bin/bash
# Clean old build and create fresh one
echo "Cleaning old build..."
rm -rf build

echo "Creating build directory..."
mkdir -p build

echo "Running CMake..."
cd build
cmake ..

if [ $? -ne 0 ]; then
    echo "CMake failed!"
    exit 1
fi

echo "Building..."
make

if [ $? -ne 0 ]; then
    echo "Make failed!"
    exit 1
fi

echo ""
echo "======================================"
echo "Build successful!"
echo "======================================"
echo "Compiler is at: build/src/compiler"
