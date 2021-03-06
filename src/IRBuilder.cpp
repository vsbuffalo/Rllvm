
#include "Rllvm.h"
#include <llvm/Support/IRBuilder.h>

SEXP
R_createRef(const void *ptr, const char * const className, const char * tag)
{
    SEXP ans, klass;
    PROTECT(klass = MAKE_CLASS(className));
    PROTECT(ans = NEW_OBJECT(klass));
    if(!tag)
        tag = className;
    SET_SLOT(ans, Rf_install("ref"), R_MakeExternalPtr((void*)ptr, Rf_install(tag), R_NilValue));
    UNPROTECT(2);
    return(ans);
}

extern "C"
SEXP
R_new_IRBuilder(SEXP r_block)
{
    llvm::BasicBlock *block;
    block =  llvm::cast<llvm::BasicBlock>  (GET_REF(r_block, BasicBlock)); 
    llvm::IRBuilder<> *ans;
    ans = new llvm::IRBuilder<>(block);
    return(R_createRef(ans, "IRBuilder"));
}

extern "C"
SEXP
R_new_IRBuilder_LLVMContext(SEXP r_ctxt)
{
    llvm::LLVMContext *context;
    context =   (GET_REF(r_ctxt, LLVMContext)); 
    llvm::IRBuilder<> *ans;
    ans = new llvm::IRBuilder<>(*context);
    return(R_createRef(ans, "IRBuilder"));
}


extern "C"
SEXP
R_IRBuilder_SetInsertPoint(SEXP r_builder, SEXP r_block)
{
    llvm::IRBuilder<> *builder;
    builder = GET_REF(r_builder, IRBuilder<>);
    llvm::BasicBlock *block = GET_REF(r_block, BasicBlock);
    builder->SetInsertPoint(block);

    return(R_NilValue);
} 


extern "C"
SEXP
R_IRBuilder_GetInsertBlock(SEXP r_builder)
{
    llvm::IRBuilder<> *builder;
    builder = GET_REF(r_builder, IRBuilder<>);

    llvm::BasicBlock *ans = builder->GetInsertBlock();
    return(R_createRef(ans, "BasicBlock"));
}


extern "C"
SEXP
R_IRBuilder_CreateRetVoid(SEXP r_builder)
{
    llvm::IRBuilder<> *builder;
    builder = GET_REF(r_builder, IRBuilder<>);
    llvm::ReturnInst *ret = builder->CreateRetVoid();
    return(R_createRef(ret, "ReturnInst"));
} 

extern "C"
SEXP
R_IRBuilder_CreateRet(SEXP r_builder, SEXP r_val)
{
    llvm::IRBuilder<> *builder;
    builder = GET_REF(r_builder, IRBuilder<>);
    llvm::Value *val = GET_REF(r_val, Value);
    llvm::ReturnInst *ret = builder->CreateRet(val);
    return(R_createRef(ret, "ReturnInst"));
} 

extern "C"
SEXP
R_IRBuilder_CreateBinOp(SEXP r_builder, SEXP r_op, SEXP r_lhs, SEXP r_rhs, SEXP r_id)
{
    llvm::IRBuilder<> *builder;
    builder = GET_REF(r_builder, IRBuilder<>);
    llvm::Value *lhs = GET_REF(r_lhs, Value);
    llvm::Value *rhs = GET_REF(r_rhs, Value);
    if(!lhs || !rhs) {
       PROBLEM "NULL value passed to binary op"
         ERROR;
    }     
    
    llvm::Instruction::BinaryOps op = (llvm::Instruction::BinaryOps) INTEGER(r_op)[0];

    llvm::Value *ret = builder->CreateBinOp(op, lhs, rhs); 

    if(Rf_length(r_id)) {
        ret->setName(makeTwine(r_id));
    }
   
    return(R_createRef(ret, "Value"));
} 

