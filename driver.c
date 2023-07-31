#include <stdio.h>
#include <stdlib.h>
#include "reader.h"

void
PrintType (struct Lexema* Lexema, FILE* Stream) {
  switch (Lexema->Type) {
  case NO_LEXEMA:
    printf("NO_LEXEMA");
    break;

  case MAYBE_NUMBER:
    printf("MAYBE_NUMBER");
    break;

  case TEXT:
    printf("TEXT");
    break;

  case STRING_TEXT:
    printf("STRING_TEXT");
    break;

  case OPEN_PAREN:
    printf("OPEN_PAREN");
    break;

  case CLOSE_PAREN:
    printf("CLOSE_PAREN");
    break;

  case QUOTE:
    printf("QUOTE");
    break;

  case EOF_LEXEMA:
    printf("EOF_LEXEMA");
    break;
  }
}

void
PrintString (struct Lexema* Lexema, FILE* Stream) {
  if (Lexema->Type == STRING_TEXT)
    putc('"', Stream);
  for (size_t i = 0; i < Lexema->Length; i++) {
    if (Lexema->Type == STRING_TEXT && Lexema->String[i] == '"')
      putc('\\', Stream);
    putc(Lexema->String[i], Stream);
  }
  if (Lexema->Type == STRING_TEXT)
    putc('"', Stream);
}

void
Print (struct Lexema* Lexema, FILE* Stream) {
  fprintf(Stream, "TYPE = ");
  PrintType(Lexema, Stream);
  
  switch (Lexema->Type) {
  case MAYBE_NUMBER:
  case TEXT:
  case STRING_TEXT:
    fprintf(Stream, ", CONTENT = ");
    PrintString(Lexema, Stream);
    break;

  default:
    break;
  }
  fprintf(Stream, "\n");
}

int
main (int argc, char** argv) {
  if (argc < 2) {
    fprintf(stderr, "Please input a file.\n");
    return 1;
  }

  FILE* input = fopen (argv[1], "r");
  if (!input) {
    fprintf(stderr, "File does not exist or is not accessible.\n");
    return 1;
  }
  
  UpdatePeekCharacter(input);
  struct LexemaChain* LexemaChain = ReadLexemaChain(input);
  for (struct LexemaChain* Lexema = LexemaChain; Lexema; Lexema = Lexema->Next)
    Print(Lexema->Lexema, stdout);
  FreeLexemaChain(LexemaChain);
  return 0;
}
