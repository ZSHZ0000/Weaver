#include <sys/cdefs.h>
#include "alloc.h"
#include "env.h"

/* The NIL symbol. */
LispObjectImm QuoteNil;

/* Initialize environment. */
void
InitEnvironment () {
  /* Nil symbol. */
  struct SymbolObject* Nil = GetSymbol();
  Nil->Name = UntagString(MakeSmallString("nil", 3));
  Nil->Hash = FNV1AHash(Nil->Name->String, Nil->Name->Length);
  Nil->Value = TagSymbol(Nil);
  QuoteNil = TagSymbol(Nil);
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

/* Define a new variable. */
void
DefineEnvVariable (LispObjectImm Symbol, LispObjectImm Value) {
  UntagSymbol(Symbol)->Value = Value;
}