extern "C"
SEXP
R_IRBuilder_CreateCall(SEXP r_builder, SEXP r_fun, SEXP r_args, SEXP r_id)
{
    llvm::IRBuilder<> *builder;
    builder = GET_REF(r_builder, IRBuilder<>);

    llvm::Value *callee = GET_REF(r_fun, Value);

    int nargs = Rf_length(r_args);
    llvm::CallInst *ans;

    if(nargs) {
        std::vector<llvm::Value *> args; // does this disappear and we lose the elements?
        for(int i = 0; i < nargs; i++)
            args.push_back(GET_REF(VECTOR_ELT(r_args, i), Value));
        ans = builder->CreateCall(callee, args.begin(), args.end()); // Use the name.
    } else {
        ans = builder->CreateCall(callee);
    }

    if(Rf_length(r_id))
        ans->setName(makeTwine(r_id));

    return(R_createRef(ans, "CallInst"));
}

extern "C"
SEXP
R_IRBuilder_CreateBr(SEXP r_builder, SEXP r_block)
{
    llvm::IRBuilder<> *builder;
    builder = GET_REF(r_builder, IRBuilder<>);
    llvm::BasicBlock *block = GET_REF(r_block, BasicBlock);

    llvm::BranchInst *ans = builder->CreateBr(block);
    return(R_createRef(ans, "BranchInst"));
}

extern "C"
SEXP
R_IRBuilder_CreateCondBr(SEXP r_builder, SEXP r_cond, SEXP r_true, SEXP r_false)
{
    llvm::IRBuilder<> *builder;
    builder = GET_REF(r_builder, IRBuilder<>);
    llvm::Value *cond = GET_REF(r_cond, Value);
    llvm::BasicBlock *block1 = GET_REF(r_true, BasicBlock);
    llvm::BasicBlock *block2 = GET_REF(r_false, BasicBlock);

    llvm::BranchInst *ans = builder->CreateCondBr(cond, block1, block2);

    return(R_createRef(ans, "BranchInst"));
}


extern "C"
SEXP
R_IRBuilder_CreateNot(SEXP r_builder, SEXP r_val)
{
    llvm::IRBuilder<> *builder;
    builder = GET_REF(r_builder, IRBuilder<>);
    llvm::Value *val = GET_REF(r_val, Value);
    llvm::Value * ans = builder->CreateNot(val);
    return(R_createRef(ans, "Value"));
}

extern "C"
SEXP
R_IRBuilder_CreateNeg(SEXP r_builder, SEXP r_val)
{
    llvm::IRBuilder<> *builder;
    builder = GET_REF(r_builder, IRBuilder<>);
    llvm::Value *val = GET_REF(r_val, Value);
    llvm::Value * ans = builder->CreateNeg(val);
    return(R_createRef(ans, "Value"));
}

extern "C"
SEXP
R_IRBuilder_CreateFNeg(SEXP r_builder, SEXP r_val)
{
    llvm::IRBuilder<> *builder;
    builder = GET_REF(r_builder, IRBuilder<>);
    llvm::Value *val = GET_REF(r_val, Value);
    llvm::Value * ans = builder->CreateFNeg(val);
    return(R_createRef(ans, "Value"));
}

extern "C"
SEXP
R_IRBuilder_CreateUIToFP(SEXP r_builder, SEXP r_val, SEXP r_type)
{
    llvm::IRBuilder<> *builder;
    builder = GET_REF(r_builder, IRBuilder<>);
    llvm::Value *val = GET_REF(r_val, Value);
    llvm::Type *type = GET_REF(r_type, Type);

    llvm::Value * ans = builder->CreateUIToFP(val, type);

    return(R_createRef(ans, "Value"));
}


extern "C"
SEXP
R_IRBuilder_CreateFPToSI(SEXP r_builder, SEXP r_val, SEXP r_type)
{
    llvm::IRBuilder<> *builder;
    builder = GET_REF(r_builder, IRBuilder<>);
    llvm::Value *val = GET_REF(r_val, Value);
    llvm::Type *type = GET_REF(r_type, Type);

    llvm::Value * ans = builder->CreateFPToSI(val, type);

    return(R_createRef(ans, "Value"));
}

