#include <string.h>
#include "unity.h"
#include "sstring.h"
#include "sstring.c"
#include "mmemory.h"

/* Setup and teardown routine */
void setUp() {}
void tearDown() {}

void testHashString() {
  TEST_ASSERT_EQUAL_UINT32(1706468258, hashString("blip", 4)); // FNV-1a hash
}

void testAllocateString() {
  String* string = allocateString("blip", 4);
  TEST_ASSERT_EQUAL_INT(4, string->length);
  TEST_ASSERT_EQUAL_STRING("blip", string->chars);
  TEST_ASSERT_EQUAL_UINT32(1706468258, string->hash);
}
