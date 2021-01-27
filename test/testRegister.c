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
}

/* Loading values
============== */

/* Load variable */
void testRegisterLoadVariable() {
  loadVariable(reg, key, BOOL_VAL(false));
  TEST_ASSERT_TRUE(stringsEqual(key, reg->varName));
  TEST_ASSERT_TRUE(valuesEqual(BOOL_VAL(false), reg->varValue));
}
