
setClass("RC++Reference", representation(ref = "externalptr"))

setClass("Module", contains = "RC++Reference")
setClass("IRBuilder", contains = "RC++Reference")
setClass("LLVMContext", contains = "RC++Reference")
setClass("ExecutionEngine", contains = "RC++Reference")
setClass("Value", contains = "RC++Reference")
setClass("BasicBlock", contains = "Value")

setClass("FunctionPassManager", contains = "RC++Reference")


setClass("Instruction", contains = "RC++Reference")
setClass("UnaryInstruction", contains = "RC++Reference")
setClass("TerminatorInst", contains = "Instruction")
setClass("ReturnInst", contains = "TerminatorInst")
setClass("CallInst", contains = "Instruction")
setClass("CastInst", contains = "UnaryInstruction")
setClass("SExtInst", contains = "CastInst")

setClass("BinaryOperator", contains = "Instruction")

setClass("BranchInst", contains = "TerminatorInst")

setClass("ParameterList", contains = "list")

  # allocates memory on the stack.
setClass("AllocaInst", contains = "UnaryInstruction")
setClass("LoadInst", contains = "UnaryInstruction")

setClass("StoreInst", contains = "Instruction")
setClass("GetElementPtrInst", contains = "Instruction")



setClass("Type", contains = "RC++Reference")
setClass("DerivedType", contains = "Type")
setClass("CompositeType", contains = "DerivedType")
setClass("StructType", contains = "CompositeType")
setClass("UnionType", contains = "CompositeType")
setClass("SequentialType", contains = "CompositeType")
setClass("PointerType", contains = "SequentialType")


setClass("User", contains = "Value")
setClass("Constant", contains = "User")
setClass("ConstantInt", contains = "Constant")
setClass("ConstantFP", contains = "Constant")
setClass("GlobalValue", contains = "Constant")
setClass("GlobalVariable", contains = "GlobalValue")

setClass("Function", contains = "GlobalValue")

setClass("NativeFunctionPointer", contains = "RC++Reference")
setClass("NativeGlobalVariable", contains = "RC++Reference")

setTailCall =
function(call, val = TRUE)
{  
   .Call("R_CallInst_setTailCall", call, as.logical(val))
}


