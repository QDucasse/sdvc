#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compiler.h"
#include "scanner.h"

FILE* outstream;

/* ==================================
          EXECUTION METHODS
====================================*/

/* File handling
============= */

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

/* Actions
======= */

/* Scan a given file */
static void scanFile(const char* path, FILE* outstream) {
  char* source = readFile(path);
  initScanner(source);
  Token token;
  for (;;) {
    token = scanToken();
    printToken(token);
    if (token.type == TOKEN_EOF) {
      fprintf(outstream, "File scanned\n");
      break;
    }
  }
  free(source);
}

/* Compiler a given file */
static void compileFile(const char* path, FILE* outstream, bool verbose) {
  initCompiler();
  char* source = readFile(path);
  compile(source, outstream, verbose);
  freeCompiler();
  free(source);
}

/* Disassemble a given file */
static void disassembleFile(const char* path, FILE* outstream, bool verbose) {
  char* source = readFile(path);
  free(source);
}

/* ==================================
               MAIN
====================================*/

int main(int argc, char *argv[])
{
  /* Default values */
  bool verbose = false;
  /* Default string values */
  char* compileTarget = NULL;
  char* disassembleTarget = NULL;
  char* scanTarget = NULL;
  /* Default output stream */
  outstream = stdout;

  /* Modes */
  enum {
    ERROR_MODE,
    COMPILE_MODE,
    DISASSEMBLE_MODE,
    SCAN_MODE
  } mode = ERROR_MODE;

  /* Parsing arguments */
  size_t optind;
  for (optind = 1; optind < argc && argv[optind][0] == '-'; optind++) {
    switch (argv[optind][1]) {
    case 'c': {
      mode = COMPILE_MODE;
      compileTarget = argv[optind + 1];
      break;
    }
    case 'd': {
      mode = DISASSEMBLE_MODE;
      disassembleTarget = argv[optind + 1];
      break;
    }
    case 's': {
      mode = SCAN_MODE;
      scanTarget = argv[optind + 1];
      break;
    }
    case 'o': {
      outstream = fopen(argv[optind + 1], "w");
      break;
    }
    case 'v': verbose = true; break;
    default:
      fprintf(stderr, "Usage: %s [-cds] [file...]\n", argv[0]);
      exit(64);
    }
  }

  /* Using arguments */
  switch (mode) {
    case COMPILE_MODE:     compileFile(compileTarget, outstream, verbose); break;
    case DISASSEMBLE_MODE: disassembleFile(disassembleTarget, outstream, verbose); break;
    case SCAN_MODE:        scanFile(scanTarget, outstream); break;
    case ERROR_MODE: {
      fprintf(stderr, "Usage: %s [-cds] [file...]\n", argv[0]);
      exit(64);
      break;
    }
    default: break; // Unreachable
  }
  /* Close file if used instead of stdout */
  if (outstream != stdout) fclose(outstream);
}
