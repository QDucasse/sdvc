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

void printRegister(Register* reg) {
  printf("Register number %d containing variable named %s with value '", reg->number, reg->varName->chars);
  printValue(reg->varValue);
  printf("' at address %u\n", reg->address);
}

void loadVariable(Register* reg, String* varName, Value varValue, uint32_t varAddress) {
  reg->varName = varName;
  reg->varValue = varValue;
  reg->address = varAddress;
}
