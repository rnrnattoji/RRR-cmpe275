#!/bin/bash

# set -eu 

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

cd "$SCRIPT_DIR" || exit 1

clang_path="$(which clang-18)"
clang_flag="clang++-18"

if [ -z "$clang_path" ]; then
    clang_path="$(which clang)"
    clang_flagg="clang++-"
fi

rm -rf build

cmake --no-warn-unused-cli -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_C_COMPILER:FILEPATH="$clang_path" -DCMAKE_CXX_COMPILER:FILEPATH="${clang_flag}" -S"${SCRIPT_DIR}" -B"${SCRIPT_DIR}/"build -G "Unix Makefiles"

cmake --build build --config Debug --target all -j 18 --

cmake --build build --config Debug --target install -j 18 --