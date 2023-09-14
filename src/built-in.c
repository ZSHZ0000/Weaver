#include "alloc.h"
#include "built-in.h"
#include "env.h"
#include "eval.h"
#include "print.h"

LispObjectImm
IntAddition (LispObjectImm Operands) {
  switch (Length(Operands)) {
  case 0:
    return TagInt(0);
  case 1:
    return GetConsCar(UntagCons(Operands));
  default: {
    intptr_t Result = 0;
    for (LispObjectImm Cons = Operands; Cons && Cons != QuoteNil;
         Cons = GetConsCdr(UntagCons(Cons)))
      Result += UntagInt(GetConsCar(UntagCons(Cons)));
    return TagInt(Result);
  }
  }
}

LispObjectImm
IntSubstraction (LispObjectImm Operands) {
  switch (Length(Operands)) {
  case 0:
    return TagInt(0);
  case 1:
    return GetConsCar(UntagCons(Operands));
  default: {
    intptr_t Result = 0;
    for (LispObjectImm Cons = Operands; Cons && Cons != QuoteNil;
         Cons = GetConsCdr(UntagCons(Cons)))
      Result -= UntagInt(GetConsCar(UntagCons(Cons)));
    return TagInt(Result);
  }
  }
}

LispObjectImm
IntMultiplication (LispObjectImm Operands)
{
  switch (Length(Operands)) {
  case 0:
    return TagInt(0);
  case 1:
    return GetConsCar(UntagCons(Operands));
  default: {
    intptr_t Result = 0;
    for (LispObjectImm Cons = Operands; Cons && Cons != QuoteNil;
         Cons = GetConsCdr(UntagCons(Cons)))
      Result *= UntagInt(GetConsCar(UntagCons(Cons)));
    return TagInt(Result);
  }
  }
}

LispObjectImm
IntDivision (LispObjectImm Operands) {
  switch (Length(Operands)) {
  case 0:
    return TagInt(0);
  case 1:
    return GetConsCar(UntagCons(Operands));
  default: {
    intptr_t Result = 0;
    for (LispObjectImm Cons = Operands; Cons && Cons != QuoteNil;
         Cons = GetConsCdr(UntagCons(Cons)))
      Result /= UntagInt(GetConsCar(UntagCons(Cons)));
    return TagInt(Result);
  }
  }
}

void
InitFnEnv() {
  /* Basic arithmetic functions. */
  AddCFn(IntAddition, 0, -1, "+", 1);
  AddCFn(IntSubstraction, 0, -1, "-", 1);
  AddCFn(IntMultiplication, 0, -1, "*", 1);
  AddCFn(IntDivision, 0, -1, "/", 1);

  /* Length function. */
  AddCFn(ListLength, 1, 1, "length", 6);

  /* Print functions. */
  AddCFn(Print, 1, 1, "print", 5);
  AddCFn(NL, 0, 0, "terpri", 6);

  /* Environment functions. */
  AddCFn(SetVar, 1, 2, "set", 3);
  AddCFn(SetVarFn, 1, 2, "setfn", 5);

  /* Cons cell functions. */
  AddCFn(Cons, 2, 2, "cons", 4);
  AddCFn(Car, 1, 1, "car", 3);
  AddCFn(Cdr, 1, 1, "cdr", 3);
  AddCFn(RCar, 2, 2, "rcar!", 5);
  AddCFn(RCdr, 2, 2, "rcdr!", 5);

  /* Alloc statistics. */
  AddCFn(AllocStatistics, 0, 0, "memstats", 8);
}
