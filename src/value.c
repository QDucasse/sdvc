#include <stdio.h>
#include <string.h>

#include "mmemory.h"
#include "value.h"

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
