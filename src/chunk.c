#include <stdlib.h>

#include "chunk.h"
#include "memory.h"

/* Chunk Initialization */
void initChunk(Chunk* chunk) {
  chunk->count = 0;
  chunk->capacity = 0;
  chunk->instructions = NULL;
  chunk->lines = NULL;
}

/* Free the given chunk */
void freeChunk(Chunk* chunk) {
  /* Free the array "code" */
  FREE_ARRAY(uint32_t, chunk->instructions, chunk->capacity);
  /* Free the array "lines" */
  FREE_ARRAY(int, chunk->lines, chunk->capacity);
  /* Remove the count and capacity */
  initChunk(chunk);
}

/* Write an instruction to the given chunk*/
void writeChunk(Chunk* chunk, uint32_t instruction, int line) {
  /* Check if the current array has room for a new instruction */
  if (chunk->capacity < chunk->count + 1) {
    int oldCapacity = chunk->capacity;
    /* Figure out the new capacity */
    chunk->capacity = GROW_CAPACITY(oldCapacity);
    /* Grow the array for the amount of capacity */
    chunk->instructions = GROW_ARRAY(uint32_t, chunk->instructions, oldCapacity, chunk->capacity);
    /* Grow the line array as well */
    chunk->lines = GROW_ARRAY(int, chunk->lines, oldCapacity, chunk->capacity);
  }

  /* Add the byte of code to the chunk */
  chunk->instructions[chunk->count] = instruction;
  chunk->lines[chunk->count] = line;
  chunk->count++;
}
