#ifndef ALLOC_H
#define ALLOC_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

enum ObjectType {
  CONS_CELL,
  INTEGER_OBJ,
  SYMBOL_OBJ,
  STRING_OBJ,
  FN_OBJ
};

enum FnType {
  NO_FN,
  BUILT_IN_FN,
  INTERP_LAMBDA
};

#define TYPEFIELD (CONS_CELL | INTEGER_OBJ | SYMBOL_OBJ | STRING_OBJ | FN_OBJ)

typedef intptr_t LispObjectImm;

struct SymbolObject {
  struct StringObject* Name;
  uintptr_t Hash;
  LispObjectImm Value;
  LispObjectImm Fn;
};

struct ConsObject {
  LispObjectImm Car;
  LispObjectImm Cdr;
};

struct StringObject {
  size_t Length;
  char String[];
};

struct FnObject {
  enum FnType Type;
  size_t MinArgs;
  size_t MaxArgs;
  union {
    LispObjectImm Obj;
    LispObjectImm (*CFn) (LispObjectImm);
  } U;
};

LispObjectImm
TagInt (intptr_t Integer);

intptr_t
UntagInt (LispObjectImm Integer);

struct ConsObjectArena {
  size_t TotalAllocations;
  size_t FreeCount;
  struct ConsObject* NextFree;
  struct ConsObject* ConsCells;
};

#define CONS_CELLS_PER_ARENA 65536

LispObjectImm
TagCons (struct ConsObject* Cons);

struct ConsObject*
UntagCons (LispObjectImm Cons);

void
SetConsCar (struct ConsObject* ConsCell,  LispObjectImm Value);

void
SetConsCdr (struct ConsObject* ConsCell, LispObjectImm Value);

LispObjectImm
GetConsCar (struct ConsObject* ConsCell);

LispObjectImm
GetConsCdr (struct ConsObject* ConsCell);

void
AllocFreeCons ();

LispObjectImm
MakeConsCell (LispObjectImm Car, LispObjectImm Cdr);

struct SmallStringObjectArena {
  size_t TotalAllocations;
  size_t FreeCount;
  struct StringObject* NextFree;
  struct StringObject* SmallStrings;
};

#define SMALL_STRING_SIZE 32
#define SMALL_STRINGS_PER_ARENA 2048

void
AllocFreeSmallString ();

LispObjectImm
MakeSmallString (char* InputString, size_t Length);

LispObjectImm
TagString (struct StringObject* String);

struct StringObject*
UntagString (LispObjectImm String);

struct SymbolObjectArena {
  size_t TotalAllocations;
  size_t FreeCount;
  struct SymbolObject* NextFree;
  struct SymbolObject* Symbols;
};

#define SYMBOLS_PER_ARENA 2048

LispObjectImm
TagSymbol (struct SymbolObject* Symbol);

struct SymbolObject*
UntagSymbol (LispObjectImm Symbol);

void
AllocFreeSymbol ();

struct SymbolObject*
GetSymbol ();

LispObjectImm
MakeSymbol (char* InputString, size_t Length);

struct FnObjectArena {
  size_t TotalAllocations;
  size_t FreeCount;
  struct FnObject* NextFn;
  struct FnObject* Fns;
};

#define FUNCTIONS_PER_ARENA 128

LispObjectImm
TagFn (struct FnObject* Fn);

struct FnObject*
UntagFn (LispObjectImm Fn);

struct FnObject*
MakeBuiltInFn (LispObjectImm (*FnPtr) (LispObjectImm), size_t MinArgs, size_t MaxArgs);

struct FnObject*
MakeInterpLambda (LispObjectImm Lambda);

void
AllocFreeFn ();

_Bool
ConsTypeP (LispObjectImm Object);

_Bool
IntegerTypeP (LispObjectImm Object);

_Bool
SymbolTypeP (LispObjectImm Object);

_Bool
StringTypeP (LispObjectImm Object);

_Bool
FnTypeP (LispObjectImm Object);

_Bool
BuiltInFnP (LispObjectImm Object);

_Bool
InterpLambdaP (LispObjectImm Object);

#define INITIAL_OBARRAY_SIZE 256

struct ObarrayBucket {
  struct SymbolObject* Symbol;
  struct ObarrayBucket* Next;
};

struct ObarrayHashtable {
  size_t Size;
  size_t Fullness;
  struct ObarrayBucket Buckets[];
};

void
AllocObarray ();

uintptr_t
FNV1AHash (char* String, size_t Length);

void
InternSymbol (struct SymbolObject* Symbol);

LispObjectImm
FindOrMakeSymbol (char* String, size_t Length);

void
PrintAllocationStatistics (FILE* Stream);

/* Lisp interface to the useful calls. */

LispObjectImm
Cons (LispObjectImm Args);

LispObjectImm
Car (LispObjectImm Arg);

LispObjectImm
Cdr (LispObjectImm Arg);

LispObjectImm
RCar (LispObjectImm Args);

LispObjectImm
RCdr (LispObjectImm Args);

LispObjectImm
AllocStatistics (LispObjectImm Args);

#endif /* ALLOC_H */
