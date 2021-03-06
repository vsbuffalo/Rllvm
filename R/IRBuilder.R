IRBuilder =
function(block = getGlobalContext())
{
    # if no block, use the context
   if(is(block, "LLVMContext"))
      .Call("R_new_IRBuilder_LLVMContext", block)     
   else
     .Call("R_new_IRBuilder", block)
}

setInsertPoint =
function(builder, block)
{
  .Call("R_IRBuilder_SetInsertPoint", builder, block)
}

binOp =
function(builder, op, x, y, id = character())
{
     # incomplete. The idea is to recognize symbols in R such as + or - and
     # map those to the corresponding enum values.
   fun = deparse(substitute(op))
   if(is.function(op))
      op = fun

   if(isBasicType(x))
     x = makeConstant(builder, x)
   if(isBasicType(y))
     y = makeConstant(builder, y)

   .Call("R_IRBuilder_CreateBinOp", builder, as.integer(op), x, y, as.character(id))
}

createReturn = createRet =
function(builder, value = NULL)
{
   if(missing( value ))
      createReturnVoid(builder)
   else
      .Call("R_IRBuilder_CreateRet", builder, value)
}

createReturnVoid = createRetVoid =
function(builder)
{
  .Call("R_IRBuilder_CreateRetVoid", builder)
}


createCall =
function(builder, fun, ..., .args = list(...), id = character())
{
  .Call("R_IRBuilder_CreateCall", builder, fun, .args, as.character(id))
}    


createBr =
function(builder, block)
{
  .Call("R_IRBuilder_CreateBr", builder, block)
}

createCondBr =
function(builder, cond, true, false)
{
  .Call("R_IRBuilder_CreateCondBr", builder, cond, true, false)
}


createNeg =
function(builder, val, id = character())
{
  .Call("R_IRBuilder_CreateNeg", builder, val)
}


createFNeg =
function(builder, val, id = character())
{
  .Call("R_IRBuilder_CreateFNeg", builder, val)
}


createNot =
function(builder, val, id = character())
{
  .Call("R_IRBuilder_CreateNot", builder, val)
}


createUIToFP =
function(builder, val, type, id = character())
{
  .Call("R_IRBuilder_CreateUIToFP", builder, val, type)
}


createFPToSI =
function(builder, val, type, id = character())
{
  .Call("R_IRBuilder_CreateFPToSI", builder, val, type)
}

createFPToUI =
function(builder, val, type, id = character())
{
  .Call("R_IRBuilder_CreateFPToUI", builder, val, type)
}

createSIToFP =
function(builder, val, type, id = character())
{
  .Call("R_IRBuilder_CreateSIToFP", builder, val, type)
}

createICmp =
function(builder, op, lhs, rhs, id = character())
{
  .Call("R_IRBuilder_CreateICmp", builder, as(op, "integer"), lhs, rhs)
}

createFCmp =
function(builder, op, lhs, rhs, id = character())
{
  .Call("R_IRBuilder_CreateFCmp", builder, as(op, "integer"), lhs, rhs)
}

isBasicType =
function(val)
{
  typeof(val) %in% c("integer", "logical", "character", "double")
}

createStore =
function(builder, val, ptr, volatile = FALSE)
{
  if(isBasicType(val))  # make a constant.
      val = makeConstant(builder, val)

  .Call("R_IRBuilder_CreateStore", builder, val, ptr, as.logical(volatile))
}

createLoad =
function(builder, val, isVolatile = FALSE, id = character())
{
  .Call("R_IRBuilder_CreateLoad", builder, val, as.logical(isVolatile), as.character(id))
}


createGEP =
function(builder, val, index, id = character())
{
  if(isBasicType(index))
     index = makeConstant(builder, index)

  .Call("R_IRBuilder_CreateGEP", builder, val, index, as.character(id))
}


createLocalVariable =
  #XXX doesn't match method in IRBuilder.  Building myself. Be suspicious
function(builder, type, id)
{
  .Call("R_IRBuilder_createLocalVariable", builder, type, integer(), as.character(id))
}

