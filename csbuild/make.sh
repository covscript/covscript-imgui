#!/usr/bin/env bash
mkdir -p cmake-build/unix
cd       cmake-build/unix
cmake -G "Unix Makefiles" ../..
cmake --build . -- -j4
cd ../..
rm -rf build
mkdir -p build/imports
cp cmake-build/unix/*.cse build/imports/