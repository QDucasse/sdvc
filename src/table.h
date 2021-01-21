#ifndef sdvu_table_h
#define sdvu_table_h

#include "common.h"
#include "sstring.h"
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

Table* initTable();
void freeTable(Table* table);
bool tableGet(Table* table, String* key, Value* value);
void tableSet(Table* table, String* key, Value value);
bool tableDelete(Table* table, String* key);
void tableAddAll(Table* from, Table* to);

#endif
