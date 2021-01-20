#include <stdlib.h>

#include "chunk.h"
#include "memory.h"

/* Chunk Initialization */
Chunk* initChunk() {
  Chunk* chunk = (Chunk*)reallocate(NULL, 0, sizeof(Chunk));
  chunk->count = 0;
  chunk->capacity = 0;
  chunk->instructions = NULL;
  return chunk;
}

/* Free the given chunk */
void freeChunk(Chunk* chunk) {
  /* Free the array "code" */
  FREE_ARRAY(uint32_t, chunk->instructions, chunk->capacity);
  /* Remove the count and capacity */
  initChunk(chunk);
}

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
