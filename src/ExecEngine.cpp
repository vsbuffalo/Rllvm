#include "Rllvm.h"

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JIT.h>

#include <llvm/Target/TargetSelect.h>

extern "C"
void
R_InitializeNativeTarget()
{
    llvm::InitializeNativeTarget();
}


extern "C"
SEXP
R_create_ExecutionEngine(SEXP r_module)
{
    std::string errStr;
    llvm::Module *module = GET_REF(r_module, Module);
    llvm::ExecutionEngine *EE = llvm::EngineBuilder(module).setErrorStr(&errStr).setEngineKind(llvm::EngineKind::JIT).create();
    if(!EE) {
        PROBLEM "failed to create execution engine: %s", errStr.c_str()
            ERROR;
    }

    return(R_createRef(EE, "ExecutionEngine"));
}

extern "C"
SEXP 
R_callFunction(SEXP r_fun, SEXP r_args, SEXP r_execEngine)
{
    llvm::Function *fun;
    fun = GET_REF(r_fun, Function);

    int nargs = Rf_length(r_args);
    const llvm::FunctionType *fty = fun->getFunctionType();
    if(nargs != fty->getNumParams()) {
        PROBLEM "incorrect number of arguments to routine"
            ERROR; 
    }

    std::vector<llvm::GenericValue> args(nargs);
    if(nargs > 0) {
        for(int i = 0; i < nargs; i++)
            convertRToGenericValue(&args[i], VECTOR_ELT(r_args, i), fty->getParamType(i));
    }

    llvm::ExecutionEngine *ee = GET_REF(r_execEngine, ExecutionEngine);
    llvm::GenericValue val = ee->runFunction(fun, args);
    // need to return any mutable parameters.
    return(convertGenericValueToR(&val, fun->getReturnType()));
}


extern "C"
SEXP
R_ExecutionEngine_getPointerToFunction(SEXP r_execEngine, SEXP r_func)
{
    llvm::ExecutionEngine *ee = GET_REF(r_execEngine, ExecutionEngine);
    llvm::Function *fun = GET_REF(r_func, Function);
    void *ans = ee->getPointerToFunction(fun);
    
    return(R_createRef(ans, "NativeFunctionPointer", "native symbol"));
}

extern "C"
SEXP
R_ExecutionEngine_getPointerToGlobal(SEXP r_execEngine, SEXP r_var)
{
    llvm::ExecutionEngine *ee = GET_REF(r_execEngine, ExecutionEngine);
    llvm::GlobalValue *var = GET_REF(r_var, GlobalValue);
    void *ans = ee->getPointerToGlobal(var);
    
    return(R_createRef(ans, "NativeGlobalVariable"));
}


extern "C"
SEXP
R_ExecutionEngine_FindFunctionNames(SEXP r_execEngine, SEXP r_id)
{
    llvm::ExecutionEngine *ee = GET_REF(r_execEngine, ExecutionEngine);
    
    llvm::Function *ans = ee->FindFunctionNamed(CHAR(STRING_ELT(r_id, 0)));
    
    return(R_createRef(ans, "Function"));
}

#if 0
extern "C"
SEXP
R_ExecutionEngine_getGlobalValue(SEXP r_gv, SEXP r_convert)
{
    const llvm::GlobalValue *gval = GET_REF(r_gv, GlobalValue);
    return(convertGenericValueToR(gval->getUnderlyingObject(), gval->getType()));
}
#endif
