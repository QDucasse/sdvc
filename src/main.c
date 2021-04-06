#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compiler.h"
#include "scanner.h"

FILE* logOutstream;
char* binName;

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

int countOccurencesInString(char* source, char* word) {
  int stringLen = strlen(source); // length of string
  int searchLen = strlen(word);   // length of word to be searched
  int count = 0;
  int found = 1;
  for(int i=0 ; i <= stringLen-searchLen ; i++) {
    /* Match word with string */
    found = 1;
    for(int j=0 ; j < searchLen ; j++) {
      if(source[i + j] != word[j]) {
        found = 0;
        break;
      }
    }
    if(found == 1) {
      count++;
    }
  }
  return count;
}

/* Scan a given file */
static void scanFile(const char* path, FILE* outstream) {
  char* source = readFile(path);
  initScanner(source);
  Token token;
  for (;;) {
    token = scanToken();
    fprintToken(outstream, token);
    if (token.type == TOKEN_EOF) {
      fprintf(outstream, "File scanned\n");
      break;
    }
  }
  free(source);
}

/* Compiler a given file */
static void compileFile(const char* path, int nbTargets, bool verbose) {
  /* Read file */
  char* source = readFile(path);
  /* Count number of guard/actions if needed */
  int nbGA = 0;
  if (nbTargets != 1) {
    nbGA = countOccurencesInString(source, "process");
  }
  /* Setup disassembler */
  initDisassembler(verbose, logOutstream);
  /* Setup compiler */
  initCompiler();
  compile(source, nbTargets, nbGA, binName);
  /* Free resources */
  freeCompiler();
  freeDisassembler();
  free(source);
}

/* Disassemble a given file */
static void disassembleFile(const char* path, bool verbose) {
  /* Setup disassembler */
  initDisassembler(verbose, logOutstream);
  /* Disassemble binary file */
  disassembleBinary(path);
  /* Free resources */
  freeDisassembler();
}

/* ==================================
               MAIN
====================================*/

int main(int argc, char *argv[]) {
  /* Default values */
  bool verbose = false;
  /* Default string values */
  char* compileTarget = NULL;
  char* disassembleTarget = NULL;
  char* scanTarget = NULL;
  /* Number of CPUs */
  int nbTargets = 1;
  /* Name of the resulting binary */
  binName = "a.out";
  /* Default output stream */
  logOutstream = stdout;

  /* Modes */
  enum {
    ERROR_MODE,
    COMPILE_MODE,
    DISASSEMBLE_MODE,
    SCAN_MODE,
    COUNT_MODE
  } mode = ERROR_MODE;

  /* Parsing arguments */
  size_t optind;
  for (optind = 1; optind < argc && argv[optind][0] == '-'; optind++) {
    switch (argv[optind][1]) {
    case 'c': {
      mode = COMPILE_MODE;
      compileTarget = argv[optind + 1];
      optind++;
      break;
    }
    case 'd': {
      mode = DISASSEMBLE_MODE;
      disassembleTarget = argv[optind + 1];
      optind++;
      /* Force verbose to true */
      verbose = true;
      break;
    }
    case 's': {
      mode = SCAN_MODE;
      scanTarget = argv[optind + 1];
      optind++;
      break;
    }
    case 'n': {
      nbTargets = atoi(argv[optind + 1]);
      optind ++;
      break;
    }
    case 'l': {
      logOutstream = fopen(argv[optind + 1], "a");
      optind++;
      break;
    }
    case 'o': {
      binName = argv[optind + 1];
      optind++;
      break;
    }
    case 'v': verbose = true; break;
    default:
      fprintf(stderr, "Usage: %s [-cdlosv] [file...]\n", argv[0]);
      exit(64);
    }
  }

  /* Using arguments */
  switch (mode) {
    case COMPILE_MODE:     compileFile(compileTarget, nbTargets, verbose); break;
    case DISASSEMBLE_MODE: disassembleFile(disassembleTarget, verbose); break;
    case SCAN_MODE:        scanFile(scanTarget, logOutstream); break;
    case ERROR_MODE: {
      fprintf(stderr, "Usage: %s [-cds] [file...]\n", argv[0]);
      exit(64);
      break;
    }
    default: break; // Unreachable
  }
  /* Close file if used instead of stdout */
  if (logOutstream != stdout) fclose(logOutstream);
}
