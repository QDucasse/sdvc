#include "unity.h"
#include "chunk.h"
#include "disassembler.h"
#include "mmemory.h"
#include "register.h"
#include "value.h"


static Chunk* testChunk;
static Instruction* testInstruction;
static uint32_t testBitInstruction = 0xFF;

/* Setup and teardown routine */
void setUp() {
  testChunk = initChunk();
  testInstruction = initInstruction();
}
void tearDown() {
  freeInstruction(testInstruction);
  freeChunk(testChunk);
}

/* ==================================
          CHUNK OPERATIONS
====================================*/

void testChunkInitialization() {
  TEST_ASSERT_EQUAL_INT(0, testChunk->count);
  TEST_ASSERT_EQUAL_INT(0, testChunk->capacity);
  TEST_ASSERT_EQUAL(NULL, testChunk->instructions);
}

/* Writing an instruction in an empty chunk should raise the capacity to 8 */
void testChunkWriteResizingFromZero() {
  writeChunk(testChunk, testBitInstruction);
  TEST_ASSERT_EQUAL_INT(1, testChunk->count);
  TEST_ASSERT_EQUAL_INT(8, testChunk->capacity);
  TEST_ASSERT_EQUAL_UINT32(testBitInstruction, testChunk->instructions[testChunk->count-1]);
}

/* Writing an instruction in a chunk with enough capacity should not change it */
void testChunkWriteNoResizing() {
  writeChunk(testChunk, 0x01);
  writeChunk(testChunk, 0x02);
  writeChunk(testChunk, testBitInstruction);
  TEST_ASSERT_EQUAL_INT(3, testChunk->count);
  TEST_ASSERT_EQUAL_INT(8, testChunk->capacity);
  TEST_ASSERT_EQUAL_UINT32(testBitInstruction, testChunk->instructions[testChunk->count-1]);
}


/* Writing an instruction in a chunk without enough capacity should change it (*2) */
void testChunkWriteResizing() {
  writeChunk(testChunk, 0x01);
  writeChunk(testChunk, 0x02);
  writeChunk(testChunk, 0x03);
  writeChunk(testChunk, 0x04);
  writeChunk(testChunk, 0x05);
  writeChunk(testChunk, 0x06);
  writeChunk(testChunk, 0x07);
  writeChunk(testChunk, 0x08);
  writeChunk(testChunk, testBitInstruction);
  TEST_ASSERT_EQUAL_INT(9, testChunk->count);
  TEST_ASSERT_EQUAL_INT(16, testChunk->capacity);
  TEST_ASSERT_EQUAL_UINT32(testBitInstruction, testChunk->instructions[testChunk->count-1]);
}

/* ==================================
      INSTRUCTION OPERATIONS
====================================*/

/* Initialization of the fields */
void testInstructionInitialization() {
  TEST_ASSERT_EQUAL_UINT(testInstruction->op_code,  0);
  TEST_ASSERT_EQUAL_UINT(testInstruction->cfg_mask, 0);
  TEST_ASSERT_EQUAL_UINT(testInstruction->rd, 0);
  TEST_ASSERT_EQUAL_UINT(testInstruction->ra, 0);
  TEST_ASSERT_EQUAL_UINT(testInstruction->rb, 0);
  TEST_ASSERT_EQUAL_UINT(testInstruction->imma, 0);
  TEST_ASSERT_EQUAL_UINT(testInstruction->immb, 0);
  TEST_ASSERT_EQUAL_UINT(testInstruction->addr, 0);
}

/* Binary Instructions
=================== */

/* RR representation of an instruction */
void testInstructionBinaryRR() {
  // Add the content of registers 1 and 2 and put the result in register 15
  for (unsigned int op_code = 0 ; op_code < 13 ; op_code++) {
    uint32_t instructionBits = binaryInstructionRR(testInstruction, op_code, 15, 1, 2);
    TEST_ASSERT_EQUAL_UINT(testInstruction->op_code,  op_code);
    TEST_ASSERT_EQUAL_UINT(testInstruction->cfg_mask, CFG_RR);
    TEST_ASSERT_EQUAL_UINT(testInstruction->rd, 15);
    TEST_ASSERT_EQUAL_UINT(testInstruction->ra, 1);
    TEST_ASSERT_EQUAL_UINT(testInstruction->rb, 2);
    TEST_ASSERT_EQUAL_UINT(testInstruction->imma, 0);
    TEST_ASSERT_EQUAL_UINT(testInstruction->immb, 0);
    TEST_ASSERT_EQUAL_UINT(testInstruction->addr, 0);
    // instruction: opcode(4bits) | cfg mask (00) | rd (1111) | ra (0001) | rb (0010)
    uint32_t expectedBits = op_code << 28 | 0b0011110000000000100000000010;
    TEST_ASSERT_EQUAL_UINT(expectedBits, instructionBits);
  }
}

