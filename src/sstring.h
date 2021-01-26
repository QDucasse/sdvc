#ifndef sdvu_string_h
#define sdvu_string_h

#include "common.h"

/* Structure of a string (object) */
typedef struct {
  uint32_t hash; /* Hash of the string */
  int length;    /* Length of the string */
  char* chars;   /* Characters composing the string */
} String;

/* Initialize the string */
String* initString();
/* Assign a character array to the string */
void assignString(String* string, char* key, int length);
/* String comparison */
bool stringsEqual(String* string1, String* string2);

#endif
