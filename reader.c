#include <stdio.h>
#include <stdlib.h>
#include "reader.h"

/* Static peek character slot. */
static int CurrentCharacter = 0;

/* Update the peek character with filestream Stream, the status is returned by the function, the read value is stored in the Character pointer. */
void
UpdatePeekCharacter (FILE* Stream) {
  size_t TotalRead = fread(&CurrentCharacter, sizeof(char), 1, Stream);
  if (TotalRead != 1) {
    if (feof(Stream))
      CurrentCharacter = EOF_VALUE;
    if (ferror(Stream))
      CurrentCharacter = ERROR_VALUE;
  }
}

/* Get the current character. */
char
GetCurrentCharacter () {
  if (CurrentCharacter >= EOF_VALUE)
    return 0;
  return CurrentCharacter;
}

/* Checks if EOF has been reached. */
_Bool
EOFReached () {
  return CurrentCharacter == EOF_VALUE;
}

/* Checks if we got an error. */
_Bool
GotError () {
  return CurrentCharacter == ERROR_VALUE;
}


/* Checks if it is whitespace character. */
_Bool
IsWhiteSpace (char Character) {
  return Character == ' ' || Character == '\t' || Character == '\n' || Character == '\r';
}

/* Checks if it is a numeric character. */
_Bool
IsNumeric (char Character) {
  return Character >= '0' && Character <= '9';
}

/* Checks if it is an alphabetic character. */
_Bool
IsAlpha (char Character) {
  return (Character >= 'a' && Character <= 'z') || (Character >= 'A' && Character <= 'Z');
}

/* Checks if it is a symbol. */
_Bool
IsSymbol (char Character) {
  return (Character == '!' || Character == '@' || Character == '#' || Character == '$' || Character == '%' ||
	  Character == '^' || Character == '&' || Character == '*' || Character == '_' || Character == '-' ||
	  Character == '+' || Character == '=' || Character == ':' || Character == '<' || Character == '>' ||
	  Character == ',' || Character == '.' || Character == '?' || Character == '/');
}

/* Checks if it is a reserved character. */
_Bool
IsReserved (char Character) {
  return (Character == '(' || Character == ')' || Character == '\'');
}

/* Initialize or deinitialize to NO_LEXEMA. */
void
DeLexema (struct Lexema* Lexema) {
  Lexema->Type = NO_LEXEMA;
  Lexema->Length = 0;
  Lexema->BufferSize = 0;
  if (Lexema->String)
    free(Lexema->String);
  Lexema->String = NULL;
}

/* Allocate a lexema struct initialized. */
struct Lexema*
NoLexema () {
  struct Lexema* Lexema = malloc(sizeof(struct Lexema));
  Lexema->String = NULL;
  DeLexema(Lexema);
  return Lexema;
}

/* Read a text lexema. */
struct Lexema*
ReadTextLexema (FILE* Stream) {
  struct Lexema* TextLexema = NoLexema();
  if (IsAlpha(CurrentCharacter) || IsSymbol(CurrentCharacter)) {
    TextLexema->Type = TEXT;
    TextLexema->Length = 1;
    TextLexema->BufferSize = INITIAL_SIZE * sizeof(char);
    TextLexema->String = malloc(INITIAL_SIZE * sizeof(char));
    TextLexema->String[0] = (char) CurrentCharacter;

    UpdatePeekCharacter(Stream);
    if (EOFReached() || GotError())
      return TextLexema;

    while (!(IsWhiteSpace(CurrentCharacter)) && !EOFReached() && !GotError() && !IsReserved(CurrentCharacter)) {
      /* Expand the buffer if it is too small. */
      if (TextLexema->Length >= TextLexema->BufferSize) {
	TextLexema->String = realloc(TextLexema->String, TextLexema->BufferSize + INITIAL_SIZE * sizeof(char));
	TextLexema->BufferSize = TextLexema->BufferSize + INITIAL_SIZE * sizeof(char);
      }

      TextLexema->String[TextLexema->Length] = (char) CurrentCharacter;
      TextLexema->Length++;

      UpdatePeekCharacter(Stream);
    }
  }
  return TextLexema;
}

