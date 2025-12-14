#!/bin/bash
# Script to compile and run using LLVM

echo "Testing LLVM Compiler"
echo "======================================"

cd build/src

# Test different inputs to find the problem

echo ""
echo "Test 1: Simple variable declaration"
./compiler "var x int = 5;" 2>&1 | head -20

echo ""
echo "Test 2: Two variables"
./compiler "var x int = 5;
var y int = 10;" 2>&1 | head -20

echo ""
echo "Test 3: With comment"
./compiler "/* test */
var x int = 5;" 2>&1 | head -20

echo ""
echo "Test 4: With INC"
./compiler "var x int = 5;
INC x;" 2>&1 | head -20

echo ""
echo "Test 5: With print"
./compiler "var x int = 5;
print(x);" 2>&1 | head -20

echo ""
echo "======================================"
echo "If you see LLVM IR above, that test passed!"
echo "If you see 'Unexpected: X', that's the problem token"
