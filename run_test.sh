#!/bin/bash
export LLVM_DIR=/Users/nikos/gr/llvm-project/build
$LLVM_DIR/bin/clang -emit-llvm -c ./inputs/input_for_hello.c -o output/input_for_hello.bc
# MakeFile
# $LLVM_DIR/bin/opt -load-pass-plugin ./build/lib/libInjectFuncCall.dylib -passes="inject-func-call" output/input_for_hello.bc -o output/instrumented.bin
# Xcode
$LLVM_DIR/bin/opt -load-pass-plugin ./build/lib/Debug/libInjectFuncCall.dylib -passes="inject-func-call" output/input_for_hello.bc -o output/instrumented.bc -debug
$LLVM_DIR/bin/llvm-dis output/instrumented.bc -o output/instrument.ll
$LLVM_DIR/bin/lli output/instrumented.bc
clang output/instrumented.bc -o output/instrumentx