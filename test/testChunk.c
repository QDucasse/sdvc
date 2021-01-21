#include "unity.h"
#include "chunk.h"
#include "mmemory.h"

/* Setup and teardown routine */
void setUp() {}
void tearDown() {}

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
