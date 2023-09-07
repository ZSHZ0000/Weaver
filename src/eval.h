#ifndef EVAL_H
#define EVAL_H

#include "alloc.h"
#include "env.h"

LispObjectImm
ApplyBuiltInFn (LispObjectImm Fn, LispObjectImm Args);

LispObjectImm
Apply (LispObjectImm Expr);

LispObjectImm
EvalIf (LispObjectImm Expr);

LispObjectImm
Eval1 (LispObjectImm Expr);

#endif /* EVAL_H */
