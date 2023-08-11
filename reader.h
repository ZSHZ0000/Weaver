#ifndef READER_H
#define READER_H

#include <stdio.h>
#include <stdlib.h>

#define EOF_VALUE 256
#define ERROR_VALUE 257

/* Initial Lexema sizes for variadic size ones. */
#define INITIAL_SIZE 32

enum LexemaType {
  NO_LEXEMA, /* This is an error. */
  MAYBE_NUMBER,
  TEXT,
  STRING_TEXT, /* Program strings. */
  OPEN_PAREN,
  CLOSE_PAREN,
  QUOTE,
  EOF_LEXEMA
};

struct Lexema {
  enum LexemaType Type;
  size_t Length;
  size_t BufferSize;
  char* String;
};

struct LexemaChain {
  struct Lexema* Lexema;
  struct LexemaChain* Next;
};

struct LexemaIndex {
  struct LexemaChain* Chain;
  struct LexemaChain* CurrentSlab;
  struct Lexema* NoLexema;
  struct LexemaChain* NoSlab;
};

_Bool
IsNumeric (char Character);

void
UpdatePeekCharacter ();

struct LexemaChain*
ReadLexemaChain (FILE* Stream);

void
FreeLexemaChain (struct LexemaChain* LexemaChainHead);

struct LexemaIndex*
MakeLexemaIndex (struct LexemaChain* LexemaChain);

struct Lexema*
PeekLexema (struct LexemaIndex* LexemaIndex);

void
NextLexema (struct LexemaIndex* LexemaIndex);

void
FreeLexemaIndex (struct LexemaIndex* LexemaIndex);

#endif /* READER_H */
