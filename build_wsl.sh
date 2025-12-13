#!/bin/bash
# Build script for WSL

echo "======================================"
echo "Building Compiler Project with LLVM"
echo "======================================"
echo ""

# Check if we're in WSL
if ! grep -q Microsoft /proc/version 2>/dev/null; then
    echo "Error: This script must be run in WSL"
    exit 1
fi

# Install dependencies if needed
echo "Checking dependencies..."
if ! command -v cmake &> /dev/null; then
    echo "Installing cmake..."
    sudo apt-get update
    sudo apt-get install -y cmake
fi

if ! command -v clang++ &> /dev/null; then
    echo "Installing clang..."
    sudo apt-get install -y clang llvm
fi

if ! command -v make &> /dev/null; then
    echo "Installing make..."
    sudo apt-get install -y build-essential
fi

# Clean build directory
echo ""
echo "Cleaning build directory..."
rm -rf build
mkdir -p build
cd build

# Run CMake
echo ""
echo "Running CMake..."
cmake ..

if [ $? -ne 0 ]; then
    echo "CMake failed!"
    exit 1
fi

# Build
echo ""
echo "Building..."
make

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo ""
echo "======================================"
echo "Build successful!"
echo "======================================"
echo ""
echo "To run the compiler:"
echo "  cd build/src"
echo "  ./compiler \"\$(cat ../../input.txt)\""
