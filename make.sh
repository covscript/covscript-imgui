#!/usr/bin/env bash
mkdir cmake-build
cd cmake-build
cmake -G "Unix Makefiles" ..
make -j4
cd ..
mkdir -p build/imports
mv cmake-build/*.cse build/imports/