#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "alloc.h"

/* Tag an integer. */
LispObjectImm
TagInt (intptr_t Integer) {
  return (LispObjectImm) (Integer << 3) | INTEGER_OBJ;
}

/* Untag an integer. */
intptr_t
UntagInt (LispObjectImm Integer) {
  return (intptr_t) Integer >> 3;
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
  FreeCons.TotalAllocations = 0;
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
  FreeCons.TotalAllocations++;
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
  return (struct StringObject*) (String ^ STRING_OBJ);
}

/* Our small strings arena. */
static struct SmallStringObjectArena FreeSmallString;

/* Allocate free small strings arena for our usage. */
void
AllocFreeSmallString () {
  /* size_t = Size, char[SMALL_STRING_SIZE] = String. */
  FreeSmallString.SmallStrings = malloc(SMALL_STRINGS_PER_ARENA * (sizeof(size_t) + sizeof(char) * SMALL_STRING_SIZE));
  FreeSmallString.FreeCount = SMALL_STRINGS_PER_ARENA;
  FreeSmallString.NextFree = FreeSmallString.SmallStrings;
  FreeSmallString.TotalAllocations = 0;
}

/* Get a small string from our small strings arena. */
struct StringObject*
GetSmallString () {
  /* TODO: A PROPER LISP WOULDN'T PANIC & GARBAGE COLLECT INSTEAD!!! */
  if (FreeSmallString.FreeCount < 1) {
    fprintf(stderr, "GetSmallString: Small strings arena has ran out of small strings.");
    exit(2);
  }

  FreeSmallString.FreeCount--;
  FreeSmallString.TotalAllocations++;
  struct StringObject* SmallString = FreeSmallString.NextFree;
  SmallString->Length = 0;
  /* Constant folding EZ. */
  FreeSmallString.NextFree = (struct StringObject*) ((intptr_t) FreeSmallString.NextFree + sizeof(size_t) + sizeof(char) * SMALL_STRING_SIZE);
  return SmallString;
}

/* Make a small string, initialize it with a string text. */
LispObjectImm
MakeSmallString (char* InputString, size_t Length) {
  if (Length > SMALL_STRING_SIZE)
    fprintf(stderr, "Input string of length above %d passed to MakeSmallString", SMALL_STRING_SIZE);
  
  struct StringObject* SmallString = GetSmallString();
  SmallString->Length = Length > 32 ? 32 : Length;

  memcpy(&SmallString->String, InputString, Length > 32 ? 32 : Length);
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
  return (struct SymbolObject*) (Symbol ^ SYMBOL_OBJ);
}

/* Our symbol arena. */
static struct SymbolObjectArena FreeSymbol;

/* Allocate free symbols arena for our usage. */
void
AllocFreeSymbol () {
  FreeSymbol.Symbols = malloc(sizeof(struct SymbolObject) * SYMBOLS_PER_ARENA);
  FreeSymbol.FreeCount = SYMBOLS_PER_ARENA;
  FreeSymbol.NextFree = FreeSymbol.Symbols;
  FreeSymbol.TotalAllocations = 0;
}

/* Get a symbol. */
struct SymbolObject*
GetSymbol () {
  /* TODO: A PROPER LISP WOULDN'T PANIC & GARBAGE COLLECT INSTEAD!!! */
  if (FreeSymbol.FreeCount < 1) {
    fprintf(stderr, "GetSymbol: Small strings arena has ran out of small strings.");
    exit(2);
  }

  FreeSymbol.FreeCount--;
  FreeSymbol.TotalAllocations++;
  struct SymbolObject* Symbol = FreeSymbol.NextFree;
  /* This will be properly initialized forth. */
  Symbol->Name = NULL;
  Symbol->Hash = 0;
  FreeSymbol.NextFree += sizeof(struct SymbolObject);
  return Symbol;
}

/* Make a symbol. */
LispObjectImm
MakeSymbol (char* InputString, size_t Length) {
  /* TODO: ADD HUGESTRING SUPPORT (literally just long strings)!!! */
  if (Length > SMALL_STRING_SIZE)
    fprintf(stderr, "Input string of length above %d passed to MakeSymbol", SMALL_STRING_SIZE);

  struct SymbolObject* Symbol = GetSymbol();
  Symbol->Name = UntagString(MakeSmallString(InputString, Length));
  Symbol->Hash = FNV1AHash(Symbol->Name->String, Symbol->Name->Length);
  return TagSymbol(Symbol);
}

