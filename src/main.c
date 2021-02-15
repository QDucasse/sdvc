#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <argp.h>

#include "compiler.h"
#include "scanner.h"

/* ==================================
      COMMAND LINE ARGUMENTS
====================================*/

/* Communication between main and parser_opt */
struct arguments {
  int verbose;              /* Verbose mode */
  char *outfile;            /* Argument for -o */
  char *stringCompile;      /* Argument for -c */
  char *stringScan;         /* Argument for -s */
  char *stringDisassemble;  /* Argument for -d */
};

/* CLI options (name, key, arg, flags, doc) */
static struct argp_option options[] = {
  {"verbose", 'v', 0, 0, "Produce verbose output (register states after each compile step)."},
  {"compile", 'c', "INFILE", 0, "Compile INFILE into a succession of instructions."},
  {"scan", 's', "INFILE", 0, "Scan INFILE into a succession of tokens."},
  {"disassemble", 'd', "INFILE", 0, "Disassemble INFILE into a succession of instructions."},
  {"output", 'o', "OUTFILE", 0, "Output to OUTFILE (instead of standard output)."}
};

/* Email reporting */
const char *argp_program_bug_address = "quentin.ducasse@ensta-bretagne.org";

/* Mandatory arguments */
static char args_doc[] = "All arguments need a flag.";

/* Order of parameters: KEY, ARG, STATE. */
static error_t parse_opt (int key, char *arg, struct argp_state *state) {
  struct arguments *arguments = state->input;
  switch (key) {
    case 'v': arguments->verbose = 1;             break;
    case 'c': arguments->stringCompile = arg;     break;
    case 's': arguments->stringScan = arg;        break;
    case 'd': arguments->stringDisassemble = arg; break;
    case 'o': arguments->outfile = arg;           break;
    case ARGP_KEY_ARG:
      if (state->arg_num >= 2) argp_usage(state);
      break;
    case ARGP_KEY_END:
      if (state->arg_num < 2) argp_usage (state);
      break;
    default: return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

/* Documentation of the command */
static char doc[] = "sdvc -- SDVE compiler and disassembler.";

/* The ARGP structure itself */
static struct argp argp = {options, parse_opt, args_doc, doc};


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

/* Write a file */
static void writeFile(const char* source, FILE* dest) {

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
static void compileFile(const char* path, FILE* outstream) {
  initCompiler();
  char* source = readFile(path);
  compile(source);
  writeFile(path, outstream);
  freeCompiler();
  free(source);
}

/* Disassemble a given file */
static void disassembleFile(const char* path, FILE* outstream) {
  char* source = readFile(path);
  writeFile(source, outstream);
  free(source);
}

/* ==================================
               MAIN
====================================*/

// int main(int argc, char* argv[]) {
//   if (argc == 2) {
//     initCompiler();
//     char* source = readFile(argv[1]);
//     compile(source);
//     freeCompiler();
//   } else if (argc == 3) {
//     scanFile(argv[2]);
//   } else {
//     fprintf(stderr, "Usage: sdvu path\n");
//     exit(64);
//   }
//   return 0;
// }


/* Main function using the CLI */
int main (int argc, char *argv[]) {
  struct arguments arguments;
  FILE *outstream;
  /* Set argument defaults */
  arguments.outfile = NULL;
  arguments.stringCompile = NULL;
  arguments.stringScan = NULL;
  arguments.stringDisassemble = NULL;
  arguments.verbose = 0;
  /* Argument parsing */
  argp_parse (&argp, argc, argv, 0, 0, &arguments);
  /* Set the output */
  if (arguments.outfile) {
    outstream = fopen (arguments.outfile, "w");
  } else {
    outstream = stdout;
  }
  /* Run the correct option */
  if (arguments.stringCompile) {
    compileFile(arguments.stringCompile, outstream);
  } else if (arguments.stringScan) {
    scanFile(arguments.stringScan, outstream);
  } else if (arguments.stringDisassemble) {
    disassembleFile(arguments.stringDisassemble, outstream);
  } else {
    exit(64);
  }
  return 0;
}
