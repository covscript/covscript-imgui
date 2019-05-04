@echo off
mkdir cmake-build\mingw-w64
cd    cmake-build\mingw-w64
cmake -G "MinGW Makefiles" ..\..
cmake --build . -- -j4
cd ..\..
rd /S /Q build
mkdir build\imports
xcopy /Y cmake-build\mingw-w64\*.cse build\imports\