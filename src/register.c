#include "common.h"
#include "register.h"
#include "sstring.h"

/* Register initialization */
Register* initRegister(int number) {
  Register* reg = ALLOCATE_OBJ(Register);
  reg->varName = NULL;
  reg->varValue = NIL_VAL;
  reg->number = number;
  reg->address = 0;
  return reg;
}

/* Free a given register */
void freeRegister(Register* reg) {
  FREE(reg);
}
