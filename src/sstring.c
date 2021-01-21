#include <stdlib.h>
#include <string.h>

#include "memory.h"
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

/* Allocate a string */
String* allocateString(char* chars, int length) {
  /* Compute the hash */
  uint32_t hash  = hashString(chars, length);
  /* Allocate and fill the actual string */
  String* string = (String*)reallocate(NULL, 0, sizeof(String));
  string->length = length;
  string->chars  = chars;
  string->hash   = hash;

  return string;
}
