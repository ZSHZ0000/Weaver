#include <stdint.h>
#include <stdio.h>
#include "reader.h"
#include "alloc.h"
#include "print.h"
#include "env.h"

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
  if (GetConsCdr(UntagCons(List)) && GetConsCdr(UntagCons(List)) != QuoteNil) {
    for (LispObjectImm Cons = GetConsCdr(UntagCons(List)); Cons && Cons != QuoteNil; Cons = GetConsCdr(UntagCons(Cons))) {
      putc(' ', Stream);
      if (ConsTypeP(Cons))
	PrintObject1(GetConsCar(UntagCons(Cons)), Stream);
      else {
	fprintf(Stream, ". ");
	PrintObject1(Cons, Stream);
	break;
      }
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
  if (FnTypeP(Object)) {
    if (InterpLambdaP(Object)) {
      fprintf(Stream, "#<lambda ");
      /* The lambda arglist.
      **(SYMBOL ARGLIST EXPR) */
      PrintObject1(GetConsCar(UntagCons(GetConsCdr(UntagCons(UntagFn(Object)->U.Obj)))),
		   Stream);
      fprintf(Stream, ">");
    }
    if (BuiltInFnP(Object))
      fprintf(Stream, "#<built-in-fn>");
  }
}

LispObjectImm
Print (LispObjectImm Object) {
  PrintObject1(GetConsCar(UntagCons(Object)), stdout);
  return Object;
}

LispObjectImm
NL (LispObjectImm Object) {
  /* TODO: Perhaps we should use a truth value here. */
  fprintf(stdout ,"\n");
  return QuoteNil;
}
