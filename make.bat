@echo off
mkdir win32-cmake-build
cd win32-cmake-build
cmake -G "MinGW Makefiles" ..
mingw32-make -j4
cd ..
mkdir build\imports
xcopy /Y win32-cmake-build\*.cse build\imports\