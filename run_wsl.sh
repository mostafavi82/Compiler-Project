#!/bin/bash
# Run script for WSL

echo "======================================"
echo "Running Compiler with LLVM"
echo "======================================"
echo ""

if [ ! -f "build/src/compiler" ]; then
    echo "Error: Compiler not built yet!"
    echo "Run ./build_wsl.sh first"
    exit 1
fi

# Read input file
INPUT_CODE=$(cat input.txt)

echo "Input code:"
echo "---"
cat input.txt
echo "---"
echo ""

# Run compiler
echo "Compiling..."
cd build/src
./compiler "$INPUT_CODE" > compiler.ll 2>&1

if [ $? -ne 0 ]; then
    echo "Compilation failed!"
    cat compiler.ll
    exit 1
fi

echo "Generated LLVM IR:"
echo "---"
head -20 compiler.ll
echo "..."
echo "---"
echo ""

# Generate object file
echo "Generating object file..."
llc --filetype=obj -o=compiler.o compiler.ll

if [ $? -ne 0 ]; then
    echo "llc failed!"
    exit 1
fi

# Link with runtime
echo "Linking..."
clang -o compilerbin compiler.o ../../rtCompiler.c

if [ $? -ne 0 ]; then
    echo "Linking failed!"
    exit 1
fi

# Run executable
echo ""
echo "======================================"
echo "Execution Output:"
echo "======================================"
./compilerbin

echo ""
echo "======================================"
echo "Done!"
echo "======================================"
