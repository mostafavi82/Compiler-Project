cd build
cd src

echo "Compiling input.txt..."
./compiler "$(cat ../../input.txt)" > compiler.ll 2>&1

if [ $? -ne 0 ]; then
    echo "Compilation failed!"
    cat compiler.ll
    exit 1
fi

echo "Compilation successful, generating object file..."
llc --filetype=obj -o=compiler.o compiler.ll

if [ $? -ne 0 ]; then
    echo "llc failed!"
    exit 1
fi

echo "Linking..."
clang -o compilerbin compiler.o ../../rtCompiler.c

if [ $? -ne 0 ]; then
    echo "Linking failed!"
    exit 1
fi

echo "Running program..."
echo "======================================"
./compilerbin
echo "======================================"