/* RI representation of a binary instruction */
void testInstructionBinaryRI() {
  // Add the content of register 1 and immediate value 255 and put the result in register 15
  for (unsigned int op_code = 0 ; op_code < 13 ; op_code++) {
    uint32_t instructionBits = binaryInstructionRI(testInstruction, op_code, 15, 1, 255);
    TEST_ASSERT_EQUAL_UINT(testInstruction->op_code,  op_code);
    TEST_ASSERT_EQUAL_UINT(testInstruction->cfg_mask, CFG_RI);
    TEST_ASSERT_EQUAL_UINT(testInstruction->rd, 15);
    TEST_ASSERT_EQUAL_UINT(testInstruction->ra, 1);
    TEST_ASSERT_EQUAL_UINT(testInstruction->rb, 0);
    TEST_ASSERT_EQUAL_UINT(testInstruction->imma, 0);
    TEST_ASSERT_EQUAL_UINT(testInstruction->immb, 255);
    TEST_ASSERT_EQUAL_UINT(testInstruction->addr, 0);
    // instruction: opcode(4bits) | cfg mask (01) | rd (1111) | ra (0001) | immb (00011111111)
    uint32_t expectedBits = op_code << 28 | 0b0111110000000000100011111111;
    TEST_ASSERT_EQUAL_UINT(expectedBits, instructionBits);
  }
}

/* IR representation of a binary instruction */
void testInstructionBinaryIR() {
  // Add the content of immediate value 255 and register 2 and put the result in register 15
  for (unsigned int op_code = 0 ; op_code < 13 ; op_code++) {
    uint32_t instructionBits = binaryInstructionIR(testInstruction, op_code, 15, 255, 2);
    TEST_ASSERT_EQUAL_UINT(testInstruction->op_code,  op_code);
    TEST_ASSERT_EQUAL_UINT(testInstruction->cfg_mask, CFG_IR);
    TEST_ASSERT_EQUAL_UINT(testInstruction->rd, 15);
    TEST_ASSERT_EQUAL_UINT(testInstruction->ra, 0);
    TEST_ASSERT_EQUAL_UINT(testInstruction->rb, 2);
    TEST_ASSERT_EQUAL_UINT(testInstruction->imma, 255);
    TEST_ASSERT_EQUAL_UINT(testInstruction->immb, 0);
    TEST_ASSERT_EQUAL_UINT(testInstruction->addr, 0);
    // instruction: opcode(4bits) | cfg mask (10) | rd (1111) | imma (00011111111) | rb (0010)
    uint32_t expectedBits = op_code << 28 | 0b1011110001111111100000000010;
    TEST_ASSERT_EQUAL_UINT(expectedBits, instructionBits);
  }
}

/* IR representation of a binary instruction */
void testInstructionBinaryII() {
  // Add the content of immediate value 255 and immediate value 255 and put the result in register 15
  for (unsigned int op_code = 0; op_code < 13 ; op_code++) {
    uint32_t instructionBits = binaryInstructionII(testInstruction, op_code, 15, 255, 255);
    TEST_ASSERT_EQUAL_UINT(testInstruction->op_code,  op_code);
    TEST_ASSERT_EQUAL_UINT(testInstruction->cfg_mask, CFG_II);
    TEST_ASSERT_EQUAL_UINT(testInstruction->rd, 15);
    TEST_ASSERT_EQUAL_UINT(testInstruction->ra, 0);
    TEST_ASSERT_EQUAL_UINT(testInstruction->rb, 0);
    TEST_ASSERT_EQUAL_UINT(testInstruction->imma, 255);
    TEST_ASSERT_EQUAL_UINT(testInstruction->immb, 255);
    TEST_ASSERT_EQUAL_UINT(testInstruction->addr, 0);
    // instruction: opcode(4bits) | cfg mask (11) | rd (1111) | imma (00011111111) | immb (00011111111)
    uint32_t expectedBits = op_code << 28 | 0b1111110001111111100011111111;
    TEST_ASSERT_EQUAL_UINT(expectedBits, instructionBits);
  }
}

/* Unary Instructions (STORE/JMP)
============================== */

