#include <string.h>
#include "unity.h"
#include "sstring.h"
#include "sstring.c"
#include "mmemory.h"

static String* testString;
static String* testString2;

/* Setup and teardown routine */
void setUp() {
  testString = initString();
  testString2 = initString();
}
void tearDown() {
  freeString(testString);
  freeString(testString2);
}

/* Test hash function */
void testHashString() {
  TEST_ASSERT_EQUAL_UINT32(1706468258, hashString("blip", 4)); // FNV-1a hash
}

/* Test string initialization */
void testStringInitialization() {
  TEST_ASSERT_EQUAL_INT(0, testString->length);
  TEST_ASSERT_EQUAL_STRING(NULL, testString->chars);
  TEST_ASSERT_EQUAL_UINT32(0, testString->hash);
}

/* Test string assignment */
void testStringAssignment() {
  assignString(testString, "blip", 4);
  TEST_ASSERT_EQUAL_INT(4, testString->length);
  TEST_ASSERT_EQUAL_STRING("blip", testString->chars);
  TEST_ASSERT_EQUAL_UINT32(1706468258, testString->hash);
}

/* Test string comparison */
void testStringEqualTrue() {
  assignString(testString, "blip", 4);
  assignString(testString2, "blip", 4);
  TEST_ASSERT_TRUE(stringsEqual(testString, testString2));
}

/* Test string comparison */
void testStringEqualFalse() {
  assignString(testString, "blip", 4);
  assignString(testString2, "bloup", 5);
  TEST_ASSERT_FALSE(stringsEqual(testString, testString2));
}
