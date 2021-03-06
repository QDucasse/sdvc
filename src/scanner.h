#ifndef sdvu_scanner_h
#define sdvu_scanner_h

/* Token types */
typedef enum {
  /* Single-character tokens */
  TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
  TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
  TOKEN_LEFT_SQBRACKET, TOKEN_RIGHT_SQBRACKET,
  TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS,
  TOKEN_PLUS, TOKEN_SEMICOLON, TOKEN_SLASH,
  TOKEN_STAR, TOKEN_MODULO,

  /* One or two character tokens */
  TOKEN_BANG_EQUAL, TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,
  TOKEN_GREATER, TOKEN_GREATER_EQUAL,
  TOKEN_LESS, TOKEN_LESS_EQUAL,


  /* Literals */
  TOKEN_IDENTIFIER, TOKEN_NUMBER,

  /* Keywords */
  TOKEN_AND, TOKEN_OR, TOKEN_EFFECT, TOKEN_NOT,
  TOKEN_FALSE, TOKEN_GUARD_BLOCK, TOKEN_GUARD_COND,
  TOKEN_PROCESS, TOKEN_SYSTEM, TOKEN_TEMP, TOKEN_TRUE,

  /* Types */
  TOKEN_INT, TOKEN_BOOL, TOKEN_BYTE, TOKEN_STATE, TOKEN_TUPLE,

  /* Other */
  TOKEN_ERROR,
  TOKEN_EOF
} TokenType;

/* Token Definition */
typedef struct {
  TokenType type;
  char* start;
  int length;
  int line;
} Token;

void initScanner(char* source);
Token scanToken();
void fprintToken(FILE* outstream, Token token);

#endif