createLocalArrayVariable =
  #XXX doesn't match method in IRBuilder.  Building myself. Be suspicious
  # size is a Value.  If a constant, we should create the value.
function(builder, type, id, size = NULL)
{
#   if(size <= 0)
#     stop("Must be positive value for size of array")
   
  .Call("R_IRBuilder_createLocalVariable", builder, type, size, as.character(id))
}

##############
#  syntactic sugar to allow calls such as irbuilder$createGEP(...)
#    bldr$binOp(...)
# rather than having the builder as the first argument.
# This makes it easier to follow along with C++ code examples.
#

setMethod("$", "IRBuilder",
           function(x, name) {
              fun = get(name)
              
              # formals(fun)$builder = x
              function(...)
                 fun(builder = x, ...)
           })


createSExt =
function(builder, val, type, id = character())
{
   if(is(type, "numeric"))
      type =  switch(as.character(type),
                     "32" = Int32Type,
                     "64" = Int64Type)
   .Call("R_IRBuilder_CreateSExt", builder, val, type, as.character(id))
}

createBitCast =
function(builder, val, type, id = character())
{
   .Call("R_IRBuilder_CreateBitCastInst", builder, val, type, as.character(id))
}


createGlobalString =
function(builder, val, id = character())
{
  .Call("R_IRBuilder_CreateGlobalString", builder, as.character(val), as.character(id))
}

createIsNull =
function(builder, val, id = character())
{
  .Call("R_IRBuilder_CreateIsNull", builder, val, as.character(id))
}


createIsNotNull =
function(builder, val, id = character())
{
  .Call("R_IRBuilder_CreateIsNull", builder, val, as.character(id))
}

createUnwind  =
function(builder, id = character())
{
  .Call("R_IRBuilder_CreateUnwind", builder,  as.character(id))
}

createUnreachable  =
function(builder, id = character())
{
  .Call("R_IRBuilder_CreateUnreachable", builder,  as.character(id))
}


createSelect  =
function(builder, cond, true, false, id = character())
{
  .Call("R_IRBuilder_CreateSelect", builder, cond, true, false, as.character(id))
}


getInsertBlock =
function(builder)
{
  .Call("R_IRBuilder_GetInsertBlock", builder)
}

setGeneric("createNot",
           function(val, id = character(), block = NULL, builder = NULL)
             standardGeneric("createNot"))

if(FALSE) {
setMethod("createNot", c("Value", block = "missing"),
           function(val, id = character(), block = NULL)
              .Call("R_BinaryOperator_CreateNot", val, as.character(id), VoidType, block))

setMethod("createNot", c("Value", block = "BasicBlock"),
           function(val, id = character(), block = NULL) {
              if(length(id) == 0)
                  id = ""
              .Call("R_BinaryOperator_CreateNot", val, as.character(id), VoidType, block)
            })
}

setMethod("createNot", c("Value", block = "missing", builder = "IRBuilder"),
           function(val, id = character(), block = NULL, builder = NULL)
              .Call("R_IRBuilder_CreateNot", builder, val))

setMethod("createNot", c("Value", block = "BasicBlock", builder = "IRBuilder"),
           function(val, id = character(), block = NULL, builder = NULL) {
              if(length(id) == 0)
                  id = ""
              .Call("R_BinaryOperator_CreateNot", builder, val)
            })


setGeneric("createNeg",
           function(val, id = character(), block = NULL)
             standardGeneric("createNeg"))

setMethod("createNeg", c("Value", block = "missing"),
           function(val, id = character(), block = NULL)
              .Call("R_BinaryOperator_CreateNeg", val, as.character(id), getType(val), NULL))

setMethod("createNeg", c("Value", block = "BasicBlock"),
           function(val, id = character(), block = NULL) {
              if(length(id) == 0)
                  id = ""
                 
              .Call("R_BinaryOperator_CreateNeg", val, as.character(id), getType(val), block)
            })




getFalse =
function(ir)
{
  .Call("R_IRBuilder_getFalse", ir)
}

getTrue =
function(ir)
{
  .Call("R_IRBuilder_getTrue", ir)
}
