#!/bin/bash
# Test parser with verbose output

cd build/src

echo "Testing parser with simple input..."
echo ""
echo "Input: var x int = 5;"
echo "---"
./compiler "var x int = 5;" 2>&1
echo "---"
echo ""

echo "Checking if compiler.ll was created:"
if [ -f "compiler.ll" ]; then
    echo "compiler.ll exists, content:"
    cat compiler.ll
else
    echo "compiler.ll was NOT created"
fi
