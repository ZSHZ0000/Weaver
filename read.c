#include "reader.h"
#include "alloc.h"
#include "env.h"

/* TODO: All 0-set return values MUST be replaced with a proper list value that can be
** returned to the interpreter. */

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
  return FindOrMakeSymbol(Lexema->String, Lexema->Length);
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
  if (PeekLexema(LexemaIndex)->Type == EOF_LEXEMA || (PeekLexema(LexemaIndex) == NO_LEXEMA))
    return QuoteNil;
  /* Guard if. */
  if (PeekLexema(LexemaIndex)->Type == OPEN_PAREN) {
    NextLexema(LexemaIndex);
    LispObjectImm Head = QuoteNil;

    if (PeekLexema(LexemaIndex)->Type == EOF_LEXEMA || (PeekLexema(LexemaIndex) == NO_LEXEMA))
      /* TODO: This should be an error. */
      return QuoteNil;
    if (PeekLexema(LexemaIndex)->Type == CLOSE_PAREN) {
      return QuoteNil;
    }

    /* List init. */
    Head = MakeConsCell(ReadObject1(LexemaIndex), QuoteNil);
    LispObjectImm Tail = Head;

    while (PeekLexema(LexemaIndex)->Type != EOF_LEXEMA && PeekLexema(LexemaIndex)->Type != NO_LEXEMA) {
      if (PeekLexema(LexemaIndex)->Type == CLOSE_PAREN)
	return Head;
      LispObjectImm NextTail = MakeConsCell(ReadObject1(LexemaIndex), QuoteNil);
      SetConsCdr(UntagCons(Tail), NextTail);
      Tail = NextTail;
    }
  }
  /* Unreachable if control code is proper. */
  return QuoteNil;
}

/* Read a Lisp object. */
LispObjectImm
ReadObject1 (struct LexemaIndex* LexemaIndex) {
  switch (PeekLexema(LexemaIndex)->Type) {
  case NO_LEXEMA:
  case EOF_LEXEMA:
    /* TODO: Should be an error. */
    return QuoteNil;

  case MAYBE_NUMBER: {
      LispObjectImm Number = ReadNumberType(PeekLexema(LexemaIndex));
      NextLexema(LexemaIndex);
      return Number;
    }

  case TEXT: {
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
    return QuoteNil;

  case QUOTE: {
    NextLexema(LexemaIndex);
    /* We need the quote symbol here. */
    LispObjectImm QuotedObject = MakeConsCell(QuoteQuote, ReadObject1(LexemaIndex));
    NextLexema(LexemaIndex);
    return QuotedObject;
  }

    /* Control should NEVER reach here, this should raise an error. */
  default:
    return QuoteNil;
  }
}
