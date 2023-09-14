#ifndef EVAL_H
#define EVAL_H

#include "alloc.h"
#include "env.h"

uintptr_t
Length (LispObjectImm List);

LispObjectImm
ListLength (LispObjectImm Arg);

LispObjectImm
ApplyBuiltInFn (LispObjectImm Fn, LispObjectImm Args);

LispObjectImm
Apply (LispObjectImm Expr);

LispObjectImm
EvalIf (LispObjectImm Expr);

LispObjectImm
Eval1 (LispObjectImm Expr);

#endif /* EVAL_H */