/** TYPE PREDICATES. **/

_Bool
ConsTypeP (LispObjectImm Object) {
  return (Object & TYPEFIELD) == CONS_CELL;
}

_Bool
IntegerTypeP (LispObjectImm Object) {
  return (Object & TYPEFIELD) == INTEGER_OBJ;
}

_Bool
SymbolTypeP (LispObjectImm Object) {
  return (Object & TYPEFIELD) == SYMBOL_OBJ;
}

_Bool
StringTypeP (LispObjectImm Object) {
  return (Object & TYPEFIELD) == STRING_OBJ;
}

/** OBARRAY. **/

static struct ObarrayHashtable* GlobalObarray;

/* Allocate the global obarray, this is what makes symbols useful. */
void
AllocObarray () {
  /* (Size + Fullness) + BucketsArray */
  GlobalObarray = malloc(sizeof(size_t) * 2 + sizeof(struct ObarrayBucket) * INITIAL_OBARRAY_SIZE);
  GlobalObarray->Size = INITIAL_OBARRAY_SIZE;
  GlobalObarray->Fullness = 0;
  for (size_t i = 0; i < GlobalObarray->Size; i++) {
    GlobalObarray->Buckets->Next = NULL;
    GlobalObarray->Buckets->Symbol = NULL;
  }
}

/* Our hash function. */
uintptr_t
FNV1AHash (char* String, size_t Length) {
  /* NOTE: Maybe naming the offset basis constant might be a good idea. */
  uintptr_t Hash = 14695981039346656057UL;

  for (size_t i = 0; i < Length; i++) {
    Hash ^= String[i];
    Hash *= 1099511628211UL;
  }
  return (intptr_t) Hash;
}

/* Insert a symbol into a bucket from the obarray. */
void
InsertIntoBucket (struct ObarrayBucket* Bucket, struct SymbolObject* Symbol) {
  if (!Bucket->Symbol) {
    Bucket->Symbol = Symbol;
    return;
  }
  struct ObarrayBucket* NextBucket = NULL;
  struct ObarrayBucket* PrevBucket = Bucket;

  for (NextBucket = PrevBucket->Next; NextBucket; PrevBucket = NextBucket);

  PrevBucket->Next = malloc(sizeof(struct ObarrayBucket));
  NextBucket = PrevBucket->Next;
  NextBucket->Symbol = Symbol;
  NextBucket->Next = NULL;
}

/* Put the symbol into the obarray so we can fetch it & return it instead of allocating more new symbols. */
void
InternSymbol (struct SymbolObject* Symbol) {
  size_t Place = Symbol->Hash % GlobalObarray->Size;
  InsertIntoBucket(&GlobalObarray->Buckets[Place], Symbol);
  GlobalObarray->Fullness++;
}

/* Lookup a symbol in the obarray.. */
struct SymbolObject*
LookupSymbol (char* String, size_t Length) {
  uintptr_t Hash = FNV1AHash(String, Length);
  struct ObarrayBucket* Bucket = &GlobalObarray->Buckets[Hash % GlobalObarray->Size];
  for (; Bucket; Bucket = Bucket->Next) {
    if (Bucket->Symbol && Bucket->Symbol->Hash == Hash)
      return Bucket->Symbol;
  }
  return NULL;
}

/* Intern or get a symbol named by the input. */
LispObjectImm
FindOrMakeSymbol (char* String, size_t Length) {
  struct SymbolObject* Symbol = LookupSymbol(String, Length);
  if (Symbol)
    return TagSymbol(Symbol);
  Symbol = UntagSymbol(MakeSymbol(String, Length));
  InternSymbol(Symbol);
  return TagSymbol(Symbol);
}

/* Print statistics. */
void
PrintAllocationStatistics (FILE* Stream) {
  fprintf(Stream, "Cons cells allocated: %ld\n", FreeCons.TotalAllocations);
  fprintf(Stream, "Small strings allocated: %ld\n", FreeSmallString.TotalAllocations);
  fprintf(Stream, "Symbols allocated: %ld\n", FreeSymbol.TotalAllocations);
}
