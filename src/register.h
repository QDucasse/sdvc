#ifndef sdvu_register_h
#define sdvu_register_h

#include "mmemory.h"
#include "sstring.h"
#include "value.h"

#define REG_NUMBER 15

/* Register structure */
typedef struct {
  String* varName;  /* Name of the variable in the register */
  Value varValue;   /* Value of the variable in the register */
  int number;       /* Register number */
  uint32_t address; /* Store the address in case of a global variable */
} Register;

/* Register initialization */
Register* initRegister(int number);
/* Empty a given register */
void emptyRegister(Register* reg);
/* Free a given register */
void freeRegister(Register* reg);
/* Variable loading */
void loadVariable(Register* reg, String* varName, Value varValue, uint32_t varAddress);
/* Printing utility */
void printRegister(Register* reg);

#endif
