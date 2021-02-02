#ifndef sdvu_table_h
#define sdvu_table_h

#include "common.h"
#include "sstring.h"
#include "value.h"

/* Entry representation, key and value */
typedef struct {
  String* key;
  Value value;
  uint32_t address;
} Entry;

/* Entry operations */
Entry* initEntry();
void assignEntry(Entry* entry, String* key, Value value, uint32_t address);
void freeEntry(Entry* entry);


/* Table structure, dynamic array of entries */
typedef struct {
  int count;      /* Number of used entries + TOMBSTONES */
  int capacity;   /* Size of the table */
  Entry* entries; /* Actual entries */
  uint32_t currentAddress; /* Cumulated address of the variables in the table */
} Table;

/* Table operations */
Table* initTable();
void freeTable(Table* table);
bool tableGet(Table* table, String* key, Value* value, uint32_t* address);
void tableSet(Table* table, String* key, Value value, uint32_t address);
bool tableDelete(Table* table, String* key);
void tableAddAll(Table* from, Table* to);

#endif