extern "C"
SEXP
R_IRBuilder_CreateFPToUI(SEXP r_builder, SEXP r_val, SEXP r_type)
{
    llvm::IRBuilder<> *builder;
    builder = GET_REF(r_builder, IRBuilder<>);
    llvm::Value *val = GET_REF(r_val, Value);
    llvm::Type *type = GET_REF(r_type, Type);

    llvm::Value * ans = builder->CreateFPToUI(val, type);

    return(R_createRef(ans, "Value"));
}


extern "C"
SEXP
R_IRBuilder_CreateSIToFP(SEXP r_builder, SEXP r_val, SEXP r_type)
{
    llvm::IRBuilder<> *builder;
    builder = GET_REF(r_builder, IRBuilder<>);
    llvm::Value *val = GET_REF(r_val, Value);
    llvm::Type *type = GET_REF(r_type, Type);

    llvm::Value * ans = builder->CreateSIToFP(val, type);

    return(R_createRef(ans, "Value"));
}



extern "C"
SEXP
R_IRBuilder_CreateICmp(SEXP r_builder, SEXP r_op, SEXP r_lhs, SEXP r_rhs)
{
    llvm::IRBuilder<> *builder;
    builder = GET_REF(r_builder, IRBuilder<>);
    
    llvm::Value *lhs = GET_REF(r_lhs, Value);
    llvm::Value *rhs = GET_REF(r_rhs, Value);
    llvm::ICmpInst::Predicate op = (llvm::ICmpInst::Predicate) INTEGER(r_op)[0];

    llvm::Value* ans = builder->CreateICmp(op, lhs, rhs);

    return(R_createRef(ans, "Value"));
}

extern "C"
SEXP
R_IRBuilder_CreateFCmp(SEXP r_builder, SEXP r_op, SEXP r_lhs, SEXP r_rhs)
{
    llvm::IRBuilder<> *builder;
    builder = GET_REF(r_builder, IRBuilder<>);
    
    llvm::Value *lhs = GET_REF(r_lhs, Value);
    llvm::Value *rhs = GET_REF(r_rhs, Value);
    llvm::FCmpInst::Predicate op = (llvm::FCmpInst::Predicate) INTEGER(r_op)[0];

    llvm::Value* ans = builder->CreateFCmp(op, lhs, rhs);

    return(R_createRef(ans, "Value"));
}


extern "C"
SEXP
R_IRBuilder_CreateStore(SEXP r_builder, SEXP r_val, SEXP r_ptr, SEXP r_isVolatile)
{
    llvm::IRBuilder<> *builder;
    builder = GET_REF(r_builder, IRBuilder<>);

    llvm::Value *val = GET_REF(r_val, Value);
    llvm::Value *ptr = GET_REF(r_ptr, Value);

    llvm::StoreInst *ans;
    ans = builder->CreateStore(val, ptr, LOGICAL(r_isVolatile)[0]);

    return(R_createRef(ans, "StoreInst"));

}

extern "C"
SEXP
R_IRBuilder_CreateLoad(SEXP r_builder, SEXP r_val, SEXP r_isVolatile, SEXP r_id)
{
    llvm::IRBuilder<> *builder;
    builder = GET_REF(r_builder, IRBuilder<>);

    llvm::Value *val = GET_REF(r_val, Value);

    if(!val) {
        PROBLEM "NULL pointer passed to CreateLoad for the value"
            ERROR;
    }
    llvm::LoadInst *ans;
    ans = builder->CreateLoad(val, LOGICAL(r_isVolatile)[0]);

    if(Rf_length(r_id))
        ans->setName(makeTwine(r_id));

    return(R_createRef(ans, "StoreInst"));

}



