#include <stdlib.h>

#include "mmemory.h"

/* ======================
      REALLOCATION
====================== */

/* Memory reallocation */
void* reallocate(void* pointer, size_t oldSize, size_t newSize) {
  /* Free option */
  if (newSize == 0) {
    free(pointer);
    return NULL;
  }
  /* Allocation options */
  void* result = realloc(pointer, newSize);
  /* In case there isnt enough memory */
  if (result == NULL) exit(1);
  return result;
}
