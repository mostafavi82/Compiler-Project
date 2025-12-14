#!/bin/bash
# Debug script to test compiler step by step

echo "======================================"
echo "Debug: Testing Compiler"
echo "======================================"
echo ""

if [ ! -f "build/src/compiler" ]; then
    echo "Error: Compiler not built!"
    exit 1
fi

cd build/src

# Test 1: Very simple code
echo "Test 1: Minimal code"
echo "---"
CODE1="var x int = 5;"
echo "$CODE1"
echo "---"
./compiler "$CODE1" 2>&1
echo ""

# Test 2: With INC
echo "Test 2: With INC"
echo "---"
CODE2="var x int = 5;
INC x;"
echo "$CODE2"
echo "---"
./compiler "$CODE2" 2>&1
echo ""

# Test 3: With print
echo "Test 3: With print"
echo "---"
CODE3="var x int = 5;
print(x);"
echo "$CODE3"
echo "---"
./compiler "$CODE3" 2>&1
echo ""

# Test 4: Full input.txt
echo "Test 4: Full input.txt"
echo "---"
cat ../../input.txt
echo "---"
./compiler "$(cat ../../input.txt)" 2>&1
echo ""

echo "======================================"
echo "Check compiler.ll if no errors above"
echo "======================================"
