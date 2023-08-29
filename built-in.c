#include "alloc.h"
#include "built-in.h"
#include "print.h"

void
InitFnEnv() {
  /* Print functions. */
  AddCFn(Print, 1, 1, "print", 5);
  AddCFn(NL, 0, 0, "terpri", 6);

  /* Environment functions. */
  AddCFn(SetVar, 1, 2, "set", 3);
  AddCFn(SetVarFn, 1, 2, "setfn", 5);
}
