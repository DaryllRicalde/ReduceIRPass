#!/bin/bash

#   compile
# clang++ -S -c $1 -emit-llvm -o $2 -IreplacerIncludes

export LLVM_DIR=/usr/lib/llvm-13