#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "compiler.h"
#include "scanner.h"
#include "sstring.h"
#include "register.h"
#include "table.h"
#include "value.h"


/* Parser structure */
typedef struct {
  Token current;  /* current Token being investigated */
  Token previous; /* next Token being investigated */
  bool hadError;  /* Previous error was encountered */
  bool panicMode; /* To avoid cascading errors */
} Parser;

/* Parser singleton */
static Parser parser;


/* Binary Operator Table */
int binopTable[] = {
  [TOKEN_MINUS]         = OP_SUB,
  [TOKEN_PLUS]          = OP_ADD,
  [TOKEN_SLASH]         = OP_DIV,
  [TOKEN_STAR]          = OP_MUL,
  [TOKEN_MODULO]        = OP_MOD,
  [TOKEN_BANG_EQUAL]    = OP_NEQ,
  [TOKEN_EQUAL_EQUAL]   = OP_EQ,
  [TOKEN_GREATER]       = OP_GT,
  [TOKEN_GREATER_EQUAL] = OP_GTEQ,
  [TOKEN_LESS]          = OP_LT,
  [TOKEN_LESS_EQUAL]    = OP_LTEQ,
  [TOKEN_AND]           = OP_AND,
  [TOKEN_OR]            = OP_OR
};

/* ==================================
          NAME PROCESSING
=================================== */

void print_binary(unsigned char c)
{
 unsigned char i1 = (1 << (sizeof(c)*8-1));
 for(; i1; i1 >>= 1)
      printf("%d",(c&i1)!=0);
}

/* Check if a string contains another string as a prefix */
static bool prefix(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}

/* Determines if an identifier is a global or a temporary */
static bool isTemp(char* name) {
  return prefix("t_", name);
}

/* Determines if an identifier is a global or a temporary */
static bool isTempToken(Token* tokenIdentifier) {
  char* name = tokenIdentifier->start;
  return isTemp(name);
}

/* Determines if the given token is a binary operator */
static bool isBinOp(Token* tokenOperator) {
  TokenType type = tokenOperator->type;
  return type == TOKEN_MINUS || type == TOKEN_PLUS || type == TOKEN_SLASH ||
         type == TOKEN_STAR  || type == TOKEN_MODULO || type == TOKEN_BANG_EQUAL ||
         type == TOKEN_EQUAL_EQUAL || type == TOKEN_GREATER || type == TOKEN_GREATER_EQUAL ||
         type == TOKEN_LESS || type == TOKEN_LESS_EQUAL || type == TOKEN_AND || type == TOKEN_OR;
}

/* ==================================
      ALLOCATION - DEALLOCATION
=================================== */

/* Compiler initialization */
void initCompiler() {
  compiler = ALLOCATE_OBJ(Compiler);
  compiler->chunk     = initChunk();
  compiler->globals   = initTable();
  compiler->registers = ALLOCATE_ARRAY(Register, REG_NUMBER);
  for (int i = 0 ; i < REG_NUMBER ; i++) {
    compiler->registers[i] = *initRegister(i);
  }
}

