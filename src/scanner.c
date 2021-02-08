#include <stdio.h>
#include <string.h>

#include "common.h"
#include "scanner.h"

/* ==================================
        STRUCTS AND GLOBALS
=================================== */

typedef struct {
  char* start;   /* Start of the lexem being scanned */
  char* current; /* Current character being scanned */
  int line;      /* Line number for error reporting */
} Scanner;

/* Scanner singleton */
static Scanner scanner;

/* Scanner initialization */
void initScanner(char* source) {
  scanner.start = source;
  scanner.current = source;
  scanner.line = 1;
}


/* ==================================
          CHARACTER TESTS
====================================*/

/* Check if the current character is EOF */
static bool isAtEnd() {
  return *scanner.current == '\0';
}


/* Check if the current character is a letter */
static bool isAlpha(char c) {
  return (c >= 'a' && c <= 'z') ||
         (c >= 'A' && c <= 'Z');
}


/* Check if the current character is a digit */
static bool isDigit(char c) {
  return c >= '0' && c <= '9';
}


/* Check if the current character is either an underscore or a dot (as accepted in the language).
   It can also be an array such as a.x[1] */
static bool isIDPunctuation(char c) {
  return c == '_' || c == '.';
}


/* ==================================
            SCAN HELPERS
====================================*/

/* Consume the current character and return it */
static char advance() {
  scanner.current++;
  return scanner.current[-1];
}


/* Return the current scanned character */
static char peek() {
  return *scanner.current;
}


/* Return the character next to the currently scanned one */
static char peekNext() {
  if (isAtEnd()) return '\0';
  return scanner.current[1];
}


/* Check that the current character is the expected */
static bool match(char expected) {
  if (isAtEnd()) return false;
  if (*scanner.current != expected) return false;
  /* Increment the counter */
  scanner.current++;
  return true;
}


/* Skip all whitspace characters */
static void skipWhitespace() {
  for (;;) {
    char c = peek();
    switch (c) {
      case ' ':
      case '\r':
      case '\t':
        advance();
        break;
      /* carriage return */
      case '\n':
        scanner.line++;
        advance();
        break;
      /* comment */
      case '/':
        if (peekNext() == '/') {
          /* A comment goes until the end of the line. */
          while (peek() != '\n' && !isAtEnd()) advance();
        } else {
          return;
        }
        break;

      default:
        return;
    }
  }
}


/* ==================================
          TOKEN CREATION
====================================*/

/* Create a token from a type */
static Token makeToken(TokenType type) {
  Token token;
  token.type = type;
  token.start = scanner.start;
  token.length = (int)(scanner.current - scanner.start);
  token.line = scanner.line;

  return token;
}


/* Create an error token with a message and the line */
static Token errorToken(char* message) {
  Token token;
  token.type = TOKEN_ERROR;
  token.start = message;
  token.length = (int)strlen(message);
  token.line = scanner.line;

  return token;
}


/* ==================================
      TOKEN TYPE IDENTIFICATION
====================================*/

/* Check if a word is a reserved keyword */
static TokenType checkKeyword(int start, int length, const char* rest, TokenType type) {
  /* Test if :
  - the lexeme is exactly as long as the reserved word
  - the characters are corresponding */
  if (scanner.current - scanner.start == start + length &&
      memcmp(scanner.start + start, rest, length) == 0) {
    return type;
  }
  return TOKEN_IDENTIFIER;
}


/* Define the identifier type, by comparing it to keywords */
static TokenType identifierType() {
  switch (scanner.start[0]) {
    case 'a': return checkKeyword(1, 2, "nd", TOKEN_AND); // and
    case 'b':
      if (scanner.current - scanner.start > 1) {
        switch (scanner.start[1]) {
          case 'o': return checkKeyword(2, 2, "ol", TOKEN_BOOL);       // bool
          case 'y': return checkKeyword(2, 2, "te", TOKEN_BYTE);       // byte
        }
      }
    case 'e': return checkKeyword(1, 5, "ffect", TOKEN_EFFECT);        // effect
    case 'f': return checkKeyword(1, 4, "alse", TOKEN_FALSE);          // false
    case 'g':
      if (scanner.current - scanner.start > 5) {
        switch (scanner.start[5]) {
          case 'b':
          case 'B': return checkKeyword(6, 4, "lock", TOKEN_GUARD_BLOCK);     // guardblock
          case 'c':
          case 'C': return checkKeyword(6, 8, "ondition", TOKEN_GUARD_COND);  // guardcondition
        }
      }
    case 'i': return checkKeyword(1, 2, "nt", TOKEN_INT);              // int
    case 'o': return checkKeyword(1, 1, "r", TOKEN_OR);                // or
    case 'p': return checkKeyword(1, 6, "rocess", TOKEN_PROCESS);      // process
    case 's': return checkKeyword(1, 4, "tate", TOKEN_STATE);          // type
    case 't':
      if (scanner.current - scanner.start > 1) {
        switch (scanner.start[1]) {
          case 'e': return checkKeyword(2, 2, "mp", TOKEN_TEMP);       // temp
          case 'r': return checkKeyword(2, 2, "ue", TOKEN_TRUE);       // true
          case 'u': return checkKeyword(2, 3, "ple", TOKEN_TUPLE);     // tuple
        }
      }
  }
  return TOKEN_IDENTIFIER;
}


