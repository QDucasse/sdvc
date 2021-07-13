#include <stdlib.h>
#include <string.h>

#include "mmemory.h"
#include "register.h"
#include "table.h"
#include "value.h"

/* Grow the array when it reaches 75% of its capacity */
#define TABLE_MAX_LOAD 0.75

/* Allocate the memory for an entry */
Entry* initEntry() {
  Entry* entry = ALLOCATE_OBJ(Entry);
  entry->key = NULL;
  entry->value = NIL_VAL;
  entry->address = 0;
  return entry;
}


/* Assign actual values to the entry */
void assignEntry(Entry* entry, String* key, Value value, uint32_t address) {
  entry->key = key;
  entry->value = value;
  entry->address = address;
}


/* Free the memory of an entry */
void freeEntry(Entry* entry) {
  FREE(entry);
}


/* Initialize the hash table */
Table* initTable() {
  Table* table = ALLOCATE_OBJ(Table);
  table->count = 0;
  table->capacity = 0;
  table->entries = NULL;
  table->currentAddress = 0;
  return table;
}


/* Free the hash table */
void freeTable(Table* table) {
  FREE(table->entries);
  FREE(table);
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
    } else if (stringsEqual(entry->key, key)) {
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
    entries[i].address = 0;
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
    dest->address = entry->address;
    table->count++; /* Increment if non-tombstone */
  }

  /* Set the new entries and capacity */
  FREE(table->entries);
  table->entries = entries;
  table->capacity = capacity;
}


/* ==================================
      TABLE ENTRY MANIPULATION
====================================*/

/* Set the value at the given key */
void tableSet(Table* table, String* key, Value value, uint32_t address) {
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
  entry->address = address;
}


/* Copy the content of a hash table into another */
void tableAddAll(Table* from, Table* to) {
  for (int i = 0; i < from->capacity; i++) {
    Entry* entry = &from->entries[i];
    if (entry->key != NULL) {
      tableSet(to, entry->key, entry->value, entry->address);
    }
  }
}


/* Get an entry for a given key and store the value in the corresponding pointers */
bool tableGet(Table* table, String* key, Value* value, uint32_t* address) {
  /* The table is empty */
  if(table->count == 0) return false;
  /* Look for the entry corresponding to a given key */
  Entry* entry = findEntry(table->entries, table->capacity, key);
  if (entry->key == NULL) return false;

  *value   = entry->value;
  *address = entry->address;
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
  entry->address = 0;

  return true;
}


/* ==================================
    TABLE TO REGISTER OPERATIONS
====================================*/

bool tableGetToRegister(Table* table, String* key, Register* reg) {
  reg->varName = key;
  return tableGet(table, key, &reg->varValue, &reg->address);
}

void tableSetFromRegister(Table* table, Register* reg) {
  return tableSet(table, reg->varName, reg->varValue, reg->address);
}
