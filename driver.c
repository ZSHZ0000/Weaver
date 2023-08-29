#include <stdio.h>
#include <stdlib.h>
#include "alloc.h"
#include "built-in.h"
#include "reader.h"
#include "read.h"
#include "print.h"
#include "env.h"
#include "eval.h"

int
main (int argc, char** argv) {
  if (argc < 2) {
    fprintf(stderr, "Please input a file.\n");
    return 1;
  }

  FILE* input = fopen (argv[1], "r");
  if (!input) {
    fprintf(stderr, "File does not exist or is not accessible.\n");
    return 1;
  }

  AllocFreeCons();
  AllocFreeSymbol();
  AllocFreeSmallString();
  AllocFreeFn();
  AllocObarray();

  InitEnvironment();
  InitFnEnv();

  UpdatePeekCharacter(input);
  struct LexemaChain* LexemaChain = ReadLexemaChain(input);
  struct LexemaIndex* LexemaIndex = MakeLexemaIndex(LexemaChain);

  LispObjectImm Object = ReadObject1(LexemaIndex);
  LispObjectImm Result = Eval1(Object);
  fprintf(stdout, "Return value: ");
  PrintObject1(Result, stdout);
  fprintf(stdout, "\n");
  PrintAllocationStatistics(stderr);

  FreeLexemaIndex(LexemaIndex);
  FreeLexemaChain(LexemaChain);
  return 0;
}