/* Create an identifier token */
static Token identifier() {
  /* Numbers are also allowed after the first letter */
  while (isAlpha(peek()) || isDigit(peek()) || isIDPunctuation(peek())) advance();
  return makeToken(identifierType());
}


/* Create a number token */
static Token number() {
  /* Consume the integer part */
  while(isDigit(peek())) advance();
  /* Look for a decimal part */
  if (peek() == '.' && isDigit(peekNext())) {
    /* Consume the . */
    advance();
  }
  /* Consume the decimal part */
  while(isDigit(peek())) advance();
  return makeToken(TOKEN_NUMBER);
}


/* ==================================
          SCANNING ROUTINE
====================================*/

/* Scan the current lexeme into a token */
Token scanToken() {
  skipWhitespace();

  scanner.start = scanner.current;

  if (isAtEnd()) return makeToken(TOKEN_EOF);

  char c = advance();
  /* Identifier */
  if (isAlpha(c)) return identifier();
  /* Digit */
  if (isDigit(c)) return number();

  switch (c) {
    /* Single character */
    case '(': return makeToken(TOKEN_LEFT_PAREN);
    case ')': return makeToken(TOKEN_RIGHT_PAREN);
    case '[': return makeToken(TOKEN_LEFT_SQBRACKET);
    case ']': return makeToken(TOKEN_RIGHT_SQBRACKET);
    case '{': return makeToken(TOKEN_LEFT_BRACE);
    case '}': return makeToken(TOKEN_RIGHT_BRACE);
    case ';': return makeToken(TOKEN_SEMICOLON);
    case ',': return makeToken(TOKEN_COMMA);
    case '.': return makeToken(TOKEN_DOT);
    case '-': return makeToken(TOKEN_MINUS);
    case '+': return makeToken(TOKEN_PLUS);
    case '/': return makeToken(TOKEN_SLASH);
    case '*': return makeToken(TOKEN_STAR);
    case '%': return makeToken(TOKEN_MODULO);
    /* Double character */
    case '!':
      if (match('=')) return makeToken(TOKEN_BANG_EQUAL);
    case '=':
      return makeToken(
          match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
    case '<':
      return makeToken(
          match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
    case '>':
      return makeToken(
          match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
  }

  return errorToken("Unexpected character.");
}


/* ==================================
           TOKEN PRINTING
====================================*/

static const char* TokenNames[] = {
 "TOKEN_LEFT_PAREN", "TOKEN_RIGHT_PAREN",
 "TOKEN_LEFT_BRACE", "TOKEN_RIGHT_BRACE",
 "TOKEN_LEFT_SQBRACKET", "TOKEN_RIGHT_SQBRACKET",
 "TOKEN_COMMA", "TOKEN_DOT", "TOKEN_MINUS",
 "TOKEN_PLUS", "TOKEN_SEMICOLON", "TOKEN_SLASH",
 "TOKEN_STAR", "TOKEN_MODULO",

 "TOKEN_BANG_EQUAL", "TOKEN_EQUAL", "TOKEN_EQUAL_EQUAL",
 "TOKEN_GREATER", "TOKEN_GREATER_EQUAL",
 "TOKEN_LESS", "TOKEN_LESS_EQUAL",

 "TOKEN_IDENTIFIER", "TOKEN_NUMBER",

 "TOKEN_AND", "TOKEN_OR", "TOKEN_EFFECT",
 "TOKEN_FALSE", "TOKEN_GUARD_BLOCK", "TOKEN_GUARD_COND",
 "TOKEN_PROCESS", "TOKEN_SYSTEM", "TOKEN_TEMP", "TOKEN_TRUE",

 "TOKEN_INT", "TOKEN_BOOL", "TOKEN_BYTE", "TOKEN_STATE", "TOKEN_TUPLE",

 "TOKEN_ERROR",
 "TOKEN_EOF"
};

/* Print the name of the current token */
void printToken(Token token) {
  printf("%s\n", TokenNames[token.type]);
}
