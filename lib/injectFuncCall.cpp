//========================================================================
// FILE:
//    InjectFuncCall.cpp
//
// DESCRIPTION:
//    For each function defined in the input IR module, InjectFuncCall inserts
//    a call to printf (from the C standard I/O library). The injected IR code
//    corresponds to the following function call in ANSI C:
//    ```C
//      printf("(llvm-tutor) Hello from: %s\n(llvm-tutor)   number of arguments: %d\n",
//             FuncName, FuncNumArgs);
//    ```
//    This code is inserted at the beginning of each function, i.e. before any
//    other instruction is executed.
//
//    To illustrate, for `void foo(int a, int b, int c)`, the code added by InjectFuncCall
//    will generated the following output at runtime:
//    ```
//    (llvm-tutor) Hello World from: foo
//    (llvm-tutor)   number of arguments: 3
//    ```
//
// USAGE:
//    1. Legacy pass manager:
//      $ opt -load <BUILD_DIR>/lib/libInjectFuncCall.so `\`
//        --legacy-inject-func-call <bitcode-file>
//    2. New pass maanger:
//      $ opt -load-pass-plugin <BUILD_DIR>/lib/libInjectFunctCall.so `\`
//        -passes=-"inject-func-call" <bitcode-file>
//
// License: MIT
//========================================================================
#include "injectFuncCall.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/IR/LLVMContext.h"

using namespace llvm;

#define DEBUG_TYPE "inject-func-call"

//-----------------------------------------------------------------------------
// InjectFuncCall implementation
//-----------------------------------------------------------------------------
bool InjectFuncCall::runOnModule(Module &M) {
  bool InsertedAtLeastOnePrintf = false;
  LLVMContext &CTX = M.getContext();
  
  // Step1 create a define
  
  PointerType *PrintfArgTy = PointerType::getUnqual(Type::getInt8Ty(CTX));
  FunctionType *PrintfFuncTy = FunctionType::get(IntegerType::getInt32Ty(CTX), PrintfArgTy , true);
  FunctionCallee PrintfCallee = M.getOrInsertFunction("printf", PrintfFuncTy);
  Function *PrintfF = dyn_cast<Function>(PrintfCallee.getCallee());
  PrintfF->setDoesNotThrow();
  PrintfF->addParamAttr(0, Attribute::ReadOnly);
  PrintfF->addParamAttr(0, Attribute::NoCapture);
  
  //Step2 insert global formatstring
  Constant *PrintfStr = ConstantDataArray::getString(CTX, "func %s args: %d \n");
  Constant *PrintfVar = M.getOrInsertGlobal("printfformatstr", PrintfStr->getType());
  GlobalVariable *var = dyn_cast<GlobalVariable>(PrintfVar);
  var->setInitializer(PrintfStr);
  
  //Step3 insert call on in Function
  
  
  for(auto &F: M) {
    if(F.isDeclaration()){
      continue;
    }
    IRBuilder<> Builder(&*F.getEntryBlock().getFirstInsertionPt());
    Constant *FuncName = Builder.CreateGlobalStringPtr(F.getName());
    Value *FormatPtr = Builder.CreatePointerCast(PrintfVar, PrintfArgTy, "formatit");
    Builder.CreateCall(PrintfCallee, {FormatPtr, FuncName, Builder.getInt32(F.arg_size())});
    InsertedAtLeastOnePrintf = true;
//    LLVM_DEBUG(errs()<< "gdooo");
  }

  return InsertedAtLeastOnePrintf;
}

PreservedAnalyses InjectFuncCall::run(llvm::Module &M,
                                       llvm::ModuleAnalysisManager &) {
  bool Changed =  runOnModule(M);

  return (Changed ? llvm::PreservedAnalyses::none()
                  : llvm::PreservedAnalyses::all());
}

bool LegacyInjectFuncCall::runOnModule(llvm::Module &M) {
  bool Changed = Impl.runOnModule(M);

  return Changed;
}

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getInjectFuncCallPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "inject-func-call", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "inject-func-call") {
                    MPM.addPass(InjectFuncCall());
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getInjectFuncCallPluginInfo();
}

//-----------------------------------------------------------------------------
// Legacy PM Registration
//-----------------------------------------------------------------------------
char LegacyInjectFuncCall::ID = 0;

// Register the pass - required for (among others) opt
static RegisterPass<LegacyInjectFuncCall>
    X(/*PassArg=*/"legacy-inject-func-call", /*Name=*/"LegacyInjectFuncCall",
      /*CFGOnly=*/false, /*is_analysis=*/false);
