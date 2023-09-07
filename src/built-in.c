#include "alloc.h"
#include "built-in.h"
#include "env.h"
#include "print.h"

void
InitFnEnv() {
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
