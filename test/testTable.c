#include "unity.h"
#include "mmemory.h"
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
  Entry* entry1 = allocateEntry(key1, INT_VAL(1));
  Entry* entry2 = allocateEntry(key2, INT_VAL(2));
  uint32_t index1 = entry1->key->hash % 8;
  uint32_t index2 = entry2->key->hash % 8;
  testEntries[index1] = *entry1;
  testEntries[index2] = *entry2;
  Entry* outEntry = findEntry(testEntries, 8, key1);
  printf("%s\n", testEntries[1].key->chars);
  printf("%s\n", outEntry->key->chars);
  TEST_ASSERT_EQUAL(entry1, outEntry);
}

/* Size Operation
============== */


/* Table entry manipulation
========================= */
void testTableSet() {
  Table* table = initTable();
  String* key = allocateString("blip1", 5);
  Value value = INT_VAL(1);
  Entry* entry = allocateEntry(key, value);
  tableSet(table, key, value);
  TEST_ASSERT_EQUAL_INT(8, table->capacity);
  TEST_ASSERT_EQUAL(&table->entries[key->hash % 8], entry);
}
