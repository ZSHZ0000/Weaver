#ifndef ENV_H
#define ENV_H

#include "alloc.h"

extern LispObjectImm QuoteNil;
extern LispObjectImm QuoteIf;
extern LispObjectImm QuoteQuote;
extern LispObjectImm QuoteProgn;

void
InitEnvironment ();

LispObjectImm
FindEnvVariable (LispObjectImm Symbol);

void
SetEnvVariable (LispObjectImm Symbol, LispObjectImm Value);

void
SetEnvFn (LispObjectImm Symbol, LispObjectImm Fn);

LispObjectImm
GetEnvFn (LispObjectImm Symbol);

void
AddCFn (LispObjectImm (*FnPtr) (LispObjectImm), size_t MinArgs, size_t MaxArgs, char* Name, size_t Length);

LispObjectImm
SetVar (LispObjectImm Args);

LispObjectImm
SetVarFn (LispObjectImm Args);

#endif /* ENV_H */
