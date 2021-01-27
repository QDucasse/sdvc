#include <stdlib.h>
#include <string.h>

#include "mmemory.h"
#include "sstring.h"

/* ==================================
          STRING OPERATIONS
====================================*/

/* Compute the hash using the FNV-1a hash function */
static uint32_t hashString(const char* key, int length) {
  uint32_t hash = 2166136261u;

  for (int i = 0; i < length; i++) {
    hash ^= key[i];
    hash *= 16777619;
  }

  return hash;
}

/* Initialize a string */
String* initString() {
  /* Allocate the string in memory */
  String* string = ALLOCATE_OBJ(String);
  string->length = 0;
  string->chars  = NULL;
  string->hash   = 0;
  return string;
}

/* Free an allocated string */
void freeString(String* string) {
  FREE(string);
}

/* Assign a given character array to the string */
void assignString(String* string, char* key, int length) {
  /* Compute the hash */
  uint32_t hash = hashString(key, length);
  /* Fill the String fields */
  string->chars  = key;
  string->length = length;
  string->hash   = hash;
}

/* String comparison */
bool stringsEqual(String* a, String* b) {
  if((a->hash != b->hash) || (a->length != b->length)) return false;
  return memcmp(a->chars, b->chars, a->length) == 0;

}
