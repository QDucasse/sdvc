#include <stdlib.h>

#include "common.h"
#include "chunk.h"
#include "disassembler.h"
#include "mmemory.h"
#include "register.h"


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
  /* Free the instructions array */
  FREE(chunk->instructions);
  /* Free the actual structure */
  FREE(chunk);
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
    chunk->instructions = GROW_ARRAY(uint32_t, chunk->instructions, chunk->capacity);
  }

  /* Add the byte of code to the chunk */
  chunk->instructions[chunk->count] = instruction;
  chunk->count++;
}


/* ==================================
      INSTRUCTION OPERATIONS
================================== */

unsigned int typeCfg(ValueType type) {
    unsigned int typeCodes[] = {
      [VAL_BOOL]  = 0b00,
      [VAL_BYTE]  = 0b01,
      [VAL_INT]   = 0b10,
      [VAL_STATE] = 0b11
    };
    return typeCodes[type];
}

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
  instruction->type = 0;
  return instruction;
}


/* Free the given instruction */
void freeInstruction(Instruction* instruction) {
  FREE(instruction);
}


/* Conversion from Instruction struct to uint32_t */
uint32_t instructionToUint32(Instruction* instruction) {
  /* Adding op code to the instruction*/
  uint32_t convertedInstruction = instruction->op_code << 28;
  switch(instruction->op_code) {
      /* NOP AND ENDGA OPERATIONS
      ======================== */
      case OP_NOP: break;
      case OP_ENDGA: break;

      /* LOAD OPERATION
      ============== */
      case OP_LOAD: {
          /* LOAD OPERATION */
          /* Adding config mask */
          convertedInstruction |= instruction->cfg_mask << 26;
          /* Adding type information */
          convertedInstruction |= instruction->type << 24;
          /* Adding destination register */
          convertedInstruction |= instruction->rd << 20;
          switch (instruction->cfg_mask) {
              case LOAD_REG:
              case LOAD_RAA:
                  /* Adding Ra */
                  convertedInstruction |= instruction->ra;
                  break;
              case LOAD_IMM:
                  /* Adding Imma */
                  convertedInstruction |= instruction->imma;
                  break;
              case LOAD_ADR:
                  /* Adding address */
                  convertedInstruction |= instruction->addr;
                  break;
              default:
                  break;// Unreachable
          }
          break;
      }

      /* STORE OPERATION
      =============== */
      case OP_STORE: {
          /* STORE OPERATION */
          convertedInstruction |= instruction->cfg_mask << 26;
          /* Adding type information */
          convertedInstruction |= instruction->type << 24;
          /* Adding destination register */
          convertedInstruction |= instruction->rd << 20;
          switch (instruction->cfg_mask) {
              case STORE_ADR:
                  /* Adding address */
                  convertedInstruction |= instruction->addr;
              case STORE_RAA:
                  /* Adding register */
                  convertedInstruction |= instruction->ra;
              default:
                  break; // Unreachable
          }
          break;
      }

      /* JUMP OPERATION
      ============== */
      case OP_JMP: {
          /* JMP OPERATION */
          /* Adding destination register (register holding value to test here) */
          convertedInstruction |= instruction->rd << 24;
          /* Adding address */
          convertedInstruction |= instruction->addr;
          break;
      }

      /* NOT OPERATION
      ============= */
      case OP_NOT: {
          /* JMP OPERATION */
          /* Adding destination register (register holding value to test here) */
          convertedInstruction |= instruction->rd << 24;
          /* Adding address */
          convertedInstruction |= instruction->ra;
          break;
      }
      /* BINARY OPERATION
      ================ */
      default: {
          /* BINARY OPERATIONS */
          /* Adding config mask */
          convertedInstruction |= instruction->cfg_mask << 26;
          /* Adding destination register */
          convertedInstruction |= instruction->rd << 22;
          /* Using the bitmask to choose the configuration */
          switch (instruction->cfg_mask) {
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
            default:
                break; // Unreachable
          }
          break;
      }
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

/* Fill the instruction with a destination register and address for STORE then export to uint32_t */
uint32_t storeInstruction(Instruction* instruction,  unsigned int rd, unsigned int addr, unsigned int type) {
  instruction->op_code = OP_STORE;
  instruction->rd = rd;
  instruction->addr = addr;
  instruction->type = type;
  return instructionToUint32(instruction);
}

/* Fill the instruction with a destination register to test and address for JMP then export to uint32_t */
uint32_t jumpInstruction(Instruction* instruction,  unsigned int rd, unsigned int addr) {
  instruction->op_code = OP_JMP;
  instruction->rd = rd;
  instruction->addr = addr;
  return instructionToUint32(instruction);
}

/* LOAD instructions
================= */

/* Fill the instruction with a destination register and address for LOAD then export to uint32_t */
uint32_t loadInstructionReg(Instruction* instruction, unsigned int rd, unsigned int ra) {
  instruction->op_code = OP_LOAD;
  instruction->cfg_mask = LOAD_REG;
  instruction->rd = rd;
  instruction->ra = ra;
  return instructionToUint32(instruction);
}


/* Fill the instruction with a destination register and address for LOAD then export to uint32_t */
uint32_t loadInstructionImm(Instruction* instruction, unsigned int rd, unsigned int imma) {
  instruction->op_code = OP_LOAD;
  instruction->cfg_mask = LOAD_IMM;
  instruction->rd = rd;
  instruction->imma = imma;
  return instructionToUint32(instruction);
}


/* Fill the instruction with a destination register and address for LOAD then export to uint32_t */
uint32_t loadInstructionAddr(Instruction* instruction, unsigned int rd, unsigned int addr, unsigned int type) {
  instruction->op_code = OP_LOAD;
  instruction->cfg_mask = LOAD_ADR;
  instruction->rd = rd;
  instruction->addr = addr;
  instruction ->type = type;
  return instructionToUint32(instruction);
}

/* Fill the instruction with a destination register a register containing an address for LOAD then export to uint32_t */
uint32_t loadInstructionRegAsAddr(Instruction* instruction, unsigned int rd, unsigned int ra, unsigned int type) {
    instruction->op_code = OP_LOAD;
    instruction->cfg_mask = LOAD_RAA;
    instruction->rd = rd;
    instruction->ra = ra;
    instruction ->type = type;
    return instructionToUint32(instruction);
}

uint32_t notInstruction(Instruction* instruction, unsigned int rd) {
    instruction->op_code = OP_NOT;
    instruction->rd = rd;
    instruction->ra = rd;
    return instructionToUint32(instruction);
}

/* Direct Write
============ */

/* Writes a store instruction from a register */
void writeStoreFromRegister(Register* reg, Chunk* chunk) {
  Instruction* strInstruction = initInstruction();
  uint32_t bitStoreInstruction = storeInstruction(strInstruction, reg->number, reg->address, typeCfg(reg->varValue.type));
  disassembleInstruction(bitStoreInstruction);
  writeChunk(chunk, bitStoreInstruction);
  emptyRegister(reg);
  freeInstruction(strInstruction);
}


/* Writes a load instruction from a register */
void writeLoadFromRegister(Register* reg, Chunk* chunk) {
  Instruction* loadInstruction = initInstruction();
  uint32_t bitLoadInstruction = loadInstructionAddr(loadInstruction, reg->number, reg->address, typeCfg(reg->varValue.type));
  disassembleInstruction(bitLoadInstruction);
  writeChunk(chunk, bitLoadInstruction);
  freeInstruction(loadInstruction);
}

/* NOP operation
============= */
uint32_t nopInstruction(Instruction* instruction) {
  instruction->op_code = OP_NOP;
  return instructionToUint32(instruction);
}


/* ENDGA operation
=============== */

uint32_t endGAInstruction(Instruction* instruction) {
  instruction->op_code = OP_ENDGA;
  return instructionToUint32(instruction);
}
