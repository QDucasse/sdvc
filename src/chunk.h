
#ifndef sdvu_instruction_h
#define sdvu_instruction_h

#include "common.h"
#include "value.h"

/* OPCODE constants */
#define OP_ADD   0b0000
#define OP_SUB   0b0001
#define OP_MUL   0b0010
#define OP_DIV   0b0011
#define OP_MOD   0b0100
#define OP_AND   0b0101
#define OP_OR    0b0110
#define OP_LT    0b0111
#define OP_LTEQ  0b1000
#define OP_GT    0b1001
#define OP_GTEQ  0b1010
#define OP_EQ    0b1011
#define OP_NEQ   0b1100
#define OP_JMP   0b1101
#define OP_STORE 0b1110
#define OP_LOAD  0b1111

/* CONFIG bitmasks */
#define CFG_RRR  0b00
#define CFG_RRI  0b01
#define CFG_RIR  0b10
#define CFG_RII  0b11

/* Instruction definition */
typedef struct {
  int count;              // Number of allocated entries in use
  int capacity;           // Size of the chunk
  uint32_t* instructions; // Actual raw 32-bits instructions
  int* lines;             // Line number of the corresponding instructions
} Chunk;

/* Initialize a given chunk */
void initChunk(Chunk* chunk);
/* Free the given chunk */
void freeChunk(Chunk* chunk);
/* Write an instruction to the given chunk */
void writeChunk(Chunk* chunk, uint32_t, int line);


#endif