/* Compiler destruction */
void freeCompiler() {
  freeChunk(compiler->chunk);
  freeTable(compiler->globals);
  FREE(compiler->registers);
  FREE(compiler);
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

/* Jump handling
============= */

/* Emit a jump */
static int emitJump() {
  /* OP Code for JMP and placeholder for the jump address, size 28-bits */
  uint32_t instruction = (OP_JMP << 28) | 0xfffffff;
  writeChunk(compiler->chunk, instruction);
  /* Return the index of the placeholder */
  return compiler->chunk->count - 1;
}

/* */
static void patchJump(int offset) {
  /* -1 to adjust the jump offset itself */
  int jump = compiler->chunk->count - offset - 1;

  if (jump > 0xfffffff) {
    error("Too much code to jump over.");
  }
  /* Replace the operand at the given location with the jump offset */
  compiler->chunk->instructions[offset] = (jump >> 28) & 0xfffffff;
}



/* ==================================
      BACK END - INSTRUCTIONS
=================================== */

/* Declarations
============ */

/* Globals
======= */

/* Process global name */
static String* globalName() {
  String* varName = initString();
  /* Consume name */
  if (check(TOKEN_IDENTIFIER)) {
    /* Process identifier as string */
    assignString(varName, parser.current.start, parser.current.length);
    /* Add to global table */
  }
  consume(TOKEN_IDENTIFIER, "Expecting name after type in global declaration.");
  if (isTemp(varName->chars)) {
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
  Value varValue = NIL_VAL;
  if (match(TOKEN_TRUE)) {
    varValue = BOOL_VAL(true);
  } else if (match(TOKEN_FALSE)) {
    varValue = BOOL_VAL(false);
  } else {
    error("Boolean variable must be initialized with either 'true' or 'false'.");
  }
  consume(TOKEN_SEMICOLON, "Expecting ';' after variable declaration.");
  /* Add to the globals table */
  tableSet(compiler->globals, varName, varValue);
}

/* Process bool global variable */
static void globalByteDeclaration() {
  /* Process name and '=' */
  String* varName = globalName();
  /* Process Value */
  Value varValue = NIL_VAL;
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
  tableSet(compiler->globals, varName, varValue);

}

/* Process int global variable */
static void globalIntDeclaration() {
  /* Process name and '=' */
  String* varName = globalName();
  /* Process Value */
  Value varValue = NIL_VAL;
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
  tableSet(compiler->globals, varName, varValue);
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
  Value varValue = NIL_VAL;
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
  tableSet(compiler->globals, varName, varValue);
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

/* Assignments
=========== */

/* Look for the register containing a given variable (NULL otherwise) */
static Register* getRegNumberFromVar(String* varName) {
  for (int i = 0 ; i < REG_NUMBER ; i++) {
    if (stringsEqual(varName, compiler->registers[i].varName)) {
      return &compiler->registers[i];
    }
  }
  return NULL;
}

static void leftHandSide(Instruction* instruction) {
  if(check(TOKEN_NUMBER)) {
    /* I? */
    instruction->imma = (unsigned int) strtol(parser.current.start, NULL, 0);
    /* Set first cfg bit to 0 */
    instruction->cfg_mask = 0b0 << 1;
  } else if(check(TOKEN_IDENTIFIER)) {
    /* Left hand side is an Identifier */
    if (isTempToken(&parser.current)) {
      /* LHS is a temporary */
      String* tempKey = initString();
      assignString(tempKey, parser.current.start, parser.current.length);
      /* Resolve register */
      Register* foundReg = getRegFromVar(globKey);
      /* Set the resolved register to ra */
      printf("Setting resolved register as a temporary!.\n");
    } else {
      /* LHS is a global */
      String* globKey = initString();
      assignString(globKey, parser.current.start, parser.current.length);
      /* Resolve register */
      Register* foundReg = getRegFromVar(globKey);
      /* Set the resolved register to ra */
      printf("Setting resolved register as a global!.\n");
    }
    /* Set first cfg bit to 1 */
    instruction->cfg_mask = 0b1 << 1;
  } else {
    /* Not a variable or an immediate value */
    error("An assignment needs the rvalue to be either a variable or immediate value.");
  }
  /* Consume LHS token */
  advance();
}

static void rightHandSide(Instruction* instruction) {
  if(check(TOKEN_NUMBER)) {  /* ?I */
    instruction->immb = (unsigned int) strtol(parser.current.start, NULL, 0);
    /* Set second cfg bit to 1 */
    instruction->cfg_mask |= 0b1;
  } else if(check(TOKEN_IDENTIFIER)) { /* ?R */
    /* Left hand side is an Identifier */
    if (isTempToken(&parser.current)) {
      /* RHS is a temporary */
      String* tempKey = initString();
      assignString(tempKey, parser.current.start, parser.current.length);
      /* Resolve register */
      Register* foundReg = getRegFromVar(tempKey);
      /* Set the resolved register to rb */
      printf("Setting resolved register as a temporary!.\n");
    } else {
      /* RHS is a global */
      String* globKey = initString();
      assignString(globKey, parser.current.start, parser.current.length);
      /* Resolve register */
      Register* foundReg = getRegFromVar(globKey);
      /* If the register is NULL -> Store the value into a new one */
      if (foundReg == NULL) {
        /* Go to the table and store the value in a register */
        /* Emit a load instruction with the correct address */
      } else {
        /* Set the resolved register to rb */
        instruction->rb = foundReg->number;
      }
      printf("Setting resolved register %u as a global!.\n", instruction->rb);
    }
    /* Set second cfg bit to 0 */
    instruction->cfg_mask |= 0b0;
  }
  /* Consume RHS token */
  advance();

}

void operator(Instruction* instruction) {
  /* Consume operator */
  if (isBinOp(&parser.current)) {
    instruction->op_code = binopTable[parser.current.type];
    advance();
  } else {
    error("Expected binary operator.");
  }
}

static void expression() {
  /* Consume identifier, probably need to use it to access the hash table */
  consume(TOKEN_IDENTIFIER, "Variable assignment should have an identifier");
  /* Store the name of the variable in a string */
  String* keyAssignedVar = initString();
  assignString(keyAssignedVar, parser.previous.start, parser.previous.length);
  /* Consume the equal token */
  consume(TOKEN_EQUAL, "Expecting '=' in assignment.");
  Instruction* instruction = initInstruction();
  /* Consume left hand side of expression */
  leftHandSide(instruction);
  /* Consume operator */
  operator(instruction);
  /* Consume right hand side of expression */
  rightHandSide(instruction);
  /* Write instruction */
  uint32_t bitsInstruction = instructionToUint32(instruction);
  print_binary(bitsInstruction);
  writeChunk(compiler->chunk, bitsInstruction);
}


/* Assign a value to a global variable */
static void globalAssignment() {
  expression();
}

/* Assign a value to a given temporary variable and store it in a register */
static void tempAssignment() {
  /* Consume temp token, probably need to use it to access the hash table */
  consume(TOKEN_TEMP, "Temporary variable assignment should begin with 'temp'.");
  /* Consume the type */
  if (check(TOKEN_BOOL) || check(TOKEN_BYTE) || check(TOKEN_INT)) {
    advance();
  } else {
    error("Temporary variable assignment should have a type.");
  }
  expression();
}

/* Check variable name to determine if it is a temporary variable or not */
static void assignment() {
  if (check(TOKEN_TEMP)) {
    tempAssignment();
  } else if (check(TOKEN_IDENTIFIER)) {
    globalAssignment();
  } else {
    error("An assignment should begin with either an identifier (global) or 'temp' (temporary).");
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
  int jmpEffect = emitJump();
  /* Process identifier */
  consume(TOKEN_IDENTIFIER, "Guardcondition should hold a variable to be tested.");
  consume(TOKEN_SEMICOLON, "Guardcondition should end with ';'.");
  /* Go through effect */
  effect();
  /* Patch jump */
  patchJump(jmpEffect);
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
  while(!match(TOKEN_PROCESS)) {
    globalDeclaration();
  }

  /* Go through processes */
  while(!match(TOKEN_EOF)) {
    process();
  }

  return parser.hadError;
}
