#ifndef ENV_H
#define ENV_H

#include "alloc.h"

extern LispObjectImm QuoteNil;
extern LispObjectImm QuoteIf;
extern LispObjectImm QuoteQuote;

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

#endif /* ENV_H */
