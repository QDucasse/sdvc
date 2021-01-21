#ifndef sdvu_string_h
#define sdvu_string_h

/* Structure of a string (object) */
typedef struct {
  uint32_t hash; /* Hash of the string */
  int length;    /* Length of the string */
  char* chars;   /* Charaters composing the string */
} String;

String* allocateString(char* chars, int length);

#endif
