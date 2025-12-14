#!/bin/bash
# Build script for Ubuntu WSL

echo "======================================"
echo "Building Compiler Project with LLVM"
echo "======================================"
echo ""

# Install dependencies
echo "Installing dependencies..."
echo "This may take a few minutes..."
sudo apt update
sudo apt install -y cmake clang llvm make build-essential

if [ $? -ne 0 ]; then
    echo "Failed to install dependencies"
    exit 1
fi

echo ""
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
echo ""
echo "To run:"
echo "  cd build/src"
echo "  ./compiler \"\$(cat ../../input.txt)\""
