#include <stdlib.h>

#include "chunk.h"
#include "mmemory.h"


/* ==================================
          CHUNK OPERATIONS
====================================*/

/* Allocation/Deallocation
======================= */

/* Chunk Initialization */
Chunk* initChunk() {
  Chunk* chunk = ALLOCATE_OBJ(Chunk);
  chunk->count = 0;
  chunk->capacity = 0;
  chunk->instructions = NULL;
  return chunk;
}

/* Free the given chunk */
void freeChunk(Chunk* chunk) {
  /* Free the array "code" */
  FREE_ARRAY(uint32_t, chunk->instructions, chunk->capacity);
  /* Free the structure */
  FREE(Chunk, chunk);
}

/* Writing
======= */

/* Write an instruction to the given chunk*/
void writeChunk(Chunk* chunk, uint32_t instruction) {
  /* Check if the current array has room for a new instruction */
  if (chunk->capacity < chunk->count + 1) {
    int oldCapacity = chunk->capacity;
    /* Figure out the new capacity */
    chunk->capacity = GROW_CAPACITY(oldCapacity);
    /* Grow the array for the amount of capacity */
    chunk->instructions = GROW_ARRAY(uint32_t, chunk->instructions, oldCapacity, chunk->capacity);
  }

  /* Add the byte of code to the chunk */
  chunk->instructions[chunk->count] = instruction;
  chunk->count++;
}

/* ==================================
      INSTRUCTION OPERATIONS
====================================*/

/* Initialization and conversion
============================= */

/*Initialization - Zero the different terms */
Instruction* initInstruction() {
  Instruction* instruction = ALLOCATE_OBJ(Instruction);
  instruction->op_code  = 0;
  instruction->cfg_mask = 0;
  instruction->rd = 0;
  instruction->ra = 0;
  instruction->rb = 0;
  instruction->imma = 0;
  instruction->immb = 0;
  instruction->addr = 0;
  return instruction;
}

/* Conversion from Instruction struct to uint32_t */
static uint32_t instructionToUint32(Instruction* instruction) {
  /* Adding op code to the instruction*/
  uint32_t convertedInstruction = instruction->op_code << 28;
  bool isBinary = instruction->op_code < 13;
  if (isBinary) {
    /* Adding config mask */
    convertedInstruction |= instruction->cfg_mask << 26;
    /* Adding destination register */
    convertedInstruction |= instruction->rd << 22;
    /* Using the bitmask to choose the configuration */
    switch(instruction->cfg_mask) {

      case CFG_RR:
        /* Adding Ra */
        convertedInstruction |= instruction->ra << 11;
        /* Adding Rb */
        convertedInstruction |= instruction->rb;
        break;

      case CFG_RI:
        /* Adding Ra */
        convertedInstruction |= instruction->ra << 11;
        /* Adding ImmB */
        convertedInstruction |= instruction->immb;
        break;

      case CFG_IR:
        /* Adding ImmA */
        convertedInstruction |= instruction->imma << 11;
        /* Adding Rb */
        convertedInstruction |= instruction->rb;
        break;

      case CFG_II:
        /* Adding ImmA */
        convertedInstruction |= instruction->imma << 11;
        /* Adding ImmB */
        convertedInstruction |= instruction->immb;
        break;

      default: break; // Unreachable
    }

  } else {
    /* Adding destination register */
    convertedInstruction |= instruction->rd << 24;
    /* Adding address */
    convertedInstruction |= instruction->addr;
  }

  return convertedInstruction;

}

/* Instruction filling
=================== */

/* Fill the instruction with information for RR then export to uint32 */
uint32_t binaryInstructionRR(Instruction* instruction, unsigned int op_code,
                             unsigned int rd, unsigned int ra, unsigned int rb) {
  instruction->op_code = op_code;
  instruction->cfg_mask = CFG_RR;
  instruction->rd = rd;
  instruction->ra = ra;
  instruction->rb = rb;
  return instructionToUint32(instruction);

}

/* Fill the instruction with information for RI then export to uint32 */
uint32_t binaryInstructionRI(Instruction* instruction, unsigned int op_code,
                             unsigned int rd, unsigned int ra, unsigned int imm) {
  instruction->op_code = op_code;
  instruction->cfg_mask = CFG_RI;
  instruction->rd = rd;
  instruction->ra = ra;
  instruction->immb = imm;
  return instructionToUint32(instruction);
}

/* Fill the instruction with information for IR then export to uint32 */
uint32_t binaryInstructionIR(Instruction* instruction, unsigned int op_code,
                             unsigned int rd, unsigned int imm, unsigned int rb) {
  instruction->op_code = op_code;
  instruction->cfg_mask = CFG_IR;
  instruction->rd = rd;
  instruction->imma = imm;
  instruction->rb = rb;
  return instructionToUint32(instruction);
}

/* Fill the instruction with information for II then export to uint32 */
uint32_t binaryInstructionII(Instruction* instruction, unsigned int op_code,
                         unsigned int rd, unsigned int imma, unsigned int immb) {
  instruction->op_code = op_code;
  instruction->cfg_mask = CFG_II;
  instruction->rd = rd;
  instruction->imma = imma;
  instruction->immb = immb;
  return instructionToUint32(instruction);
}

/* Fill the instruction with a destination register and address for STORE, LOAD or JMP then export to uint32_t */
uint32_t unaryInstruction(Instruction* instruction, unsigned int op_code,
                          unsigned int rd, unsigned int addr) {
  instruction->op_code = op_code;
  instruction->rd = rd;
  instruction->addr = addr;
  return instructionToUint32(instruction);
}
