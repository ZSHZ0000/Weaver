#include <sys/cdefs.h>
#include "alloc.h"
#include "env.h"
#include "print.h"
#include "read.h"

/* The NIL symbol. */
LispObjectImm QuoteNil;
LispObjectImm QuoteIf;
LispObjectImm QuoteQuote; /* LOL! */

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
