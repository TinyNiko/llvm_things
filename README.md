LLVM12 TEST

# Build

cmake -G "Xcode"  -DLT_LLVM_INSTALL_DIR=$LLVM_DIR ../

# static_reg.sh

mov code to llvm project.

## LLDB DEBUGE

lldb -- $LLVM_DIR/bin/opt -S -load-pass-plugin <build_dir>/lib/libMBAAdd.dylib -passes=mba-add input_for_mba.ll -o out.ll

## REFERENCE

https://github.com/banach-space/llvm-tutor