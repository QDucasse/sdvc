#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "value.h"

/* ==================================
           VALUE ARRAYS
=================================== */

/* Initialization of a ValueArray */
void initValueArray(ValueArray* array) {
  array->values = NULL;
  array->capacity = 0;
  array->count = 0;
}

/* Write a value in the ValueArray with automated growth if needed */
void writeValueArray(ValueArray* array, Value value) {
  if(array->capacity < array->count+1) {
    int oldCapacity = array->capacity;
    array->capacity = GROW_CAPACITY(oldCapacity);
    array->values = GROW_ARRAY(Value, array->values, oldCapacity, array->capacity);
  }

  array->values[array->count] = value;
  array->count++;
}

/* Free value array */
void freeValueArray(ValueArray* array) {
  FREE_ARRAY(Value, array->values, array->capacity);
  initValueArray(array);
}

/* ==================================
             UTILITIES
=================================== */

/* Print Value */
void printValue(Value value) {
  switch (value.type) {
    case VAL_NIL: printf("nil"); break;
    case VAL_BOOL:
      printf(AS_BOOL(value) ? "true" : "false");
      break;
    case VAL_BYTE: printf("%u", AS_BYTE(value)); break;
    case VAL_INT: printf("%g", AS_INT(value)); break;
    case VAL_STATE: printf("state %d out of %d", AS_STATE(value).currentState, AS_STATE(value).stateNumber); break;
  }
}

/* Compare values first by comparing type then the actual values */
bool valuesEqual(Value a, Value b) {
  if (a.type != b.type) return false;

  switch (a.type) {
    case VAL_NIL:    return true;
    case VAL_BOOL:   return AS_BOOL(a) == AS_BOOL(b);
    case VAL_BYTE:   return AS_BYTE(a) == AS_BYTE(b);
    case VAL_INT:    return AS_INT(a) == AS_INT(b);
    default:
      return false; /* Unreachable */
  }
}
