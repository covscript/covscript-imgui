#!/usr/bin/env bash
mkdir -p unix-cmake-build
cd unix-cmake-build
cmake -G "Unix Makefiles" ..
make -j4
cd ..
mkdir -p build/imports
cp unix-cmake-build/*.cse build/imports/