#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "compiler.h"
#include "scanner.h"
#include "string.h"
#include "table.h"
#include "value.h"

/* ==================================
          NAME PROCESSING
=================================== */

/* Check if a string contains another string as a prefix */
static bool prefix(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}

/* Determines if an identifier is a global or a temporary */
static bool isTemp(Token* tokenIdentifier) {
  String* varName = allocateString(parser.current.start, parser.current.length);
  if (prefix("t_", varName->chars)) {
    return true;
  }
  return false;
}



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

/* Checks that the current token is of a given type */
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

/* Declarations
============ */

// Global
// ======

/* Process global name */
static String* globalName() {
  String* varName;
  /* Consume name */
  if (check(TOKEN_IDENTIFIER)) {
    /* Process identifier as string */
    varName = allocateString(parser.current.start, parser.current.length);
    /* Add to global table */
  }
  consume(TOKEN_IDENTIFIER, "Expecting name after type in global declaration.");
  if (prefix("t_", varName->chars)) {
    error("Variable names starting with 't_' are reserved for temporary variables.");
  }
  /* Process value */
  consume(TOKEN_EQUAL, "Expecting variable initialization with '='.");

  return varName;
}

/* Process bool global variable */
static void globalBoolDeclaration() {
  /* Process name and '=' */
  String* varName = globalName();
  /* Process Value */
  Value varValue;
  if (match(TOKEN_TRUE)) {
    varValue = BOOL_VAL(true);
  } else if (match(TOKEN_FALSE)) {
    varValue = BOOL_VAL(false);
  } else {
    error("Boolean variable must be initialized with either 'true' or 'false'.");
  }
  consume(TOKEN_SEMICOLON, "Expecting ';' after variable declaration.");
  /* Add to the globals table */
  tableSet(&compiler.globals, varName, varValue, VAL_BOOL);
}

/* Process bool global variable */
static void globalByteDeclaration() {
  /* Process name and '=' */
  String* varName = globalName();
  /* Process Value */
  Value varValue;
  if (match(TOKEN_NUMBER)) {
    double value = strtod(parser.previous.start, NULL);
    if ((value < 0) || (value > 255)) {
      error("Byte variable must be initialized with a number between 0 and 255.");
    }
    varValue = INT_VAL(value);
  } else {
    error("Wrong type, byte variable must be initialized with a number between 0 and 255.");
  }
  consume(TOKEN_SEMICOLON, "Expecting ';' after variable declaration.");
  /* Add to the globals table */
  tableSet(&compiler.globals, varName, varValue, VAL_BYTE);
}

/* Process int global variable */
static void globalIntDeclaration() {
  /* Process name and '=' */
  String* varName = globalName();
  /* Process Value */
  Value varValue;
  if (match(TOKEN_NUMBER) || match(TOKEN_MINUS)) {
    double value = strtod(parser.previous.start, NULL);
    if ((value < INT16_MIN) || (value > INT16_MAX)) {
      error("Int variable must be initialized with a number between -32768 and 32767.");
    }
    varValue = INT_VAL(value);
  } else {
    error("Wrong type, an int variable must be initialized with a number between -32768 and 32767.");
  }
  consume(TOKEN_SEMICOLON, "Expecting ';' after variable declaration.");
  /* Add to the globals table */
  tableSet(&compiler.globals, varName, varValue, VAL_INT);
}

/* Process state global variable */
static void globalStateDeclaration() {
  /* Process state values */
  int stateNumber = 0;
  consume(TOKEN_LEFT_BRACE,"Expecting '{' before enumeration of states.");
  do {
    consume(TOKEN_IDENTIFIER, "Expecting state name.");
    consume(TOKEN_LEFT_PAREN, "Expecting '(' before state number.");
    consume(TOKEN_NUMBER, "Expecting number for corresponding state.");
    consume(TOKEN_RIGHT_PAREN, "Expecting ')' after state number.");
    stateNumber++;
  } while(match(TOKEN_COMMA));
  if (check(TOKEN_RIGHT_BRACE)) {
    consume(TOKEN_RIGHT_BRACE,"Expecting '}' after enumeration of states.");
  } else {
    error("Missing ',' between states.");
  }
  /* Process name and '=' */
  String* varName = globalName();
  /* Process Value */
  Value varValue;
  if (match(TOKEN_NUMBER)) {
    double currentState = strtod(parser.previous.start, NULL);
    if ((currentState < 0) || (currentState > stateNumber-1)) {
      error("State variable must be initialized with a state between 0 and the number of states.");
    }
    varValue = STATE_VAL(currentState, stateNumber);
  } else {
    error("Wrong type, an int variable must be initialized with a number between -32768 and 32767.");
  }
  consume(TOKEN_SEMICOLON, "Expecting ';' after variable declaration.");

  /* Add to the globals table */
  tableSet(&compiler.globals, varName, varValue, VAL_STATE);
}

