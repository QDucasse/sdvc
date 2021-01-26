#include "common.h"
#include "register.h"
#include "sstring.h"

/* Register initialization */
Register* initRegister(int number) {
  Register* reg = ALLOCATE_OBJ(Register);
  reg->varName = initString();
  reg->varValue = NIL_VAL;
  reg->number = number;
  return reg;
}
/* Load a temporary variable */
void loadVariable(Register* reg, String* varName, Value varValue){
  reg->varName  = varName;
  reg->varValue = varValue;
}