/* Read a number lexema. */
struct Lexema*
ReadMaybeNumberLexema (FILE* Stream) {
  struct Lexema* MaybeNumberLexema = NoLexema();
  if (IsNumeric(CurrentCharacter)) {
    MaybeNumberLexema->Type = MAYBE_NUMBER;
    MaybeNumberLexema->Length = 1;
    MaybeNumberLexema->BufferSize = INITIAL_SIZE * sizeof(char);
    MaybeNumberLexema->String = malloc(INITIAL_SIZE * sizeof(char));
    MaybeNumberLexema->String[0] = (char) CurrentCharacter;

    UpdatePeekCharacter(Stream);
    if (EOFReached() || GotError())
      return MaybeNumberLexema;

    while (!(IsWhiteSpace(CurrentCharacter)) && !EOFReached() && !GotError() && !IsReserved(CurrentCharacter)) {
      /* Expand the buffer if it is too small. */
      if (MaybeNumberLexema->Length >= MaybeNumberLexema->BufferSize) {
	MaybeNumberLexema->String = realloc(MaybeNumberLexema->String, MaybeNumberLexema->BufferSize + INITIAL_SIZE * sizeof(char));
	MaybeNumberLexema->BufferSize = MaybeNumberLexema->BufferSize + INITIAL_SIZE * sizeof(char);
      }

      MaybeNumberLexema->String[MaybeNumberLexema->Length] = (char) CurrentCharacter;
      MaybeNumberLexema->Length++;

      UpdatePeekCharacter(Stream);
    }
  }
  return MaybeNumberLexema;
}

/* Read a quoted text (string) lexema. */
struct Lexema*
ReadStringTextLexema (FILE* Stream) {
  struct Lexema* StringTextLexema = NoLexema();
  if (CurrentCharacter == '"') {
    StringTextLexema->Type = STRING_TEXT;
    StringTextLexema->Length = 0;
    StringTextLexema->BufferSize = INITIAL_SIZE * sizeof(char);
    StringTextLexema->String = malloc(INITIAL_SIZE * sizeof(char));

    UpdatePeekCharacter(Stream);
    if (EOFReached() || GotError()) {
      DeLexema(StringTextLexema);
      return StringTextLexema;
    }

    /* We want to do the control of the loop inside the loop since we must escape \'s. */
    while(1) {
      /* Expand the buffer if it is too small. */
      if (StringTextLexema->Length >= StringTextLexema->BufferSize) {
	StringTextLexema->String = realloc(StringTextLexema->String, StringTextLexema->BufferSize + INITIAL_SIZE * sizeof(char));
	StringTextLexema->BufferSize = StringTextLexema->BufferSize + INITIAL_SIZE * sizeof(char);
      }

      switch(CurrentCharacter) {
      case '"':
	return StringTextLexema;

      case '\\':
	UpdatePeekCharacter(Stream);
	if (EOFReached() || GotError()) {
	  DeLexema(StringTextLexema);
	  return StringTextLexema;
	}

	if (CurrentCharacter == '"') {
	  StringTextLexema->String[StringTextLexema->Length] = (char) CurrentCharacter;
	  StringTextLexema->Length++;
	  UpdatePeekCharacter(Stream);
	} else if (CurrentCharacter == '\\') {
	  StringTextLexema->String[StringTextLexema->Length] = (char) CurrentCharacter;
	  StringTextLexema->Length++;
	  UpdatePeekCharacter(Stream);
	} else {
	  StringTextLexema->String[StringTextLexema->Length] = '\\';
	  StringTextLexema->Length++;
	}
	break;

      default:
	if (EOFReached() || GotError()) {
	  DeLexema(StringTextLexema);
	  return StringTextLexema;
	}
	
	StringTextLexema->String[StringTextLexema->Length] = (char) CurrentCharacter;
	StringTextLexema->Length++;
	UpdatePeekCharacter(Stream);
      }
    }
  }
  return StringTextLexema;
}

