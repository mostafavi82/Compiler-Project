#!/bin/bash
# Run script for Ubuntu WSL

echo "======================================"
echo "Running Compiler with LLVM"
echo "======================================"
echo ""

if [ ! -f "build/src/compiler" ]; then
    echo "Error: Compiler not built yet!"
    echo "Run: ./build_ubuntu.sh"
    exit 1
fi

# Show input
echo "Input code:"
echo "---"
cat input.txt
echo "---"
echo ""

# Read input file
INPUT_CODE=$(cat input.txt)

# Run compiler
echo "Compiling..."
cd build/src
./compiler "$INPUT_CODE" > compiler.ll 2>&1

if [ $? -ne 0 ]; then
    echo "Compilation failed!"
    cat compiler.ll
    exit 1
fi

echo "LLVM IR generated successfully"
echo ""

# Generate object file
echo "Generating object file..."
llc --filetype=obj -o=compiler.o compiler.ll

if [ $? -ne 0 ]; then
    echo "llc failed!"
    exit 1
fi

# Link
echo "Linking..."
clang -o compilerbin compiler.o ../../rtCompiler.c

if [ $? -ne 0 ]; then
    echo "Linking failed!"
    exit 1
fi

# Run
echo ""
echo "======================================"
echo "Execution Output:"
echo "======================================"
./compilerbin
echo ""
echo "======================================"
