#include "unity.h"
#include "mmemory.h"
#include "sstring.h"
#include "table.h"
#include "table.c"
#include "value.h"


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
  freeTable(table);
}

/* Entry lookup
============ */

void testFindEntry() {
  Entry testEntries[8];
  String* key1 = initString();
  assignString(key1, "blip1", 5);
  String* key2 = initString();
  assignString(key2, "blip2", 5);
  Entry* entry1 = initEntry();
  assignEntry(entry1, key1, INT_VAL(1));
  Entry* entry2 = initEntry();
  assignEntry(entry2, key2, INT_VAL(2));
  uint32_t index1 = entry1->key->hash % 8;
  uint32_t index2 = entry2->key->hash % 8;
  testEntries[index1] = *entry1;
  testEntries[index2] = *entry2;
  Entry* outEntry = findEntry(testEntries, 8, key1);
  // ACTUAL POINTERS ARE NOT EQUAL FOR AN UNKNOWN REASON
  // TEST_ASSERT_EQUAL(entry1, outEntry);
  TEST_ASSERT_EQUAL_STRING(entry1->key->chars, outEntry->key->chars);
  TEST_ASSERT_TRUE(valuesEqual(entry1->value, outEntry->value));

  /* Free allocated objects */
  freeString(key1);
  freeString(key2);
  freeEntry(entry1);
  freeEntry(entry2);
}

/* Size Operation
============== */

void testAdjustCapacityOnCreation() {
  Table* table = initTable();
  TEST_ASSERT_EQUAL_INT(0, table->capacity);
  adjustCapacity(table, GROW_CAPACITY(table->capacity));
  TEST_ASSERT_EQUAL_INT(8, table->capacity);
  freeTable(table);
}

/* Table entry manipulation
========================= */

void testTableSet() {
  Table* table = initTable();
  String* key = initString();
  assignString(key, "blip1", 5);
  Value value = INT_VAL(1);
  Entry* entry = initEntry();
  assignEntry(entry, key, value);
  tableSet(table, key, value);
  TEST_ASSERT_EQUAL_INT(8, table->capacity);
  // ACTUAL POINTERS ARE NOT EQUAL FOR AN UNKNOWN REASON
  // TEST_ASSERT_EQUAL(&table->entries[key->hash % 8], entry);
  Entry insertedEntry = table->entries[key->hash % 8];
  TEST_ASSERT_EQUAL_STRING(key, insertedEntry.key);
  TEST_ASSERT_TRUE(valuesEqual(value, insertedEntry.value));

  /* Free allocated objects */
  freeTable(table);
  freeString(key);
  freeEntry(entry);
}

void testTableGet() {
  Table* table = initTable();
  String* key = initString();
  assignString(key, "blip1", 5);
  Value value = INT_VAL(1);
  Entry* entry = initEntry();
  assignEntry(entry, key, value);
  Value outValue = NIL_VAL;

  /* Find value with no entry */
  bool notFound = tableGet(table, key, &outValue);
  TEST_ASSERT_FALSE(notFound);

  /* Find value with entry */
  tableSet(table, key, value);
  bool found = tableGet(table, key, &outValue);
  TEST_ASSERT_TRUE(found);

  /* Free allocated objects */
  freeTable(table);
  freeString(key);
  freeEntry(entry);
}

void testTableDelete() {
  Table* table = initTable();
  String* key1 = initString();
  assignString(key1, "blip1", 5);
  String* key2 = initString();
  assignString(key2, "blip2", 5);
  Value value = INT_VAL(1);
  Entry* entry = initEntry();
  assignEntry(entry, key1, value);

  /* Delete in empty table */
  bool notFound = tableDelete(table, key1);
  TEST_ASSERT_FALSE(notFound);

  /* Delete unknown value*/
  tableSet(table, key1, value);
  notFound = tableDelete(table, key2);
  TEST_ASSERT_FALSE(notFound);

  /* Delete known value*/
  bool found = tableDelete(table, key1);
  Entry removedEntry = table->entries[key1->hash % 8];
  TEST_ASSERT_TRUE(found);
  TEST_ASSERT_EQUAL(NULL, removedEntry.key);
  TEST_ASSERT_TRUE(valuesEqual(BOOL_VAL(true), removedEntry.value));

  /* Free allocated objects */
  freeTable(table);
  freeString(key1);
  freeString(key2);
  freeEntry(entry);
}
