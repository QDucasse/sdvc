#include "unity.h"
#include "chunk.h"
#include "mmemory.h"

/* Setup and teardown routine */
void setUp() {}
void tearDown() {}

/* ==================================
          CHUNK OPERATIONS
====================================*/

void testChunkInitialization() {
  Chunk* testChunk = initChunk();
  TEST_ASSERT_EQUAL_INT(0, testChunk->count);
  TEST_ASSERT_EQUAL_INT(0, testChunk->capacity);
  TEST_ASSERT_EQUAL(NULL, testChunk->instructions);
  freeChunk(testChunk);
}

/* Writing an instruction in an empty chunk should raise the capacity to 8 */
void testChunkWriteResizingFromZero() {
  Chunk* testChunk = initChunk();
  uint32_t instruction = 0xFF;
  writeChunk(testChunk, instruction);
  TEST_ASSERT_EQUAL_INT(1, testChunk->count);
  TEST_ASSERT_EQUAL_INT(8, testChunk->capacity);
  TEST_ASSERT_EQUAL_UINT32(0xFF, testChunk->instructions[testChunk->count-1]);
  freeChunk(testChunk);
}

/* Writing an instruction in a chunk with enough capacity should not change it */
void testChunkWriteNoResizing() {
  Chunk* testChunk = initChunk();
  uint32_t* testInstructions = ALLOCATE_ARRAY(uint32_t, 2);
  testInstructions[0] = 0x01;
  testInstructions[1] = 0x01;
  testChunk->count = 2;
  testChunk->capacity = 8; // First capacity augmentation begins at 8
  testChunk->instructions = testInstructions;
  uint32_t instruction = 0xFF;
  writeChunk(testChunk, instruction);
  TEST_ASSERT_EQUAL_INT(3, testChunk->count);
  TEST_ASSERT_EQUAL_INT(8, testChunk->capacity);
  TEST_ASSERT_EQUAL_UINT32(0xFF, testChunk->instructions[testChunk->count-1]);
  freeChunk(testChunk);
}


/* Writing an instruction in a chunk without enough capacity should change it (*2) */
void testChunkWriteResizing() {
  Chunk* testChunk = initChunk();
  uint32_t* testInstructions = ALLOCATE_ARRAY(uint32_t, 8);
  testInstructions[0] = 0x01;
  testChunk->count = 8;
  testChunk->capacity = 8;
  testChunk->instructions = testInstructions;
  uint32_t instruction = 0x09;
  writeChunk(testChunk, instruction);
  TEST_ASSERT_EQUAL_INT(9, testChunk->count);
  TEST_ASSERT_EQUAL_INT(16, testChunk->capacity);
  TEST_ASSERT_EQUAL_UINT32(0x09, testChunk->instructions[testChunk->count-1]);
  freeChunk(testChunk);
}

/* ==================================
      INSTRUCTION OPERATIONS
====================================*/

/* Initialization of the fields */
void testInstructionInitialization() {
  Instruction* instruction = initInstruction();
  TEST_ASSERT_EQUAL_UINT(instruction->op_code,  0);
  TEST_ASSERT_EQUAL_UINT(instruction->cfg_mask, 0);
  TEST_ASSERT_EQUAL_UINT(instruction->rd, 0);
  TEST_ASSERT_EQUAL_UINT(instruction->ra, 0);
  TEST_ASSERT_EQUAL_UINT(instruction->rb, 0);
  TEST_ASSERT_EQUAL_UINT(instruction->imma, 0);
  TEST_ASSERT_EQUAL_UINT(instruction->immb, 0);
  TEST_ASSERT_EQUAL_UINT(instruction->addr, 0);
}

/* RR representation of an instruction */
void testInstructionBinaryRR() {
  Instruction* instruction = initInstruction();
  // Add the content of registers 1 and 2 and put the result in register 15
  for (unsigned int op_code = 0 ; op_code < 13 ; op_code++) {
    uint32_t instructionBits = binaryInstructionRR(instruction, op_code, 15, 1, 2);
    TEST_ASSERT_EQUAL_UINT(instruction->op_code,  op_code);
    TEST_ASSERT_EQUAL_UINT(instruction->cfg_mask, CFG_RR);
    TEST_ASSERT_EQUAL_UINT(instruction->rd, 15);
    TEST_ASSERT_EQUAL_UINT(instruction->ra, 1);
    TEST_ASSERT_EQUAL_UINT(instruction->rb, 2);
    TEST_ASSERT_EQUAL_UINT(instruction->imma, 0);
    TEST_ASSERT_EQUAL_UINT(instruction->immb, 0);
    TEST_ASSERT_EQUAL_UINT(instruction->addr, 0);
    // instruction: opcode(4bits) | cfg mask (00) | rd (1111) | ra (0001) | rb (0010)
    uint32_t expectedBits = op_code << 28 | 0b0011110000000000100000000010;
    TEST_ASSERT_EQUAL_UINT(expectedBits, instructionBits);
  }
}

