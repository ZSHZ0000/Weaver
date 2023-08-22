#include "alloc.h"
#include "env.h"
#include "eval.h"

LispObjectImm
ApplyBuiltInFn (LispObjectImm Fn, LispObjectImm Args) {
  struct FnObject* Function = UntagFn(Fn);
  return Function->U.CFn(Args);
}

LispObjectImm
Apply (LispObjectImm Expr) {
  LispObjectImm Fn = GetEnvFn(GetConsCar(UntagCons(Expr)));
  if (BuiltInFnP(Fn))
    return ApplyBuiltInFn(GetConsCar(UntagCons(Expr)), GetConsCdr(UntagCons(Expr)));
  return QuoteNil;
}

LispObjectImm
EvalIf (LispObjectImm Expr) {
  LispObjectImm Predicate = GetConsCar(UntagCons(Expr));
  LispObjectImm Consequent = GetConsCar(UntagCons(GetConsCdr(UntagCons(Expr))));
  LispObjectImm Alternate = GetConsCdr(UntagCons(GetConsCdr(UntagCons(Expr))));
  if (Alternate != QuoteNil)
    Alternate = GetConsCar(UntagCons(Alternate));
  if (Eval1(Predicate) != QuoteNil)
    return Eval1(Consequent);
  else if (Alternate != QuoteNil)
    return Eval1(Alternate);
  return QuoteNil;
}

LispObjectImm
Eval1 (LispObjectImm Expr) {
  if (IntegerTypeP(Expr))
    return Expr;
  if (StringTypeP(Expr))
    return Expr;
  if (FnTypeP(Expr))
    return Expr;
  if (SymbolTypeP(Expr))
    return FindEnvVariable(Expr);
  /* If it's not an integer, fn or string then it is a cons cell. */
  if (ConsTypeP(Expr)) {
    if (GetConsCar(UntagCons(Expr)) == QuoteQuote)
      return GetConsCar(UntagCons(GetConsCdr(UntagCons(Expr))));
    else if (GetConsCar(UntagCons(Expr)) == QuoteIf)
      return EvalIf(GetConsCdr(UntagCons(Expr)));
    else
      Apply(Expr);
  }
  return QuoteNil;
}
