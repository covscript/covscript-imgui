#!/usr/bin/env bash

set -e
SELF_DIR="$(dirname $(readlink -f $0))"
cd "$SELF_DIR"

function set_flag() {
    local append=false

    if [[ "$1" == "--append" ]]; then
        shift
        append=true
    fi

    local var="$1"; shift
    declare -n ref="$var" 
    if [[ "$ref" == "" ]]; then
        ref="$@"
    elif [[ "$append" == true ]]; then
        ref="$ref $@"
    fi
}

set_flag PKG_CONFIG "$(which pkg-config)"
if [[ "$PKG_CONFIG" == "" ]]; then
    echo "Error: pkg-config not found."
    exit 1
fi
GLFW_CFLAGS="$($PKG_CONFIG --cflags glfw3)"
GLFW_LDFLAGS="$($PKG_CONFIG --libs glfw3)"
if [[ "GLFW_CFLAGS" == "" || "$GLFW_LDFLAGS" == "" ]]; then
    echo "Error: glfw3 is required."
    exit 1
fi

GL_LIBS=""
case "$(uname -s)" in
    Linux*)     GL_LIBS="-lGL";;
    Darwin*)    GL_LIBS="-framework Cocoa -framework OpenGL";;
    * )         GL_LIBS="-lGL" ;;
esac

COVSCRIPT_LDFLAGS="-L$CS_DEV_PATH/lib -lcovscript"

COMMON_FLAGS="-I$CS_DEV_PATH/include -I../include -fPIC -O3"
DEFAULT_CXXFLAGS="-std=c++11 -shared -s $COMMON_FLAGS $GLFW_CFLAGS"
DEFAULT_CFLAGS="$COMMON_FLAGS"
DEFAULT_LDFLAGS="$COVSCRIPT_LDFLAGS $GLFW_LDFLAGS $GL_LIBS"
DEFAULT_CXX=g++
DEFAULT_CC=gcc

set_flag CC $DEFAULT_CC
set_flag CXX $DEFAULT_CXX
set_flag --append CFLAGS $DEFAULT_CFLAGS
set_flag --append CXXFLAGS $DEFAULT_CXXFLAGS
set_flag --append LDFLAGS $DEFAULT_LDFLAGS

while [[ "$1"x != ""x ]]; do
    arg="$1"; shift
    case "$arg" in
        --cxxflags=* ) CXXFLAGS="${arg##--cxxflags=}" ;;
        --ldflags=* ) LDFLAGS="${arg##--ldflags=}" ;;
        "--" ) break ;;
    esac
done

mkdir -p build
cd build
mkdir -p imports
for file in ../src/*.c
do
$CC $CFLAGS -c $file -o $file.o &
done
for file in ../src/*.cpp
do
$CXX $CXXFLAGS -c $file -o $file.o &
done
wait
$CXX $CXXFLAGS ../imgui_font.cpp $COVSCRIPT_LDFLAGS -o ./imports/imgui_font.cse &
$CXX $CXXFLAGS ../src/*.o ../imgui.cpp ../impl/imgui_impl_glfw.cpp ../impl/imgui_impl_opengl3.cpp $LDFLAGS -o ./imports/imgui.cse &
$CXX $CXXFLAGS -DIMGUI_IMPL_GL2 ../src/*.o ../imgui.cpp ../impl/imgui_impl_glfw.cpp ../impl/imgui_impl_opengl2.cpp $LDFLAGS -o ./imports/imgui_gl2.cse &
wait
rm ../src/*.o