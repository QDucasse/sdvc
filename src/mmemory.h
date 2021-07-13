#ifndef sdvu_memory_h
#define sdvu_memory_h

#include "common.h"

/* Allocate an object of a given type */
#define ALLOCATE_OBJ(type) \
    (type*)reallocate(NULL, sizeof(type))

/* Allocate an array with a given element type and count */
#define ALLOCATE_ARRAY(type, count) \
    (type*)reallocate(NULL, sizeof(type) * (count))

#define FREE(pointer) reallocate(pointer, 0)

#define FREE_ARRAY(type, pointer, count) \
  reallocate(pointer, sizeof(type) * (count))

/* The capacity becomes either 8 if the old one was smaller
or doubles the amount. 8 is arbitrary but should be looked
against real-world usage. */
#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2)

/* Extends an array of "type" */
#define GROW_ARRAY(type, pointer, newCount) \
    (type*)reallocate(pointer, sizeof(type) * (newCount))

/*
Only function in the compiler that allocates memory, frees it and
changes the size of allocated chunks. It knows what operation is going on through:
| Pointer  | newSize   |      Function      |
|----------+-----------+--------------------|
| NULL     | non-0     | Allocate new block |
| non-NULL | 0         | Free allocation    |
*/
void* reallocate(void* pointer, size_t newSize);

#endif
