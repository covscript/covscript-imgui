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
    Linux*)     GL_LIBS="";;
    Darwin*)    GL_LIBS="-framework Cocoa -framework OpenGL";;
    * )         GL_LIBS="" ;;
esac

DEFAULT_CXXFLAGS="-std=c++11 -I ../include -shared -fPIC -s -O3 $GLFW_CFLAGS"
DEFAULT_LDFLAGS="-ldl $GLFW_LDFLAGS $GL_LIBS"
DEFAULT_CXX=g++

set_flag CXX $DEFAULT_CXX
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
$CXX $CXXFLAGS ../*.cpp ../src/*.cpp $LDFLAGS -o ./imports/imgui.cse

