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

struct SymbolObject {
  struct StringObject* Name;
};

typedef intptr_t LispObjectImm;

struct ConsObject {
  LispObjectImm Car;
  LispObjectImm Cdr;
};

struct StringObject {
  size_t Length;
  char* String;
};

LispObjectImm
TagInt (intptr_t Integer);

intptr_t
UntagInt (LispObjectImm Integer);

struct ConsObjectArena {
  size_t FreeCount;
  struct ConsObject* NextFree;
  struct ConsObject* ConsCells;
};

#define CONS_CELLS_PER_ARENA 65536

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

struct SymbolObjectArena {
  size_t FreeCount;
  struct SymbolObject* NextFree;
  struct SymbolObject* Symbols;
};

#define SYMBOLS_PER_ARENA 2048

void
AllocFreeSymbol ();

LispObjectImm
MakeSymbol (char* InputString, size_t Length);

#endif /* ALLOC_H */