/* Make open parentheses lexema. */
struct Lexema*
OpenParenLexema (FILE* Stream) {
  struct Lexema* OpenParen = NoLexema();
  OpenParen->Type = OPEN_PAREN;
  UpdatePeekCharacter(Stream);
  return OpenParen;
}

/* Make close parentheses lexema. */
struct Lexema*
CloseParenLexema (FILE* Stream) {
  struct Lexema* CloseParen = NoLexema();
  CloseParen->Type = CLOSE_PAREN;
  UpdatePeekCharacter(Stream);
  return CloseParen;
}

/* Make quote lexema. */
struct Lexema*
QuoteLexema (FILE* Stream) {
  struct Lexema* Quote = NoLexema();
  Quote->Type = QUOTE;
  UpdatePeekCharacter(Stream);
  return Quote;
}

/* Make EOF lexema. */
struct Lexema*
EOFLexema (FILE* Stream) {
  struct Lexema* Lexema = NoLexema();
  Lexema->Type = EOF_LEXEMA;
  return Lexema;
}

/* Read a lexema. */
struct Lexema*
ReadLexema (FILE* Stream) {
  /* Rather use this than to delegate or recurse. */
 restart:
  if (EOFReached())
    return EOFLexema(Stream);
  else if (GotError())
    return NoLexema();
  else if (IsWhiteSpace(CurrentCharacter)) {
    while(IsWhiteSpace(CurrentCharacter))
      UpdatePeekCharacter(Stream);
    goto restart;
  }
  else if (CurrentCharacter == '\0') /* Error. */
    return NoLexema();
  else if (IsAlpha(CurrentCharacter) || IsSymbol(CurrentCharacter))
    return ReadTextLexema(Stream);
  else if (IsNumeric(CurrentCharacter))
    return ReadMaybeNumberLexema(Stream);
  else if (CurrentCharacter == '"')
    return ReadStringTextLexema(Stream);
  else if (CurrentCharacter == '(')
    return OpenParenLexema(Stream);
  else if (CurrentCharacter == ')')
    return CloseParenLexema(Stream);
  else if (CurrentCharacter == '\'')
    return QuoteLexema(Stream);
  return NoLexema();
}

/* Allocate a LexemaChain item. */
struct LexemaChain*
NewLexemaSlab (struct LexemaChain* MaybeHead, struct Lexema* Lexema) {
  struct LexemaChain* Slab = malloc(sizeof(struct LexemaChain));
  Slab->Lexema = Lexema;
  Slab->Next = NULL;
  if (MaybeHead)
    MaybeHead->Next = Slab;
  return Slab;
}

/* Read a chain of lexemas. */
struct LexemaChain*
ReadLexemaChain (FILE* Stream) {
  struct LexemaChain* LexemaChainHead = NULL;
  LexemaChainHead = NewLexemaSlab(NULL, ReadLexema(Stream));
  if (LexemaChainHead->Lexema->Type == EOF_LEXEMA || LexemaChainHead->Lexema->Type == NO_LEXEMA)
    return LexemaChainHead;
  struct LexemaChain* CurrentSlab = NewLexemaSlab(LexemaChainHead, ReadLexema(Stream));
  while (CurrentSlab->Lexema->Type != EOF_LEXEMA && CurrentSlab->Lexema->Type != NO_LEXEMA)
    CurrentSlab = NewLexemaSlab(CurrentSlab, ReadLexema(Stream));
  return LexemaChainHead;
}

/** FROM HERE NOW IT IS TEST CODE. **/

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
  return 0;
}
