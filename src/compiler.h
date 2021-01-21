#ifndef sdvu_compiler_h
#define sdvu_compiler_h

#include "chunk.h"
#include "scanner.h"
#include "sstring.h"
#include "table.h"
#include "value.h"

/* ==================================
        STRUCTS AND GLOBALS
=================================== */

#define REG_NUMBER 16

/* Parser structure */
typedef struct {
  Token current;  /* current Token being investigated */
  Token previous; /* next Token being investigated */
  Token tempCurrent;  /* Current token around temporaries */
  Token tempPrevious; /* Preivous token around temporaries */
  bool hadError;  /* Previous error was encountered */
  bool panicMode; /* To avoid cascading errors */
} Parser;

/* Parser singleton */
Parser parser;

/* Register structure */
typedef struct {
  String* varName; /* Name of the variable in the register */
  Value* varValue; /* Value of the variable in the register */
  int number; /* Register number */
} Register;

Chunk* chunk; /* Array of instructions */

/* Compiler structure */
typedef struct {
  Table globals; /* Hash table of the global values (configuration input and output) */

  Register registers[REG_NUMBER]; /* Array of registers behaving like a stack */
  Register* topRegister; /* Pointer to the first register available */
} Compiler;

/* Compiler singleton */
Compiler compiler;

/* Allocation/Deallocation routine */
void initCompiler();
void freeCompiler();

/* Compile routine */
bool compile(char* source);

#endif
