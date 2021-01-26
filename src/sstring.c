#include <stdlib.h>
#include <string.h>

#include "mmemory.h"
#include "sstring.h"

/* ==================================
           STRING CREATION
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

void allocateString(String* string, char* key, int length) {
  /* Compute the hash */
  uint32_t hash = hashString(key, length);
  /* Fill the String fields */
  string->chars  = key;
  string->length = length;
  string->hash   = hash;
}

/* Allocate a string */
String* initString() {
  /* Allocate the string */
  String* string = ALLOCATE_OBJ(String);
  string->length = 0;
  string->chars  = NULL;
  string->hash   = 0;
  return string;
}