/* RI representation of a binary instruction */
void testInstructionBinaryRI() {
  Instruction* instruction = initInstruction();
  // Add the content of register 1 and immediate value 255 and put the result in register 15
  for (unsigned int op_code = 0 ; op_code < 13 ; op_code++) {
    uint32_t instructionBits = binaryInstructionRI(instruction, op_code, 15, 1, 255);
    TEST_ASSERT_EQUAL_UINT(instruction->op_code,  op_code);
    TEST_ASSERT_EQUAL_UINT(instruction->cfg_mask, CFG_RI);
    TEST_ASSERT_EQUAL_UINT(instruction->rd, 15);
    TEST_ASSERT_EQUAL_UINT(instruction->ra, 1);
    TEST_ASSERT_EQUAL_UINT(instruction->rb, 0);
    TEST_ASSERT_EQUAL_UINT(instruction->imma, 0);
    TEST_ASSERT_EQUAL_UINT(instruction->immb, 255);
    TEST_ASSERT_EQUAL_UINT(instruction->addr, 0);
    // instruction: opcode(4bits) | cfg mask (01) | rd (1111) | ra (0001) | immb (00011111111)
    uint32_t expectedBits = op_code << 28 | 0b0111110000000000100011111111;
    TEST_ASSERT_EQUAL_UINT(expectedBits, instructionBits);
  }
}

/* IR representation of a binary instruction */
void testInstructionBinaryIR() {
  Instruction* instruction = initInstruction();
  // Add the content of immediate value 255 and register 2 and put the result in register 15
  for (unsigned int op_code = 0 ; op_code < 13 ; op_code++) {
    uint32_t instructionBits = binaryInstructionIR(instruction, op_code, 15, 255, 2);
    TEST_ASSERT_EQUAL_UINT(instruction->op_code,  op_code);
    TEST_ASSERT_EQUAL_UINT(instruction->cfg_mask, CFG_IR);
    TEST_ASSERT_EQUAL_UINT(instruction->rd, 15);
    TEST_ASSERT_EQUAL_UINT(instruction->ra, 0);
    TEST_ASSERT_EQUAL_UINT(instruction->rb, 2);
    TEST_ASSERT_EQUAL_UINT(instruction->imma, 255);
    TEST_ASSERT_EQUAL_UINT(instruction->immb, 0);
    TEST_ASSERT_EQUAL_UINT(instruction->addr, 0);
    // instruction: opcode(4bits) | cfg mask (10) | rd (1111) | imma (00011111111) | rb (0010)
    uint32_t expectedBits = op_code << 28 | 0b1011110001111111100000000010;
    TEST_ASSERT_EQUAL_UINT(expectedBits, instructionBits);
  }
}

/* IR representation of a binary instruction */
void testInstructionBinaryII() {
  Instruction* instruction = initInstruction();
  // Add the content of immediate value 255 and immediate value 255 and put the result in register 15
  for (unsigned int op_code = 0; op_code < 13 ; op_code++) {
    uint32_t instructionBits = binaryInstructionII(instruction, op_code, 15, 255, 255);
    TEST_ASSERT_EQUAL_UINT(instruction->op_code,  op_code);
    TEST_ASSERT_EQUAL_UINT(instruction->cfg_mask, CFG_II);
    TEST_ASSERT_EQUAL_UINT(instruction->rd, 15);
    TEST_ASSERT_EQUAL_UINT(instruction->ra, 0);
    TEST_ASSERT_EQUAL_UINT(instruction->rb, 0);
    TEST_ASSERT_EQUAL_UINT(instruction->imma, 255);
    TEST_ASSERT_EQUAL_UINT(instruction->immb, 255);
    TEST_ASSERT_EQUAL_UINT(instruction->addr, 0);
    // instruction: opcode(4bits) | cfg mask (11) | rd (1111) | imma (00011111111) | immb (00011111111)
    uint32_t expectedBits = op_code << 28 | 0b1111110001111111100011111111;
    TEST_ASSERT_EQUAL_UINT(expectedBits, instructionBits);
  }
}

/* Unary instruction */
void testInstructionUnary() {
  Instruction* instruction = initInstruction();
  // Store the content of register 15 in address 25255
  for (unsigned int op_code = 13 ; op_code < 16 ; op_code++) {
    uint32_t instructionBits = unaryInstruction(instruction, op_code, 15, 25255);
    TEST_ASSERT_EQUAL_UINT(instruction->op_code,  op_code);
    TEST_ASSERT_EQUAL_UINT(instruction->cfg_mask, 0);
    TEST_ASSERT_EQUAL_UINT(instruction->rd, 15);
    TEST_ASSERT_EQUAL_UINT(instruction->ra, 0);
    TEST_ASSERT_EQUAL_UINT(instruction->rb, 0);
    TEST_ASSERT_EQUAL_UINT(instruction->imma, 0);
    TEST_ASSERT_EQUAL_UINT(instruction->immb, 0);
    TEST_ASSERT_EQUAL_UINT(instruction->addr, 25255);
    // instruction: opcode(4bits) | rd (1111) | addr (000000000110001010100111)
    uint32_t expectedBits = op_code << 28 | 0b1111000000000110001010100111;
    TEST_ASSERT_EQUAL_UINT(expectedBits, instructionBits);
  }
}
