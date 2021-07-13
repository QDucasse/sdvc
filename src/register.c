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


/* Empty register */
void emptyRegister(Register* reg) {
  if (reg->varName != NULL) freeString(reg->varName);
  reg->varName = NULL;
  reg->varValue = NIL_VAL;
  reg->address = 0;
}


/* Free a given register */
void freeRegister(Register* reg) {
    FREE(reg);
}

/* Textual representation of a given register */
void fprintRegister(FILE* outstream, Register* reg) {
  fprintf(outstream, "Register number %d containing variable named %s with value '", reg->number, reg->varName->chars);
  fprintValue(outstream, reg->varValue);
  fprintf(outstream, "' at address %u\n", reg->address);
}

/* Load a variable in a given register */
void loadVariable(Register* reg, String* varName, Value varValue, uint32_t varAddress) {
  reg->varName = varName;
  reg->varValue = varValue;
  reg->address = varAddress;
}
