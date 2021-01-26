#include <string.h>
#include "unity.h"
#include "sstring.h"
#include "sstring.c"
#include "mmemory.h"

/* Setup and teardown routine */
void setUp() {}
void tearDown() {}

/* Test hash function */
void testHashString() {
  TEST_ASSERT_EQUAL_UINT32(1706468258, hashString("blip", 4)); // FNV-1a hash
}

/* Test string initialization */
void testStringInitialization() {
  String* string = initString();
  TEST_ASSERT_EQUAL_INT(0, string->length);
  TEST_ASSERT_EQUAL_STRING(NULL, string->chars);
  TEST_ASSERT_EQUAL_UINT32(0, string->hash);
}

/* Test string assignment */
void testStringAssignment() {
  String* string = initString();
  assignString(string, "blip", 4);
  TEST_ASSERT_EQUAL_INT(4, string->length);
  TEST_ASSERT_EQUAL_STRING("blip", string->chars);
  TEST_ASSERT_EQUAL_UINT32(1706468258, string->hash);
}

/* Test string comparison */
void testStringEqualTrue() {
  String* a = initString();
  assignString(a, "blip", 4);
  String* b = initString();
  assignString(b, "blip", 4);
  TEST_ASSERT_TRUE(stringsEqual(a, b));
}

/* Test string comparison */
void testStringEqualFalse() {
  String* a = initString();
  assignString(a, "blip", 4);
  String* b = initString();
  assignString(b, "bloup", 5);
  TEST_ASSERT_FALSE(stringsEqual(a, b));
}