extern "C"
SEXP
R_IRBuilder_CreateGEP(SEXP r_builder, SEXP r_val, SEXP r_idx, SEXP r_id)
{
    llvm::IRBuilder<> *builder;
    builder = GET_REF(r_builder, IRBuilder<>);

    llvm::Value *val = GET_REF(r_val, Value);
    llvm::Value *idx = GET_REF(r_idx, Value);

    llvm::Value *ans;
    try {	
       ans = builder->CreateGEP(val, idx);
    } catch(std::exception &e) {
	PROBLEM "failed to create GEP"
	  ERROR;
    }

    if(Rf_length(r_id))
        ans->setName(makeTwine(r_id));

    return(R_createRef(ans, "Value"));
}


extern "C"
SEXP
R_IRBuilder_createLocalVariable(SEXP r_builder, SEXP r_type, SEXP r_size, SEXP r_id)
{
    llvm::IRBuilder<> *builder;
    builder = GET_REF(r_builder, IRBuilder<>);

    llvm::Type *type = GET_TYPE(r_type);
    llvm::AllocaInst *ans;
    
    if(Rf_length(r_size)) {
        llvm::Value *size = GET_REF(r_size, Value);
        ans = new llvm::AllocaInst(type, size, makeTwine(r_id));
    } else
        ans = new llvm::AllocaInst(type, makeTwine(r_id));            

    ans = builder->Insert(ans);

    if(Rf_length(r_id))
        ans->setName(makeTwine(r_id));

    return(R_createRef(ans, "AllocaInst"));

}


extern "C"
SEXP
R_IRBuilder_CreateSExt(SEXP r_builder, SEXP r_val, SEXP r_type, SEXP r_id)
{
    llvm::IRBuilder<> *builder;
    builder = GET_REF(r_builder, IRBuilder<>);

    llvm::Value *val = GET_REF(r_val, Value);
    llvm::Type *type = GET_TYPE(r_type);	

    llvm::Value *ans;
     ans = builder->CreateSExt(val, type);

    if(Rf_length(r_id))
        ans->setName(makeTwine(r_id));

    return(R_createRef(ans, "Value"));
}

extern "C"
SEXP
R_IRBuilder_CreateBitCastInst(SEXP r_builder, SEXP r_val, SEXP r_type, SEXP r_id)
{
    llvm::IRBuilder<> *builder;
    builder = GET_REF(r_builder, IRBuilder<>);
    llvm::Value *val =  GET_REF(r_val, Value);
    llvm::Type *type =  GET_TYPE(r_type);
  
    llvm::Value* ans = builder->CreateBitCast(val, type);

    if(Rf_length(r_id))
        ans->setName(makeTwine(r_id));

    return(R_createRef(ans, "CastInst"));
}


extern "C"
SEXP
R_IRBuilder_CreateGlobalString(SEXP r_builder, SEXP r_val, SEXP r_id)
{
    llvm::IRBuilder<> *builder;
    llvm::Value *ans;

    ans = builder->CreateGlobalString(CHAR(STRING_ELT(r_val, 0)));

    if(Rf_length(r_id))
        ans->setName(makeTwine(r_id));
    return(R_createRef(ans, "Value"));
}



extern "C"
SEXP
R_IRBuilder_CreateIsNull(SEXP r_builder, SEXP r_val, SEXP r_id)
{
    llvm::IRBuilder<> *builder;
    builder = GET_REF(r_builder, IRBuilder<>);
    llvm::Value *val =  GET_REF(r_val, Value);
  
    llvm::Value* ans = builder->CreateIsNull(val);

    if(Rf_length(r_id))
        ans->setName(makeTwine(r_id));

    return(R_createRef(ans, "Value"));
}


