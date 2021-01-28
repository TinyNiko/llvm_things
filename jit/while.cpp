#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/Support/TargetSelect.h"
// #include "llvm/IR/TypeBuilder.h"
#include <iostream>
 
using namespace llvm;

int main(){
	static LLVMContext MyGlobalContext;
    LLVMContext &context = MyGlobalContext;
	
	//创建一个module
	Module *module = new Module("test", context);
	IRBuilder<> builder(context);
	
	//-------------------------------------------------------------------------------------
	//声明 int sum(int,n); 函数
	FunctionType *sum_type = FunctionType::get(Type::getInt32Ty(context), {Type::getInt32Ty(context)},false);//TypeBuilder<int(int), false>::get(context);//构建函数签名，包括返回类型和参数
	Function *sum_fun = dyn_cast<Function>(module->getOrInsertFunction("sum", sum_type).getCallee());//将函数插入module
	
	//存储参数（获取参数的引用）
	Function::arg_iterator argsIT = sum_fun->arg_begin();//Function中的一个方法，获取参数的迭代器
	Value *arg_n = argsIT++;//获取第一个参数
	arg_n->setName("n");//设置第一个参数名为a
	
	//创建两个常量值
	Value *con_0 = ConstantInt::get(IntegerType::getInt32Ty(context), 0);
	Value *con_1 = ConstantInt::get(IntegerType::getInt32Ty(context), 1);
	
	//创建max函数的entry代码块
	BasicBlock *entry_sum = BasicBlock::Create(context, "entry", sum_fun);
	builder.SetInsertPoint(entry_sum);
	
	//创建循环使用到的三个代码块
	BasicBlock *while_count = BasicBlock::Create(context, "while_count", sum_fun);
	BasicBlock *while_body = BasicBlock::Create(context, "while_body", sum_fun);
	BasicBlock *while_end = BasicBlock::Create(context, "while_end", sum_fun);
	
	//给变量i和sum申请内存,并放入初值0
	Value* i_alloca = builder.CreateAlloca(Type::getInt32Ty(context));
	Value* sum_alloca = builder.CreateAlloca(Type::getInt32Ty(context));
	builder.CreateStore(con_0, i_alloca);
	builder.CreateStore(con_0, sum_alloca);
	builder.CreateBr(while_count);
	
	//while_count基本块
	builder.SetInsertPoint(while_count);
	Value* i_load = builder.CreateLoad(Type::getInt32Ty(context), i_alloca);
	Value *cmp_value = builder.CreateICmpSLE(i_load, arg_n);
	//根据cmp的值跳转，也就是if条件
	builder.CreateCondBr(cmp_value, while_body, while_end);
	
	//while_body基本块
	builder.SetInsertPoint(while_body);
	//sum = sum + i;
	Value* sum_load = builder.CreateLoad(Type::getInt32Ty(context), sum_alloca);
	Value* temp_sum = builder.CreateAdd(sum_load, i_load);
	builder.CreateStore(temp_sum, sum_alloca);
	//i++;
	Value* temp_i = builder.CreateAdd(i_load, con_1);
	builder.CreateStore(temp_i, i_alloca);
	builder.CreateBr(while_count);
	
	//while_end基本块
	builder.SetInsertPoint(while_end);
	//创建返回值
	Value* ret_sum = builder.CreateLoad(Type::getInt32Ty(context), sum_alloca);
	builder.CreateRet(ret_sum);
	//-------------------------------------------------------------------------------------
	
	//-------------------------------------------------------------------------------------
	//声明 int main() 函数
	FunctionType *main_type = FunctionType::get(Type::getInt32Ty(context), {Type::getInt32Ty(context)}, false);
	Function *main_fun = dyn_cast<Function>(module->getOrInsertFunction("main", main_type).getCallee());
	
	//创建main函数的entry代码块
	BasicBlock *entry_mian = BasicBlock::Create(context, "entry", main_fun);
	
	//创建一个i32常量
	Value *n_value = ConstantInt::get(Type::getInt32Ty(context), 10);
	
	//构造实参列表
	std::vector<Value*> putsargs;
	putsargs.push_back(n_value);
	ArrayRef<Value*>  argsRef(putsargs);
	
	builder.SetInsertPoint(entry_mian);
	
	// 调用函数max
	Value *ret_value = builder.CreateCall(sum_fun, argsRef);
	
	//创建返回值
	builder.CreateRet(ret_value);
	
	module->dump();
	
	InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    InitializeNativeTargetAsmParser();
	
	//创建ExecutionEngine
	ExecutionEngine *ee = EngineBuilder(std::unique_ptr<Module>(module)).setEngineKind(EngineKind::JIT).create();
	
	//生成机器指令
    void *mainAddr = ee->getPointerToFunction(main_fun);
	
	//运行机器指令
    typedef int (*FuncType)();
    FuncType mianFunc = (FuncType)mainAddr;//使用类型转换将mainAddr转换成一个函数mianFunc，然后调用
    ee->finalizeObject();
    std::cout << mianFunc() << std::endl;
    
    delete module;
    return 0;
}