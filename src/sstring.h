#ifndef sdvu_string_h
#define sdvu_string_h

/* Structure of a string (object) */
typedef struct {
  uint32_t hash; /* Hash of the string */
  int length;    /* Length of the string */
  char* chars;   /* Characters composing the string */
} String;

/* Initialize the string */
String* initString();
/* */
void allocateString(String* string, char* key, int length);

#endif
