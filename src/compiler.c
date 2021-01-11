#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "compiler.h"
#include "scanner.h"

/* ==================================
      ALLOCATION - DEALLOCATION
=================================== */

/* Compiler initialization */
void initCompiler() {

}

/* Compiler destruction */
void freeCompiler() {

}

/* ==================================
           ERROR HANDLING
=================================== */

/* Notifies the error with a message */
static void errorAt(Token* token, const char* message) {
  /* if PANIC MODE already triggered */
  if (parser.panicMode) return;

  /* Enter PANIC MODE */
  parser.panicMode = true;

  fprintf(stderr, "[line %d] Error", token->line);

  if (token->type == TOKEN_EOF) {
    /* Error at the end of the file*/
    fprintf(stderr, " at end");
  } else if (token->type == TOKEN_ERROR) {
    /*Nothing, actual error */
  } else {
    /* Print the token location where the error occured */
    fprintf(stderr, " at '%.*s'", token->length, token->start);
  }

  /* Print the actual error message */
  fprintf(stderr, ": %s\n", message);
  parser.hadError = true;
}

/* Notifies an error in the token just processed */
static void error(const char* message) {
  errorAt(&parser.previous, message);
}

/* Notifies an error in the current token */
static void errorAtCurrent(const char* message) {
  errorAt(&parser.current, message);
}

/* ==================================
        FRONT END - PARSING
=================================== */

/* Utilities
========= */

/* Advance the parser with a new non-error token handed over by the scanner */
static void advance() {
  parser.previous = parser.current;

  /* Keep on reading until it finds a non-error token */
  for (;;) {
    parser.current = scanToken();
    /* Check for error */
    if(parser.current.type != TOKEN_ERROR) break;
    /* Report error */
    errorAtCurrent(parser.current.start);

  }
}

/* Expects the next token to be of a given type, else errors with given message */
static void consume(TokenType type, const char* message) {
  if (parser.current.type == type) {
    advance();
    return;
  }
  errorAtCurrent(message);
}

/* Checks that the current token is of a givent type */
static bool check(TokenType type) {
  return parser.current.type == type;
}

/* checks if the current token is of a given type */
static bool match(TokenType type) {
  /* If the type isnt correct, the token is NOT consumed */
  if (!check(type)) return false;
  /* Otherwise token consumed */
  advance();
  return true;
}

/* ==================================
      BACK END - INSTRUCTIONS
=================================== */

/* ==================================
          COMPILE ROUTINE
=================================== */
bool compile(const char* source) {
  /* Initialize scanner */
  initScanner(source);

  /* Initialize parser error handling */
  parser.hadError  = false;
  parser.panicMode = false;

  return parser.hadError;
}
