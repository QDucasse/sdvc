#ifndef sdvu_compiler_h
#define sdvu_compiler_h

#include "scanner.h"
#include "string.h"
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

/* Compiler structure */
typedef struct {
  Table globals; /* Hash table of the global values (configuration input and output) */

  Register registers[REG_NUMBER]; /* Array of registers behaving like a stack */
  Register* topRegister; /* Pointer to the first register available */

  uint16_t* instructions; /* Array of instructions */
} Compiler;

/* Compiler singleton */
Compiler compiler;

/* Allocation/Deallocation routine */
void initCompiler();
void freeCompiler();

/* Compile routine */
bool compile(const char* source);

#endif
