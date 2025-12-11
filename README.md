<h3>     </h3>
<h1 align="center"> Simple Compiler - Phase 1 </h1>
<h3 align="center"> Compiler Design Fundamentals </h3>
<h5 align="center"> Final Project - <a href="https://en.sbu.ac.ir/">Shahid Beheshti University</a> (2024) </h5>
<h3>      </h3>

## Overview
This project is a simple compiler that compiles a custom programming language to LLVM IR. The language supports various data types, control structures, and built-in functions.

## Features Implemented

### 1. Data Types
- `int` - integers (fully supported)
- `bool` - true/false values
- `float` - floating point numbers
- `array` - arrays

### 2. Variable Declaration
```c
var x int = 5;
var y float = 3.14;
var flag bool = true;
array arr = [1, 2, 3];
```

### 3. Special Assignment Statements
```c
ADD x y z    // x = y + z
SUB x y z    // x = y - z
MUL x y z    // x = y * z
DIV x y z    // x = y / z
MOD x y z    // x = y % z
INC x        // x++
DEC x        // x--
PLE x y      // x += y
MIE x y      // x -= y
AND x y z    // x = y && z (bool only)
OR x y z     // x = y || z (bool only)
```

### 4. Control Flow
```c
// If-else statements
if (x > 5) {
    print(x);
} else if (y < 10) {
    print(y);
} else {
    print(0);
}

// For loops
for (int i = 0; i < 10; i++) {
    INC x;
}

// Foreach loops
foreach (num in numbers) {
    print(num);
}

// Pattern matching
match x {
    0 -> print(0),
    1 -> print(1),
    _ -> print(999)
}
```

### 5. Built-in Functions
- `print(x)` - print value
- `to_int(x)`, `to_float(x)`, `to_bool(x)` - type conversion
- `abs(x)` - absolute value
- `length(arr)` - array length
- `max(arr)` - maximum element
- `index(arr, i)` - array access
- `find(arr, condition)` - search in array

### 6. Comments
```c
/* Single line comment */

/* Multi-line
   comment */
```

## Building the Project

### Requirements
- CMake 3.8 or higher
- LLVM 10 or higher
- C++ compiler with C++17 support

### Build
```bash
./build.sh
```

Or manually:
```bash
mkdir build
cd build
cmake ..
make
```

### Run
```bash
./run.sh
```

Input code is in `input.txt`.

## Example Programs

### Example 1: Simple calculations
```c
/* Test program */

var x int = 5;
var y int = 10;

INC x;
print(x);

ADD y x y;
print(y);
```

### Example 2: Conditional statements
```c
var x int = 5;

if (x > 5) {
    print(100);
} else {
    print(200);
}
```

### Example 3: Loops
```c
var sum int = 0;

for (int i = 0; i < 10; i++) {
    PLE sum i;
}

print(sum);
```

## Project Structure

```
.
├── src/
│   ├── AST.h           # Abstract Syntax Tree definitions
│   ├── Lexer.h/cpp     # Lexical analyzer
│   ├── Parser.h/cpp    # Syntax analyzer
│   ├── Sema.h/cpp      # Semantic analyzer
│   ├── CodeGen.h/cpp   # LLVM IR code generation
│   └── Compiler.cpp    # Main entry point
├── input.txt           # Input source code
├── build.sh            # Build script
└── run.sh              # Run script
```

## Current Limitations

1. **Data types**: Only `int` is fully supported
2. **Arrays**: Partial implementation
3. **Functions**: Some functions are placeholders
4. **Error handling**: Basic implementation

## Authors
Compiler Design Course - Shahid Beheshti University (2024)
