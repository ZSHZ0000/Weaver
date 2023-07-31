#include "reader.h"
#include "alloc.h"

/* Read a string lexema into an actual string. */
LispObjectImm
ReadStringType (struct Lexema* Lexema) {
  /* TODO: Make this work with bigstrings. */
  return MakeSmallString(Lexema->String, Lexema->Length);
}

/* Read a text lexema into an actual symbol. */
LispObjectImm
ReadSymbolType (struct Lexema* Lexema) {
  /* TODO: We need an obarray. */
  return MakeSymbol(Lexema->String, Lexema->Length);
}

/* Parse an integer string into an actual integer. */
intptr_t
ParseInteger (char* InputString, size_t Length) {
  intptr_t Integer = 0;
  for (size_t Idx = 0; Idx < Length; Idx++) {
    if (IsNumeric(InputString[Idx])) {
      Integer *= 10;
      Integer += InputString[Idx] - '0';
    }
    else {
      Integer = 0;
      break;
    }
  }

  return Integer;
}

/* Read a number lexema into an actual number. */
LispObjectImm
ReadNumberType (struct Lexema* Lexema) {
  /* TODO: Add support to other number kinds. */
  return TagInt(ParseInteger(Lexema->String, Lexema->Length));
}
