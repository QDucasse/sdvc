#ifndef sdvu_value_h
#define sdvu_value_h

#include "common.h"

/* VM-supported built-in values */
typedef enum {
  VAL_NIL,
  VAL_BOOL,
  VAL_BYTE,
  VAL_INT,
  VAL_STATE
} ValueType;

typedef struct {
  int currentState;
  int stateNumber;
} State;

typedef struct {
  ValueType type;
  union {
    bool boolean;
    uint8_t byte;
    int number;
    State state;
  } as;
  int size;
} Value;


/* Comparison Macros */
#define IS_NIL(value)     ((value).type == VAL_NIL)
#define IS_BOOL(value)    ((value).type == VAL_BOOL)
#define IS_BYTE(value)    ((value).type == VAL_BYTE)
#define IS_INT(value)     ((value).type == VAL_INT)
#define IS_STATE(value)   ((value).type == VAL_STATE)
/* Cast macros */
#define AS_BOOL(value)    ((value).as.boolean)
#define AS_BYTE(value)    ((value).as.byte)
#define AS_INT(value)     ((value).as.number)
#define AS_STATE(value)   ((value).as.state)
/* C to Compiler Macro */
#define NIL_VAL            ((Value){VAL_NIL,   {.number = 0}, 0})
#define BOOL_VAL(value)    ((Value){VAL_BOOL,  {.boolean = value}, sizeof(bool)})
#define BYTE_VAL(value)    ((Value){VAL_BYTE,  {.byte = value}, sizeof(uint8_t)})
#define INT_VAL(value)     ((Value){VAL_INT,   {.number = value}, sizeof(uint16_t)})
#define STATE_VAL(currentState, stateNumber) ((Value){VAL_STATE, {.state.currentState = currentState, .state.stateNumber = stateNumber}, sizeof(int)})

bool valuesEqual(Value a, Value b);
void fprintValue(FILE* outstream, Value value);

#endif
