#include "reader.h"
#include "alloc.h"

/* TODO: All 0-set return values MUST be replaced with a proper list value that can be
** returned to the interpreter. */

/* TODO: To avoid repeating symbols we need a way to uniquify them & return them if we read a repeat. */

LispObjectImm
ReadObject1 (struct LexemaIndex* LexemaIndex);

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

/* Read a List from the lexema index. */
LispObjectImm
ReadListObj (struct LexemaIndex* LexemaIndex) {
  /* TODO: We could possibly benefit from writing a nice interface to iterate through Lexemas
  ** without all the manual, error-prone hassle. */
  if (PeekLexema(LexemaIndex)->Type == EOF_LEXEMA || (PeekLexema(LexemaIndex) == NO_LEXEMA))
    return 0;
  /* Guard if. */
  if (PeekLexema(LexemaIndex)->Type == OPEN_PAREN) {
    LispObjectImm Head = 0;

    NextLexema(LexemaIndex);
    if (PeekLexema(LexemaIndex)->Type == EOF_LEXEMA || (PeekLexema(LexemaIndex) == NO_LEXEMA))
      return 0;
    if (PeekLexema(LexemaIndex)->Type == CLOSE_PAREN) {
      /* TODO: Return NIL or return an empty list. */
      NextLexema(LexemaIndex);
      return 0;
    }

    Head = MakeConsCell(ReadObject1(LexemaIndex), 0);
    LispObjectImm Tail = Head;

    while (PeekLexema(LexemaIndex)->Type != EOF_LEXEMA && PeekLexema(LexemaIndex)->Type != NO_LEXEMA)
      {
	if (PeekLexema(LexemaIndex)->Type == CLOSE_PAREN)
	  return Head;
	SetConsCdr(UntagCons(Tail), MakeConsCell(ReadObject1(LexemaIndex), 0));
	NextLexema(LexemaIndex);
      }
  }
  /* Unreachable if control code is proper. */
  return 0;
}

/* Read a Lisp object. */
LispObjectImm
ReadObject1 (struct LexemaIndex* LexemaIndex) {
  switch (PeekLexema(LexemaIndex)->Type) {
  case NO_LEXEMA:
  case EOF_LEXEMA:
    return 0;

  case MAYBE_NUMBER: {
      LispObjectImm Number = ReadNumberType(PeekLexema(LexemaIndex));
      NextLexema(LexemaIndex);
      return Number;
    }

  case TEXT: {
    /* TODO: Return unique symbol if another with the same name has been found, high priority. */
    LispObjectImm Symbol = ReadSymbolType(PeekLexema(LexemaIndex));
    NextLexema(LexemaIndex);
    return Symbol;
  }

  case STRING_TEXT: {
    LispObjectImm String = ReadStringType(PeekLexema(LexemaIndex));
    NextLexema(LexemaIndex);
    return String;
  }

  case OPEN_PAREN: {
    LispObjectImm List = ReadListObj(LexemaIndex);
    NextLexema(LexemaIndex);
    return List;
  }

    /* This is an error. */
  case CLOSE_PAREN:
    return 0;

  case QUOTE: {
    NextLexema(LexemaIndex);
      /* We need the quote symbol here. */
    LispObjectImm QuotedObject = MakeConsCell(0, ReadObject1(LexemaIndex));
    NextLexema(LexemaIndex);
    return QuotedObject;
    }
  }
}