extern "C"
SEXP
R_IRBuilder_CreateIsNotNull(SEXP r_builder, SEXP r_val, SEXP r_id)
{
    llvm::IRBuilder<> *builder;
    builder = GET_REF(r_builder, IRBuilder<>);
    llvm::Value *val =  GET_REF(r_val, Value);
  
    llvm::Value* ans = builder->CreateIsNotNull(val);

    if(Rf_length(r_id))
        ans->setName(makeTwine(r_id));

    return(R_createRef(ans, "Value"));
}


extern "C"
SEXP
R_IRBuilder_CreateUnwind(SEXP r_builder, SEXP r_id)
{
    llvm::IRBuilder<> *builder;
    llvm::UnwindInst *ans = builder->CreateUnwind();
    return(R_createRef(ans, "UnwindInst"));
}

extern "C"
SEXP
R_IRBuilder_CreateUnreachable(SEXP r_builder, SEXP r_id)
{
    llvm::IRBuilder<> *builder;
    llvm::UnreachableInst *ans = builder->CreateUnreachable();
    return(R_createRef(ans, "UnreachableInst"));
}

extern "C"
SEXP
R_IRBuilder_CreateSelect(SEXP r_builder, SEXP r_cond, SEXP r_lhs, SEXP r_rhs, SEXP r_id)
{
    llvm::IRBuilder<> *builder;
    builder = GET_REF(r_builder, IRBuilder<>);
    llvm::Value *cond = GET_REF(r_cond, Value);
    llvm::Value *lhs = GET_REF(r_lhs, Value);
    llvm::Value *rhs = GET_REF(r_rhs, Value);

    llvm::Value *ret = builder->CreateSelect(cond, lhs, rhs); 

    if(Rf_length(r_id)) 
        ret->setName(makeTwine(r_id));
   
    return(R_createRef(ret, "Value"));
} 




extern "C"
SEXP
R_BinaryOperator_CreateNeg(SEXP r_value, SEXP r_id, SEXP r_type, SEXP r_block)
{
    llvm::Value *val = GET_REF(r_value, Value);
    llvm::Type *type = GET_REF(r_type, Type);
    llvm::BasicBlock *block = NULL;
    if(Rf_length(r_block))
        block = GET_REF(r_block, BasicBlock);

    llvm::BinaryOperator *op;
    llvm::Type::TypeID ty = type->getTypeID();

    switch(ty) {
        case llvm::Type::IntegerTyID:
            if(block)
               op = llvm::BinaryOperator::CreateNeg(val, makeTwine(r_id), block);
            else
               op = llvm::BinaryOperator::CreateNeg(val);
            break;
        case llvm::Type::FloatTyID:
        case llvm::Type::DoubleTyID:
            if(block)
               op = llvm::BinaryOperator::CreateFNeg(val, makeTwine(r_id), block);
            else
               op = llvm::BinaryOperator::CreateFNeg(val);
            break;
            /*XXX what is the logical type? */
        default:
            if(block)
               op = llvm::BinaryOperator::CreateNot(val, makeTwine(r_id), block);
            else
               op = llvm::BinaryOperator::CreateNot(val);
            break;
    }

    if(!block && Rf_length(r_id)) 
        op->setName(makeTwine(r_id));

    return(R_createRef(op, "BinaryOperator"));
}



extern "C"
SEXP
R_IRBuilder_getTrue(SEXP r_builder)
{
    llvm::IRBuilder<> *builder;
    builder = GET_REF(r_builder, IRBuilder<>);
    return(R_createRef(builder->getTrue(), "ConstantInt"));
}

extern "C"
SEXP
R_IRBuilder_getFalse(SEXP r_builder)
{
    llvm::IRBuilder<> *builder;
    builder = GET_REF(r_builder, IRBuilder<>);
    return(R_createRef(builder->getFalse(), "ConstantInt"));
}


extern "C"
SEXP
R_IRBuilder_getCurrentFunctionReturnType(SEXP r_builder)
{
    llvm::IRBuilder<> *builder;
    builder = GET_REF(r_builder, IRBuilder<>);
    return(R_createRef(builder->getCurrentFunctionReturnType(), "Type"));
}


