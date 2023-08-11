#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "alloc.h"

/* Tag an integer. */
LispObjectImm
TagInt (intptr_t Integer) {
  return (LispObjectImm) (Integer | INTEGER_OBJ);
}

/* Untag an integer. */
intptr_t
UntagInt (LispObjectImm Integer) {
  return (intptr_t) Integer - INTEGER_OBJ;
}

/* We don't need this but for the sake of future-proofing we have it. */
LispObjectImm
TagCons (struct ConsObject* Cons) {
  return (LispObjectImm) Cons;
}

/* Likewise as above. */
struct ConsObject*
UntagCons (LispObjectImm Cons) {
  return (struct ConsObject*) Cons;
}

/* Unsafely set cell's car. */
void
SetConsCar (struct ConsObject* ConsCell,  LispObjectImm Value) {
  ConsCell->Car = Value;
}

/* Unsafely set cell's cdr. */
void
SetConsCdr (struct ConsObject* ConsCell, LispObjectImm Value) {
  ConsCell->Cdr = Value;
}

/* Unsafely set cell's car. */
LispObjectImm
GetConsCar (struct ConsObject* ConsCell) {
  return ConsCell->Car;
}

/* Unsafely set cell's cdr. */
LispObjectImm
GetConsCdr (struct ConsObject* ConsCell) {
  return ConsCell->Cdr;
}

/* Our cons cells arena. */
static struct ConsObjectArena FreeCons;

/* Allocate a free cons cells arena for usage. */
void
AllocFreeCons () {
  FreeCons.ConsCells = malloc(CONS_CELLS_PER_ARENA * sizeof(struct ConsObject));
  FreeCons.FreeCount = CONS_CELLS_PER_ARENA;
  FreeCons.NextFree = FreeCons.ConsCells;
}

/* Get a cons cell from our FreeCons arena. */
struct ConsObject*
GetConsCell () {
  /* TODO: A PROPER LISP WOULDN'T PANIC & GARBAGE COLLECT INSTEAD!!! */
  if (FreeCons.FreeCount < 1) {
    fprintf(stderr, "Cons cells arena has ran out of cons cells.");
    exit(2);
  }

  FreeCons.FreeCount--;
  struct ConsObject* Cell = FreeCons.NextFree;
  FreeCons.NextFree += sizeof(struct ConsObject);
  return Cell;
}

/* Make a cons cell. */
LispObjectImm
MakeConsCell (LispObjectImm Car, LispObjectImm Cdr) {
  struct ConsObject* Cell = GetConsCell();
  Cell->Car = Car;
  Cell->Cdr = Cdr;
  return TagCons(Cell);
}

LispObjectImm
TagString (struct StringObject* String) {
  return (LispObjectImm) String | STRING_OBJ;
}

struct StringObject*
UntagString (LispObjectImm String) {
  return (struct StringObject*) String - STRING_OBJ;
}

/* Our small strings arena. */
static struct SmallStringObjectArena FreeSmallString;

/* Allocate free small strings arena for our usage. */
void
AllocFreeSmallString () {
  /* size_t = Size, char[SMALL_STRING_SIZE] = String. */
  FreeSmallString.SmallStrings = malloc(sizeof(size_t) * SMALL_STRINGS_PER_ARENA +
				    sizeof(char) * SMALL_STRING_SIZE * SMALL_STRINGS_PER_ARENA);
  FreeSmallString.FreeCount = SMALL_STRINGS_PER_ARENA;
  FreeSmallString.NextFree = FreeSmallString.SmallStrings;
}

/* Get a small string from our small strings arena. */
struct StringObject*
GetSmallString () {
  /* TODO: A PROPER LISP WOULDN'T PANIC & GARBAGE COLLECT INSTEAD!!! */
  if (FreeSmallString.FreeCount < 1) {
    fprintf(stderr, "Small strings arena has ran out of small strings.");
    exit(2);
  }

  FreeSmallString.FreeCount--;
  struct StringObject* SmallString = FreeSmallString.NextFree;
  SmallString->Length = 0;
  /* The pointer is not intended to be used as-is but I rather have it initialized to something. */
  SmallString->String = (char*) ((uintptr_t) SmallString + sizeof(size_t));
  FreeCons.NextFree += sizeof(size_t) + sizeof(char) * SMALL_STRING_SIZE; /* Constant folding EZ. */
  return SmallString;
}

/* Make a small string, initialize it with a string text. */
LispObjectImm
MakeSmallString (char* InputString, size_t Length) {
  if (Length > SMALL_STRING_SIZE)
    fprintf(stderr, "Input string of length above %d passed to MakeSmallString", SMALL_STRING_SIZE);
  
  struct StringObject* SmallString = GetSmallString();
  /* We're using the address we initialized with in the function above. */
  memcpy(SmallString->String, InputString, Length > 32 ? 32 : Length);
  return TagString(SmallString);
}

/* Tag a symbol. */
LispObjectImm
TagSymbol (struct SymbolObject* Symbol) {
  return (LispObjectImm) Symbol | SYMBOL_OBJ;
}

/* Untag a symbol. */
struct SymbolObject*
UntagSymbol (LispObjectImm Symbol) {
  return (struct SymbolObject*) Symbol - SYMBOL_OBJ;
}

/* Our symbol arena. */
static struct SymbolObjectArena FreeSymbol;

/* Allocate free symbols arena for our usage. */
void
AllocFreeSymbol () {
  FreeSymbol.Symbols = malloc(sizeof(struct SymbolObject) * SYMBOLS_PER_ARENA);
  FreeSymbol.FreeCount = SYMBOLS_PER_ARENA;
  FreeSymbol.NextFree = FreeSymbol.Symbols;
}

/* Get a symbol. */
struct SymbolObject*
GetSymbol () {
  /* TODO: A PROPER LISP WOULDN'T PANIC & GARBAGE COLLECT INSTEAD!!! */
  if (FreeSymbol.FreeCount < 1) {
    fprintf(stderr, "Small strings arena has ran out of small strings.");
    exit(2);
  }

  FreeSymbol.FreeCount--;
  struct SymbolObject* Symbol = FreeSymbol.NextFree;
  /* This will be properly initialized forth. */
  Symbol->Name = NULL;
  FreeSymbol.NextFree += sizeof(struct SymbolObject);
  return Symbol;
}

/* Make a symbol. */
LispObjectImm
MakeSymbol (char* InputString, size_t Length) {
  /* TODO: ADD HUGESTRING SUPPORT (literally just long strings)!!! */
  if (Length > SMALL_STRINGS_PER_ARENA)
    fprintf(stderr, "Input string of length above %d passed to MakeSymbol", SMALL_STRING_SIZE);

  struct SymbolObject* Symbol = GetSymbol();
  Symbol->Name = UntagString(MakeSmallString(InputString, Length));
  return TagSymbol(Symbol);
}

/** TYPE PREDICATES. **/

_Bool
ConsTypeP (LispObjectImm Object) {
  return Object | CONS_CELL;
}

_Bool
IntegerTypeP (LispObjectImm Object) {
  return Object | INTEGER_OBJ;
}

_Bool
SymbolTypeP (LispObjectImm Object) {
  return Object | SYMBOL_OBJ;
}

_Bool
StringTypeP (LispObjectImm Object) {
  return Object | STRING_OBJ;
}
