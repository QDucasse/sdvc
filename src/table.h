#ifndef sdvu_table_h
#define sdvu_table_h

#include "common.h"
#include "string.h"
#include "value.h"

/* Entry representation, key and value */
typedef struct {
  String* key;
  Value value;
} Entry;

/* Table structure, dynamic array of entries */
typedef struct {
  int count;      /* Number of used entries + TOMBSTONES */
  int capacity;   /* Size of the table */
  Entry* entries; /* Actual entries */
} Table;

void initTable(Table* table);
void freeTable(Table* table);
bool tableGet(Table* table, String* key, Value* value);
bool tableSet(Table* table, String* key, Value value);
bool tableDelete(Table* table, String* key);
void tableAddAll(Table* from, Table* to);

#endif
