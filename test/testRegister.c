#include "unity.h"
#include "common.h"
#include "mmemory.h"
#include "register.h"
#include "sstring.h"
#include "value.h"

/* Setup and teardown routine */
void setUp() {}
void tearDown() {}

/* Register Initialization */
void testRegisterInitialization() {
  Register* reg = initRegister(3);
  String* expected = initString();
  TEST_ASSERT_TRUE(stringsEqual(expected, reg->varName));
  TEST_ASSERT_TRUE(valuesEqual(NIL_VAL, reg->varValue));
  TEST_ASSERT_EQUAL_INT(3, reg->number);
}

/* Loading values
============== */

/* Load variable */
void testRegisterLoadVariable() {
  Register* reg = initRegister(4);
  String* key = initString();
  assignString(key, "t_256", 5);
  loadVariable(reg, key, BOOL_VAL(false));
  TEST_ASSERT_TRUE(stringsEqual(key, reg->varName));
  TEST_ASSERT_TRUE(valuesEqual(BOOL_VAL(false), reg->varValue));
}
