#ifndef sdvu_string_h
#define sdvu_string_h

/* Structure of a string (object) */
typedef struct {
  int length;  /* Length of the string */
  char* chars; /* Charaters composing the string */
  uint32_t hash; /* Hash of the string */
} String;

String* allocateString(char* chars, int length);

#endif
