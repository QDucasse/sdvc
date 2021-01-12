#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "compiler.h"
#include "scanner.h"

/* ==================================
          EXECUTION METHODS
====================================*/

/* Read a file and return the contents in a buffer */
static char* readFile(const char* path) {
  /* Open the file */
  FILE* file = fopen(path, "rb");
  if (file == NULL) {
    fprintf(stderr, "Could not open file \"%s\".\n", path);
    exit(74);
  }

  /* Look for the end of the file */
  fseek(file, 0L, SEEK_END);
  /* Number of bytes since the start of the file */
  size_t fileSize = ftell(file);
  /* Return to the beginning */
  rewind(file);

  /* Allocate enough memory to store the content of the file */
  char* buffer = (char*)malloc(fileSize + 1);
  if (buffer == NULL) {
    fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
    exit(74);
  }

  /* Read the bytes from the content of the file */
  size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
  if (bytesRead < fileSize) {
    fprintf(stderr, "Could not read file \"%s\".\n", path);
    exit(74);
  }

  buffer[bytesRead] = '\0';
  fclose(file);
  return buffer;
}

/* Interpret a given file */
// static void scanFile(const char* path) {
//   char* source = readFile(path);
//   initScanner(source);
//   Token token;
//   for (;;) {
//     token = scanToken();
//     // printToken(token);
//     if (token.type == TOKEN_EOF) {
//       printf("%s: ", path);
//       printf("File scanned\n");
//       break;
//     }
//
//   }
//   free(source);
// }

/* ==================================
               MAIN
====================================*/

int main(int argc, char* argv[]) {
  if (argc == 2) {
    // scanFile(argv[1]);
    char* source = readFile(argv[1]);
    compile(source);
  } else {
    fprintf(stderr, "Usage: sdvu path\n");
    exit(64);
  }
  return 0;
}
