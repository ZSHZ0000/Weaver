#include <sys/cdefs.h>
#include "alloc.h"
#include "env.h"

/* The NIL symbol. */
LispObjectImm QuoteNil;
LispObjectImm QuoteIf;
LispObjectImm QuoteQuote; /* LOL! */
LispObjectImm QuoteProgn;

/* Initialize environment. */
void
InitEnvironment () {
  /* Nil symbol. */
  struct SymbolObject* Nil = GetSymbol();
  Nil->Name = UntagString(MakeSmallString("nil", 3));
  Nil->Hash = FNV1AHash(Nil->Name->String, Nil->Name->Length);
  Nil->Value = TagSymbol(Nil);
  QuoteNil = TagSymbol(Nil);
  InternSymbol(Nil);

  struct SymbolObject* If = UntagSymbol(MakeSymbol("if", 2));
  InternSymbol(If);
  QuoteIf = TagSymbol(If);

  struct SymbolObject* Quote = UntagSymbol(MakeSymbol("quote", 5));
  InternSymbol(Quote);
  QuoteQuote = TagSymbol(Quote);

  struct SymbolObject* Progn = UntagSymbol(MakeSymbol("progn", 5));
  InternSymbol(Progn);
  QuoteProgn = TagSymbol(Progn);
}

/* Return the value of the environment variable. */
LispObjectImm
FindEnvVariable (LispObjectImm Symbol) {
  return UntagSymbol(Symbol)->Value;
}

/* Set the value of the environment variable. */
void
SetEnvVariable (LispObjectImm Symbol, LispObjectImm Value) {
  UntagSymbol(Symbol)->Value = Value;
}

/* Set symbol's function. */
void
SetEnvFn (LispObjectImm Symbol, LispObjectImm Fn) {
  UntagSymbol(Symbol)->Fn = Fn;
}

/* Get symbol's function. */
LispObjectImm
GetEnvFn (LispObjectImm Symbol) {
  return UntagSymbol(Symbol)->Fn;
}

/* Add a C function to the function environment.*/
void
AddCFn (LispObjectImm (*FnPtr) (LispObjectImm), size_t MinArgs, size_t MaxArgs, char* Name, size_t Length) {
  SetEnvFn(FindOrMakeSymbol(Name, Length), TagFn(MakeBuiltInFn(FnPtr, MinArgs, MaxArgs)));
}

/* Set variable named by symbol to the value passed to the function. */
LispObjectImm
SetVar (LispObjectImm Args) {
  LispObjectImm Var = GetConsCar(UntagCons(Args));
  LispObjectImm Value = GetConsCdr(UntagCons(Args));
  if (Value) {
    SetEnvVariable(Var, GetConsCar(UntagCons(Value)));
    return QuoteNil;
  }
  else {
    SetEnvVariable(Var, QuoteNil);
    return QuoteNil;
  }
}

/* Set function named by symbol to the value passed to the function. */
LispObjectImm
SetVarFn (LispObjectImm Args) {
  LispObjectImm Var = GetConsCar(UntagCons(Args));
  LispObjectImm Value = GetConsCdr(UntagCons(Args));
  if (Value) {
    SetEnvFn(Var, GetConsCar(UntagCons(Value)));
    return QuoteNil;
  }
  else {
    SetEnvFn(Var, QuoteNil);
    return QuoteNil;
  }
}
