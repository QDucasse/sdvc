#ifndef sdvu_register_h
#define sdvu_register_h

#include "mmemory.h"
#include "sstring.h"
#include "value.h"

#define REG_NUMBER 16

/* Register structure */
typedef struct {
  String* varName; /* Name of the variable in the register */
  Value varValue;  /* Value of the variable in the register */
  int number;      /* Register number */
} Register;

/* Register initialization */
Register* initRegister(int number);
/* Free a given register */
void freeRegister(Register* reg);
/* Load a variable in a register */
void loadVariable(Register* reg, String* varName, Value varValue);

#endif
