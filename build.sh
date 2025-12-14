#!/bin/bash
# Clean old build and create fresh one
rm -rf build
mkdir build
cd build
cmake ..
make
