#include "alloc.h"
#include "env.h"
#include "eval.h"

uintptr_t
Length (LispObjectImm List) {
  if (List == QuoteNil)
    return 0;
  uintptr_t Len = 0;
  for (LispObjectImm Cons = List; Cons != QuoteNil; Cons = GetConsCdr(UntagCons(Cons))) {
    Len++;
  }
  return Len;
}

LispObjectImm
ListLength (LispObjectImm Arg) {
  return TagInt(Length(GetConsCar(UntagCons(Arg))));
}

LispObjectImm
ApplyBuiltInFn (LispObjectImm Fn, LispObjectImm Args) {
  struct FnObject* Function = UntagFn(Fn);
  return Function->U.CFn(Args);
}

LispObjectImm
Apply (LispObjectImm Expr) {
  LispObjectImm Fn = GetEnvFn(GetConsCar(UntagCons(Expr)));
  LispObjectImm Args = GetConsCdr(UntagCons(Expr));
  LispObjectImm EvalArgsHead = QuoteNil;
  LispObjectImm EvalArgsTail = QuoteNil;
  /* If there is atleast 1 argument, evaluate it. */
  if (Args != QuoteNil) {
    EvalArgsHead = MakeConsCell(Eval1(GetConsCar(UntagCons(Args))), QuoteNil);
    EvalArgsTail = EvalArgsHead;

    /* Built the evaluated arglist. */
    for (LispObjectImm CurrentArg = GetConsCdr(UntagCons(Args));
	 CurrentArg && CurrentArg != QuoteNil;
	 CurrentArg = GetConsCdr(UntagCons(CurrentArg))) {
      SetConsCdr(UntagCons(EvalArgsTail), MakeConsCell(Eval1(GetConsCar(UntagCons(CurrentArg))), QuoteNil));
      EvalArgsTail = GetConsCdr(UntagCons(EvalArgsTail));
    }
  }

  if (Fn == QuoteNil)
    return QuoteNil;
  if (FnTypeP(Fn) && BuiltInFnP(Fn))
    return ApplyBuiltInFn(Fn, Args != QuoteNil ? EvalArgsHead : QuoteNil);
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
    else if (GetConsCar(UntagCons(Expr)) == QuoteProgn) {
      /* Iterate through all values of the list until the last one, then return that one. */
      LispObjectImm Cons = Expr;
      LispObjectImm Return = QuoteNil;
      for (Cons = Expr; Cons && Cons != QuoteNil; Cons = GetConsCdr(UntagCons(Cons)))
	Return = Eval1(GetConsCar(UntagCons(Cons)));

      return Return;
    }
    else
      return Apply(Expr);
  }
  return QuoteNil;
}
