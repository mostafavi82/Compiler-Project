#!/bin/sh
# Build script for Alpine Linux (Docker Desktop WSL)

echo "======================================"
echo "Building Compiler Project with LLVM"
echo "======================================"
echo ""

# Install dependencies
echo "Installing dependencies..."
apk add cmake clang llvm make g++ --quiet

if [ $? -ne 0 ]; then
    echo "Failed to install dependencies"
    exit 1
fi

echo "Dependencies installed successfully"
echo ""

# Clean and create build directory
echo "Cleaning build directory..."
rm -rf build
mkdir -p build
cd build

# Run CMake
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
echo "Compiler built at: build/src/compiler"
