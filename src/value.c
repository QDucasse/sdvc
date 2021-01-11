#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "value.h"

/* Print Value */
void printValue(Value value) {
  switch (value.type) {
    case VAL_NIL: printf("nil"); break;
    case VAL_BOOL:
      printf(AS_BOOL(value) ? "true" : "false");
      break;
    case VAL_BYTE: printf("%u", AS_BYTE(value)); break;
    case VAL_NUMBER: printf("%g", AS_NUMBER(value)); break;
  }
}

/* Compare values first by comparing type then the actual values */
bool valuesEqual(Value a, Value b) {
  if (a.type != b.type) return false;

  switch (a.type) {
    case VAL_NIL:    return true;
    case VAL_BOOL:   return AS_BOOL(a) == AS_BOOL(b);
    case VAL_BYTE:   return AS_BYTE(a) == AS_BYTE(b);
    case VAL_NUMBER: return AS_NUMBER(a) == AS_NUMBER(b);
    default:
      return false; /* Unreachable */
  }
}
