#include "unity.h"
#include "memory.h"
#include "sstring.h"
#include "table.h"
#include "table.c"


/* Setup and teardown routine */
void setUp() {}
void tearDown() {}

/* Initialization
============== */
void testTableInitialization() {
  Table* table = initTable();
  TEST_ASSERT_EQUAL_INT(0, table->count);
  TEST_ASSERT_EQUAL_INT(0, table->capacity);
  TEST_ASSERT_EQUAL(NULL, table->entries);
}

/* Entry lookup
============ */
void testFindEntry() {
  Entry testEntries[8];
  String* key1 = allocateString("blip1", 5);
  String* key2 = allocateString("blip2", 5);
  Entry entry1 = {key1, INT_VAL(1)};
  Entry entry2 = {key2, INT_VAL(2)};
  uint32_t index1 = entry1.key->hash % 8;
  uint32_t index2 = entry2.key->hash % 8;
  printf("%u\n",index1);
  printf("%u\n",index2);
  testEntries[index1] = entry1;
  testEntries[index2] = entry2;
  printf("%s\n",testEntries[0].key->chars);
  printf("%s\n",findEntry(testEntries, 8, key1)->key->chars);
  TEST_ASSERT_EQUAL(&entry1, findEntry(testEntries, 8, key1));
}

/* Size Operation
============== */
