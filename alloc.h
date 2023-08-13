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
  STRING_OBJ
};

#define TYPEFIELD (CONS_CELL | INTEGER_OBJ | SYMBOL_OBJ | STRING_OBJ)

struct SymbolObject {
  struct StringObject* Name;
  uintptr_t Hash;
};

typedef intptr_t LispObjectImm;

struct ConsObject {
  LispObjectImm Car;
  LispObjectImm Cdr;
};

struct StringObject {
  size_t Length;
  char String[];
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

LispObjectImm
MakeSymbol (char* InputString, size_t Length);

_Bool
ConsTypeP (LispObjectImm Object);

_Bool
IntegerTypeP (LispObjectImm Object);

_Bool
SymbolTypeP (LispObjectImm Object);

_Bool
StringTypeP (LispObjectImm Object);

#define INITIAL_OBARRAY_SIZE 71

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

#endif /* ALLOC_H */