/* Unary instruction */
void testInstructionUnary() {
  // Store the content of register 15 in address 25255
  for (unsigned int op_code = 13 ; op_code < 15 ; op_code++) {
    uint32_t instructionBits = unaryInstruction(testInstruction, op_code, 15, 25255);
    TEST_ASSERT_EQUAL_UINT(testInstruction->op_code,  op_code);
    TEST_ASSERT_EQUAL_UINT(testInstruction->cfg_mask, 0);
    TEST_ASSERT_EQUAL_UINT(testInstruction->rd, 15);
    TEST_ASSERT_EQUAL_UINT(testInstruction->ra, 0);
    TEST_ASSERT_EQUAL_UINT(testInstruction->rb, 0);
    TEST_ASSERT_EQUAL_UINT(testInstruction->imma, 0);
    TEST_ASSERT_EQUAL_UINT(testInstruction->immb, 0);
    TEST_ASSERT_EQUAL_UINT(testInstruction->addr, 25255);
    // instruction: opcode(4bits) | rd (1111) | addr (000000000110001010100111)
    uint32_t expectedBits = op_code << 28 | 0b1111000000000110001010100111;
    TEST_ASSERT_EQUAL_UINT(expectedBits, instructionBits);
  }
}

/* LOAD instruction
================ */

/* Load instruction with REG configuration */
void testInstructionLoadReg() {
  uint32_t instructionBits = loadInstructionReg(testInstruction, 15, 12);
  TEST_ASSERT_EQUAL_UINT(testInstruction->op_code,  OP_LOAD);
  TEST_ASSERT_EQUAL_UINT(testInstruction->cfg_mask, LOAD_REG);
  TEST_ASSERT_EQUAL_UINT(testInstruction->rd, 15);
  TEST_ASSERT_EQUAL_UINT(testInstruction->ra, 12);
  TEST_ASSERT_EQUAL_UINT(testInstruction->rb, 0);
  TEST_ASSERT_EQUAL_UINT(testInstruction->imma, 0);
  TEST_ASSERT_EQUAL_UINT(testInstruction->immb, 0);
  TEST_ASSERT_EQUAL_UINT(testInstruction->addr, 0);
  // instruction: opcode(4bits) | cfg(2bits) | rd(1111) | ra(1010)
  uint32_t expectedBits = OP_LOAD << 28 | 0b0011110000000000000000001100;
  TEST_ASSERT_EQUAL_UINT(expectedBits, instructionBits);
}

/* Load instruction with IMM configuration */
void testInstructionLoadImm() {
  uint32_t instructionBits = loadInstructionImm(testInstruction, 15, 255);
  TEST_ASSERT_EQUAL_UINT(testInstruction->op_code,  OP_LOAD);
  TEST_ASSERT_EQUAL_UINT(testInstruction->cfg_mask, LOAD_IMM);
  TEST_ASSERT_EQUAL_UINT(testInstruction->rd, 15);
  TEST_ASSERT_EQUAL_UINT(testInstruction->ra, 0);
  TEST_ASSERT_EQUAL_UINT(testInstruction->rb, 0);
  TEST_ASSERT_EQUAL_UINT(testInstruction->imma, 255);
  TEST_ASSERT_EQUAL_UINT(testInstruction->immb, 0);
  TEST_ASSERT_EQUAL_UINT(testInstruction->addr, 0);
  // instruction: opcode(4bits) | cfg(2bits) | rd(1111) | imma(11111111)
  uint32_t expectedBits = OP_LOAD << 28 | 0b0111110000000000000011111111;
  TEST_ASSERT_EQUAL_UINT(expectedBits, instructionBits);
}

/* Load instruction with ADR configuration */
void testInstructionLoadAdr() {
  uint32_t instructionBits = loadInstructionAddr(testInstruction, 15, 25255);
  TEST_ASSERT_EQUAL_UINT(testInstruction->op_code,  OP_LOAD);
  TEST_ASSERT_EQUAL_UINT(testInstruction->cfg_mask, LOAD_ADR);
  TEST_ASSERT_EQUAL_UINT(testInstruction->rd, 15);
  TEST_ASSERT_EQUAL_UINT(testInstruction->ra, 0);
  TEST_ASSERT_EQUAL_UINT(testInstruction->rb, 0);
  TEST_ASSERT_EQUAL_UINT(testInstruction->imma, 0);
  TEST_ASSERT_EQUAL_UINT(testInstruction->immb, 0);
  TEST_ASSERT_EQUAL_UINT(testInstruction->addr, 25255);
  // instruction: opcode(4bits) | cfg(2bits) | rd(1111) | addr(110001010100111)
  uint32_t expectedBits = OP_LOAD << 28 | 0b1011110000000110001010100111;
  TEST_ASSERT_EQUAL_UINT(expectedBits, instructionBits);
}
