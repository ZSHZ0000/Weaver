#include "alloc.h"
#include "built-in.h"
#include "print.h"

void
InitFnEnv() {
  AddCFn(Print, 1, 1, "print", 5);
  AddCFn(NL, 0, 0, "terpri", 6);
}
