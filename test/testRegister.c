#include "unity.h"
#include "common.h"
#include "mmemory.h"
#include "register.h"
#include "sstring.h"
#include "value.h"


static Register* reg;
static String* key;

/* Setup and teardown routine */
void setUp() {
  reg = initRegister(3);
  key = initString();
  assignString(key, "t_256", 5);
}
void tearDown() {
  freeRegister(reg);
  freeString(key);
}

/* Register Initialization */
void testRegisterInitialization() {
  TEST_ASSERT_EQUAL(NULL, reg->varName);
  TEST_ASSERT_TRUE(valuesEqual(NIL_VAL, reg->varValue));
  TEST_ASSERT_EQUAL_INT(3, reg->number);
  TEST_ASSERT_EQUAL_UINT32(0, reg->address);
}

/* Variable loading */
void testVariableLoading() {
  Value value = INT_VAL(1);
  uint32_t address = 0xFF;
  loadVariable(reg, key, value, address);
  TEST_ASSERT_TRUE(stringsEqual(key, reg->varName));
  TEST_ASSERT_TRUE(valuesEqual(INT_VAL(1), reg->varValue));
  TEST_ASSERT_EQUAL_INT(3, reg->number);
  TEST_ASSERT_EQUAL_UINT32(0xFF, reg->address);
}
