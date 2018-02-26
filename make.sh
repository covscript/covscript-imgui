#!/bin/bash

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

DEFAULT_CXXFLAGS="-std=c++11 -I ../include -shared -fPIC -s -O3"
DEFAULT_LDFLAGS="-ldl"
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
$CXX $CXXFLAGS ../darwin.cpp $LDFLAGS -o ./imports/darwin.cse

