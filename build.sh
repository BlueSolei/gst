#!/bin/bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
pushd $SCRIPT_DIR
if [ "$1" = "clean" ]; then rm -rf build; fi
if [ ! -d "build" ]; then mkdir build; fi
cd build
cmake ..
cmake --build .
# uncomment if need installation
# cmake --build . --target install
popd
