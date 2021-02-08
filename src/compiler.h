#ifndef sdvu_compiler_h
#define sdvu_compiler_h

#include "chunk.h"
#include "mmemory.h"
#include "register.h"
#include "scanner.h"
#include "sstring.h"
#include "table.h"
#include "value.h"

/* ==================================
        STRUCTS AND GLOBALS
=================================== */

/* Compiler structure */
typedef struct {
  Table* globals; /* Hash table of the global values (configuration input and output) */
  Chunk* chunk;   /* Chunk of memory containing the instructions */
  Register* registers;       /* Array of registers behaving like a stack */
  Register* topTempRegister; /* Pointer to the first register available for temporary variables */
  Register* topGlobRegister; /* Pointer to the first register available for global variables */
  uint32_t pc;    /* Program counter */
} Compiler;

/* Compiler singleton */
Compiler* compiler;

/* Allocation/Deallocation routine */
void initCompiler();
void freeCompiler();

/* Compile routine */
bool compile(char* source);

#endif
