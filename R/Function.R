
Function =
function(name, retType, paramTypes = list(),  module)
{

  fun = .Call("R_createFunction", module, as.character(name), retType, paramTypes)
  
  if(length(names(paramTypes)))
     names(fun) = names(paramTypes)

  fun
}

setMethod("names", c("Function"),
           function(x) {
              .Call("R_getFunctionParamNames", x)
           })

setMethod("names<-", c("Function", "character"),
           function(x, value) {
              params = getParameters(x)
              names(params) = value
              x
           })

setMethod("names<-", c("ParameterList", "character"),
           function(x, value) {
              mapply(setName, x, value)
              x
           })

setGeneric("setName",
function(obj, name, ...)
           standardGeneric("setName"))

setGeneric("getName",
function(obj, ...)
           standardGeneric("getName"))

setMethod("setName", "Value",
          function(obj, name)
              .Call("R_Value_setName", obj, as.character(name)))

setMethod("getName", "Value",
          function(obj, name)
              .Call("R_Value_getName", obj))


getParameters =
function(fun, addNames = TRUE)
{
   els = .Call("R_getFunctionArgs", fun)
   ans = new("ParameterList", els)
   if(addNames)
     names(ans) = lapply(ans, getName)
   ans
}

setMethod("$", "Function",
           function(x, name) {
             getParameters(x)[[name]]
           })


setLinkage =
function(obj, val)
{
  .Call("R_Function_setLinkage", obj, as.integer(val))
}


setGeneric("getModule", function(obj, ...)
                            standardGeneric("getModule"))

setMethod("getModule", "Function", 
           function(obj, ...)
              .Call("R_GlobalValue_getParent", obj))

setAs("Function", "Module",
       function(from)
          getModule(from))

if(FALSE) # not needed
setAs("Function", "Module",
       function(from) {
  	  .Call("R_Function_getParent", from)
       })


getBlocks =
function(fun)
{  
 .Call("R_Function_getBasicBlockList", fun)
}


setGeneric("getReturnType",
            function(obj, ...)
               standardGeneric("getReturnType"))

setMethod("getReturnType",
           "Function",
          function(obj, ...)
             .Call("R_Function_getReturnType", obj))


setMethod("getReturnType",
           "CallInst",
          function(obj, ...)
             .Call("R_Function_getReturnType", getCalledFunction(obj)))


setGeneric("getCalledFunction",
           function(obj, ...)
             standarGeneric("getCalledFunction"))

setMethod("getCalledFunction", "CallInst",
           function(obj, ...)
             .Call("R_CallInst_getCalledFunction", obj))

  

