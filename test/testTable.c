#include "unity.h"
#include "mmemory.h"
#include "register.h"
#include "sstring.h"
#include "table.h"
#include "table.c"
#include "value.h"


static Table* testTable;
static String* key1;
static String* key2;
static Entry* entry1;
static Entry* entry2;
static Register* reg1;

/* Setup and teardown routine */
void setUp() {
  testTable = initTable();
  key1 = initString();
  key2 = initString();
  entry1 = initEntry();
  entry2 = initEntry();
  reg1 = initRegister(3);
}
void tearDown() {
  freeTable(testTable);
  freeString(key1);
  freeString(key2);
  freeEntry(entry1);
  freeEntry(entry2);
  freeRegister(reg1);
}

/* Initialization
============== */

void testTableInitialization() {
  TEST_ASSERT_EQUAL_INT(0, testTable->count);
  TEST_ASSERT_EQUAL_INT(0, testTable->capacity);
  TEST_ASSERT_EQUAL(NULL, testTable->entries);
}

/* Entry lookup
============ */

void testFindEntry() {
  Entry testEntries[8];
  assignString(key1, "blip1", 5);
  assignString(key2, "blip2", 5);
  assignEntry(entry1, key1, INT_VAL(1), 0xF);
  assignEntry(entry2, key2, INT_VAL(2), 0xFF);
  uint32_t index1 = entry1->key->hash % 8;
  uint32_t index2 = entry2->key->hash % 8;
  testEntries[index1] = *entry1;
  testEntries[index2] = *entry2;
  Entry* outEntry = findEntry(testEntries, 8, key1);
  TEST_ASSERT_EQUAL_STRING(entry1->key->chars, outEntry->key->chars);
  TEST_ASSERT_EQUAL_UINT32(entry1->address, outEntry->address);
  TEST_ASSERT_TRUE(valuesEqual(entry1->value, outEntry->value));
}

/* Size Operation
============== */

void testAdjustCapacityOnCreation() {
  TEST_ASSERT_EQUAL_INT(0, testTable->capacity);
  adjustCapacity(testTable, GROW_CAPACITY(testTable->capacity));
  TEST_ASSERT_EQUAL_INT(8, testTable->capacity);
}

/* Table entry manipulation
========================= */

void testTableSet() {
  assignString(key1, "blip1", 5);
  Value value = INT_VAL(1);
  uint32_t address = 0xFF;
  assignEntry(entry1, key1, value, address);
  tableSet(testTable, key1, value, address);
  TEST_ASSERT_EQUAL_INT(8, testTable->capacity);
  Entry insertedEntry = testTable->entries[key1->hash % 8];
  TEST_ASSERT_EQUAL_STRING(key1, insertedEntry.key);
  TEST_ASSERT_TRUE(valuesEqual(value, insertedEntry.value));
  TEST_ASSERT_EQUAL_UINT32(address, insertedEntry.address);
}

void testTableGet() {
  assignString(key1, "blip1", 5);
  Value value = INT_VAL(1);
  assignEntry(entry1, key1, value, 0xF);
  Value outValue = NIL_VAL;
  uint32_t outAddress = 0;
  /* Find value with no entry */
  bool notFound = tableGet(testTable, key1, &outValue, &outAddress);
  TEST_ASSERT_FALSE(notFound);

  /* Find value with entry */
  tableSet(testTable, key1, value, 0xF);
  bool found = tableGet(testTable, key1, &outValue, &outAddress);
  TEST_ASSERT_TRUE(found);
}

void testTableDelete() {
  assignString(key1, "blip1", 5);
  assignString(key2, "blip2", 5);
  Value value = INT_VAL(1);
  assignEntry(entry1, key1, value, 0xF);

  /* Delete in empty table */
  bool notFound = tableDelete(testTable, key1);
  TEST_ASSERT_FALSE(notFound);

  /* Delete unknown value*/
  tableSet(testTable, key1, value, 0xFF);
  notFound = tableDelete(testTable, key2);
  TEST_ASSERT_FALSE(notFound);

  /* Delete known value*/
  bool found = tableDelete(testTable, key1);
  Entry removedEntry = testTable->entries[key1->hash % 8];
  TEST_ASSERT_TRUE(found);
  TEST_ASSERT_EQUAL(NULL, removedEntry.key);
  TEST_ASSERT_TRUE(valuesEqual(BOOL_VAL(true), removedEntry.value));
}

/* Register to table operations
============================ */

void testTableGetToRegister() {
  /* Set entry */
  assignString(key1, "blip1", 5);
  Value value = INT_VAL(1);
  uint32_t address = 0xFF;
  tableSet(testTable, key1, value, address);

  tableGetToRegister(testTable, key1, reg1);
  TEST_ASSERT_EQUAL_STRING(key1, reg1->varName);
  TEST_ASSERT_TRUE(valuesEqual(value, reg1->varValue));
  TEST_ASSERT_EQUAL_UINT32(address, reg1->address);
  printRegister(reg1);
}

void testTableSetFromRegister() {
  assignString(key1, "blip1", 5);
  Value value = INT_VAL(1);
  uint32_t address = 0xFF;
  loadVariable(reg1, key1, value, address);

  tableSetFromRegister(testTable, reg1);
  Entry insertedEntry = testTable->entries[key1->hash % 8];
  TEST_ASSERT_EQUAL_STRING(reg1->varName, insertedEntry.key);
  TEST_ASSERT_TRUE(valuesEqual(reg1->varValue, insertedEntry.value));
  TEST_ASSERT_EQUAL_UINT32(reg1->address, insertedEntry.address);
  printRegister(reg1);
}
