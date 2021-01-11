#ifndef sdvu_compiler_h
#define sdvu_compiler_h

#include "scanner.h"
#include "table.h"
#include "value.h"

/* ==================================
        STRUCTS AND GLOBALS
=================================== */

/* Parser structure */
typedef struct {
  Token current;  /* current Token being investigated */
  Token previous; /* next Token being investigated */
  bool hadError;  /* Previous error was encountered */
  bool panicMode; /* To avoid cascading errors */
} Parser;

/* Parser singleton */
Parser parser;

/* Compiler structure */
typedef struct {
  Table globals; /* Hash table of the global values (configuration input and output) */
  // list of registers (not needed?)
  int topRegister; /* Index of the first available register */
} Compiler;

/* Compiler singleton */
Compiler compiler;

/* Allocation/Deallocation routine */
void initCompiler();
void freeCompiler();

#endif
