#include <stdlib.h>
#include <string.h>

#include "mmemory.h"
#include "table.h"
#include "value.h"

/* Grow the array when it reaches 75% of its capacity */
#define TABLE_MAX_LOAD 0.75

Entry* allocateEntry(String* key, Value value) {
  Entry* entry = ALLOCATE_OBJ(Entry);
  entry->key = key;
  entry->value = value;
  return entry;
}

/* Initialize the hash table */
Table* initTable() {
  Table* table = ALLOCATE_OBJ(Table);
  table->count = 0;
  table->capacity = 0;
  table->entries = NULL;
  return table;
}

/* Free the hash table */
void freeTable(Table* table) {
  FREE_ARRAY(Entry, table->entries, table->capacity);
  initTable(table);
}

/* ==================================
  FIND AN ENTRY IN ARRAY OF ENTRIES
====================================*/

/* Find an entry corresponding to a given key, returns a pointer to the entry */
static Entry* findEntry(Entry* entries, int capacity, String* key) {
  /* Map of the hash to the table using the modulo of the capacity */
  uint32_t index = key->hash % capacity;
  Entry* tombstone = NULL;
  for (;;) {
    /* Return the entry at the given index in the table */
    Entry* entry = &(entries[index]);

    if (entry->key == NULL) {
      if (IS_NIL(entry->value)) {
        /* Empty entry */
        return tombstone != NULL ? tombstone : entry;
      } else {
        /* Tombstone found */
        if (tombstone == NULL) tombstone = entry;
      }
    } else if (entry->key == key) {
      /* Key found */
      return entry;
    }

    /* Linear probing to look for another place */
    index = (index + 1) % capacity;
  }
}

/* ==================================
          SIZE OPERATIONS
====================================*/

/* Allocate an array of entries */
static void adjustCapacity(Table* table, int capacity) {
  Entry* entries = ALLOCATE_ARRAY(Entry, capacity);
  /* Fill the entries in the allocated array with NULL keys and NIL values*/
  for (int i = 0; i < capacity; i++) {
    entries[i].key = NULL;
    entries[i].value = NIL_VAL;
  }

  /* Clear the tombstone count */
  table->count = 0;

  /* Report the entries from the old table to the new one */
  for (int i = 0; i < table->capacity; i++) {
    Entry* entry = &table->entries[i];
    if (entry->key == NULL) continue;

    /* The new table is reused from scratch to handle potential collisions */
    Entry* dest = findEntry(entries, capacity, entry->key);
    dest->key = entry->key;
    dest->value = entry->value;
    table->count++; /* Increment if non-tombstone */
  }

  /* Release the memory from the old array */
  FREE_ARRAY(Entry, table->entries, table->capacity);
  table->entries = entries;
  table->capacity = capacity;
}

/* ==================================
      TABLE ENTRY MANIPULATION
====================================*/

/* Set the value at the given key */
void tableSet(Table* table, String* key, Value value) {
  if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
    int capacity = GROW_CAPACITY(table->capacity);
    adjustCapacity(table, capacity);
  }

  /* Look for the key in the table */
  Entry* entry = findEntry(table->entries, table->capacity, key);
  /* The count is incremented if the key is not overwriting a tombstone*/
  if(IS_NIL(entry->value)) table->count++;

  entry->key = key;
  entry->value = value;
}

/* Copy the content of a hash table into another */
void tableAddAll(Table* from, Table* to) {
  for (int i = 0; i < from->capacity; i++) {
    Entry* entry = &from->entries[i];
    if (entry->key != NULL) {
      tableSet(to, entry->key, entry->value);
    }
  }
}

/* Get an entry for a given key and store the value in the corresponding pointers */
bool tableGet(Table* table, String* key, Value* value) {
  /* The table is empty */
  if(table->count == 0) return false;
  /* Look for the entry corresponding to a given key */
  Entry* entry = findEntry(table->entries, table->capacity, key);
  if (entry->key == NULL) return false;

  *value = entry->value;
  return true;
}

/* Delete an entry for a given key */
bool tableDelete(Table* table, String* key) {
  /* The table is empty */
  if(table->count == 0) return false;

  /* Find the entry */
  Entry* entry = findEntry(table->entries, table->capacity, key);
  if(entry->key == NULL) return false;

  /* Place a tombstone in the entry */
  entry->key = NULL;
  entry->value = BOOL_VAL(true); /* Tombstone is true */

  return true;
}