/* Declaration of a global variable */
static void globalDeclaration() {
  /* Consume type */
  if (match(TOKEN_BOOL)) {
    globalBoolDeclaration();
  } else if (match(TOKEN_BYTE)) {
    globalByteDeclaration();
  } else if (match(TOKEN_INT)) {
    globalIntDeclaration();
  } else if (match(TOKEN_STATE)) {
    globalStateDeclaration();
  } else {
    error("Global declaration must start with a type.");
  }
}

/* Expressions
=========== */

static void unary() {

}


static void binary() {

}


static void expression() {
  unary();
  binary();
}

// static void binary()

// static void unary()


/* Assignments
=========== */

/* Assign a value to a global variable */
static void globalAssignment() {
  /* Consume identifier, probably need to use it to access the hash table */
  advance();
  consume(TOKEN_EQUAL, "Expecting '=' in assignment.");
  /* Output STORE? */
  /* Process expression */
  expression();
}

/* Assign a value to a given temporary variable and store it in a register */
static void tempAssignment() {
  /* Consume id, probably need to use it to store in the register */
  advance();
  /* Consume type from temp scanner (or call tempDeclaration?) */
  expression();
  /* Store in the top register OR lowest freed */
}

/* Check variable name to determine if it is a temporary variable or not */
static void assignment() {
  if (!check(TOKEN_IDENTIFIER)) {
    error("Assignment should begin with an identifier.");
  }
  /* Test the identifier and redirect to either global or temporary assignment */
  if (isTemp(&parser.current)){
    tempAssignment();
  } else {
    globalAssignment();
  }
}

/* Process
======= */

/* Process guardblock (sequnce of assignments) */
static void guardBlock() {
  consume(TOKEN_GUARD_BLOCK, "Guardblock should begin with 'guardblock' identifier.");
  assignment();
  while(check(TOKEN_COMMA)) {
    consume(TOKEN_COMMA, "Separate assignments with ','.");
    assignment();
  }
  consume(TOKEN_SEMICOLON, "End list of assignments in guardblock with ';'.");
}

/* Process effect (sequnce of assignments) */
static void effect() {
  consume(TOKEN_EFFECT, "Effect declaration should start with 'effect' identifier.");
  assignment();
  while(check(TOKEN_COMMA)) {
    consume(TOKEN_COMMA, "Separate assignments with ','.");
    assignment();
  }
  consume(TOKEN_SEMICOLON, "End list of assignments in guardblock with ';'.");
}

/* Process declaration */
static void process() {
  /* Consume process token */
  consume(TOKEN_PROCESS, "Expecting 'process' to begin a process declaration.");
  /* Consume process name */
  consume(TOKEN_IDENTIFIER, "Process should be given a name.");
  /* Go through guardblock */
  guardBlock();
  /* Go through guardcondition */
  consume(TOKEN_GUARD_COND, "Guardcondition should begin with 'guardcondition' identifier.");
  /* Emit jump to end of effect */
  /* Process identifier */
  consume(TOKEN_IDENTIFIER, "Guardcondition should hold a variable to be tested.");
  consume(TOKEN_SEMICOLON, "Guardcondition should end with ';'.");
  /* Go through effect */
  effect();
  /* Patch jump */
}

/* ==================================
          COMPILE ROUTINE
=================================== */
bool compile(char* source) {
  /* Initialize scanner */
  initScanner(source);
  advance(); // Move to the first token
  /* Initialize parser error handling */
  parser.hadError  = false;
  parser.panicMode = false;

  /* Compile globals */
  while(!match(TOKEN_TRANSIENT)) {
    globalDeclaration();
  }

  /* Go through temporaries */
  while(!check(TOKEN_PROCESS)) {
    advance();
  }

  /* Go through processes */
  while(!match(TOKEN_EOF)) {
    process();
  }



  return parser.hadError;
}
