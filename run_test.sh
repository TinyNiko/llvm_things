#!/bin/bash
export LLVM_DIR=/Users/nikos/gr/llvm-project/build
# inject
# $LLVM_DIR/bin/clang -emit-llvm -c ./inputs/input_for_hello.c -o output/input_for_hello.bc
# # MakeFile
# # $LLVM_DIR/bin/opt -load-pass-plugin ./build/lib/libInjectFuncCall.dylib -passes="inject-func-call" output/input_for_hello.bc -o output/instrumented.bin
# # Xcode
# $LLVM_DIR/bin/opt -load-pass-plugin ./build/lib/Debug/libInjectFuncCall.dylib -passes="inject-func-call" output/input_for_hello.bc -o output/instrumented.bc -debug
# $LLVM_DIR/bin/llvm-dis output/instrumented.bc -o output/instrument.ll
# $LLVM_DIR/bin/lli output/instrumented.bc
# clang output/instrumented.bc -o output/instrumentx

# staticcallCounter
# $LLVM_DIR/bin/clang -emit-llvm -c ./inputs/input_for_cc.c -o output/input_for_cc.bc
#  # Xcode
# $LLVM_DIR/bin/opt -load-pass-plugin ./build/lib/Debug/libStaticCall.dylib -passes="print<static-cc>" output/input_for_hello.bc -o output/instrumented.bc -debug
# $LLVM_DIR/bin/llvm-dis output/instrumented.bc -o output/instrument.ll

# dynamiccallCounter
# $LLVM_DIR/bin/clang -emit-llvm -c ./inputs/input_for_cc.c -o output/input_for_cc.bc
# # Xcode
# $LLVM_DIR/bin/opt -load-pass-plugin ./build/lib/Debug/libDynCall.dylib -passes="dynamic-cc" output/input_for_cc.bc -o output/instrumented.bc -debug
# $LLVM_DIR/bin/llvm-dis output/instrumented.bc -o output/instrument.ll
# $LLVM_DIR/bin/lli output/instrumented.bc
# clang output/instrumented.bc -o output/instrumentx

# MBASUB
# clang -S -emit-llvm -c ./inputs/input_for_mba_sub.c -o output/input_for_mba_sub.ll
# # Xcode
# $LLVM_DIR/bin/opt -load-pass-plugin ./build/lib/Debug/libMBASUB.dylib -passes="mba-sub1" output/input_for_mba_sub.ll -o output/instrumented.bc -debug
# $LLVM_DIR/bin/llvm-dis output/instrumented.bc -o output/instrument.ll
# # $LLVM_DIR/bin/lli output/instrumented.bc
# clang output/instrumented.bc -o output/instrumentx

# # MBAADD
# clang -emit-llvm -c ./inputs/input_for_mba.c -o output/input_for_mba.bc
# # Xcode
# $LLVM_DIR/bin/opt -load-pass-plugin ./build/lib/Debug/libMBAADD.dylib -passes="mba-add" output/input_for_mba.bc -o output/instrumented.bc -debug
# $LLVM_DIR/bin/llvm-dis output/instrumented.bc -o output/instrument.ll
# # $LLVM_DIR/bin/lli output/instrumented.bc
# # clang output/instrumented.bc -o output/instrumentx

# RIV
# clang -S -emit-llvm -c ./inputs/input_for_riv.c -o output/input_for_riv.ll
# # Xcode
# $LLVM_DIR/bin/opt -load build/lib/Debug/libRIV.dylib -legacy-riv -analyze output/input_for_riv.ll

# DUPBB
clang -emit-llvm -c ./inputs/input_for_duplicate_bb.c -o output/input_for_dup.bc
# # Xcode
$LLVM_DIR/bin/opt -load ./build/lib/Debug/libRIV.dylib -load ./build/lib/Debug/libDuplicateBB.dylib -legacy-duplicate-bb -S output/input_for_dup.bc -o output/instrumented.ll -debug
# $LLVM_DIR/bin/llvm-dis output/instrumented.bc -o output/instrument.ll
# $LLVM_DIR/bin/lli output/instrumented.bc
# clang output/instrumented.bc -o output/instrumentx

# MERGEBB
# clang -emit-llvm -c ./inputs/input_for_mba.c -o output/input_for_mba.bc
# # Xcode
# $LLVM_DIR/bin/opt -load-pass-plugin ./build/lib/Debug/libMBAADD.dylib -passes="mba-add" output/input_for_mba.bc -o output/instrumented.bc -debug
# $LLVM_DIR/bin/llvm-dis output/instrumented.bc -o output/instrument.ll
# # $LLVM_DIR/bin/lli output/instrumented.bc
# # clang output/instrumented.bc -o output/instrumentx