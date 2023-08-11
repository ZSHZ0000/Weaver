#include <stdint.h>
#include <stdio.h>
#include "reader.h"
#include "alloc.h"
#include "print.h"

void
PrintString (LispObjectImm String, FILE* Stream) {
  struct StringObject* TheString = UntagString(String);
  putc('"', Stream);
  for (size_t i = 0; i < TheString->Length; i++) {
    if (TheString->String[i] == '"')
      putc('\\', Stream);
    putc(TheString->String[i], Stream);
  }
  putc('"', Stream);
}

void
PrintSymbol (LispObjectImm Symbol, FILE* Stream) {
  struct SymbolObject* TheSymbol = UntagSymbol(Symbol);
  fprintf(Stream, "%*s", (int) TheSymbol->Name->Length, TheSymbol->Name->String);
}

void
PrintInteger (LispObjectImm Integer, FILE* Stream) {
  fprintf(Stream, "%ld", UntagInt(Integer));
}

void
PrintList (LispObjectImm List, FILE* Stream) {
  putc('(', Stream);
  PrintObject1(GetConsCar(UntagCons(List)), Stream);
  /* TODO: Change the 0 for NIL or empty list or whatever. */
  if (GetConsCdr(UntagCons(List)) != 0) {
    for (LispObjectImm Cons = List; Cons != 0; Cons = GetConsCdr(UntagCons(Cons))) {
      putc(' ', Stream);
      PrintObject1(GetConsCar(UntagCons(Cons)), Stream);
    }
    putc(')', Stream);
  }
  else
    putc(')', Stream);
}

void
PrintObject1 (LispObjectImm Object, FILE* Stream) {
  if (ConsTypeP(Object))
    PrintList(Object, Stream);
  if (IntegerTypeP(Object))
    PrintInteger(Object, Stream);
  if (SymbolTypeP(Object))
    PrintSymbol(Object, Stream);
  if (StringTypeP(Object))
    PrintString(Object, Stream);
}
