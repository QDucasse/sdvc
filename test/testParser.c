#include "unity.h"

#include "compiler.h"
#include "compiler.c"

#include "common.h"
#include "chunk.h"
#include "disassembler.h"
#include "mmemory.h"
#include "scanner.h"
#include "sstring.h"
#include "register.h"
#include "table.h"
#include "value.h"

typedef struct {
  TokenType type;
  char* source;
  int size;
} TestStub;

static TestStub stubs[38] = {
  {TOKEN_EOF,           "\0",              1},
  {TOKEN_LEFT_PAREN,    "(",               1},
  {TOKEN_RIGHT_PAREN,   ")",               1},
  {TOKEN_LEFT_BRACE,    "{",               1},
  {TOKEN_RIGHT_BRACE,   "}",               1},
  {TOKEN_SEMICOLON,     ";",               1},
  {TOKEN_COMMA,         ",",               1},
  {TOKEN_DOT,           ".",               1},
  {TOKEN_MINUS,         "-",               1},
  {TOKEN_PLUS,          "+",               1},
  {TOKEN_SLASH,         "/",               1},
  {TOKEN_STAR,          "*",               1},
  {TOKEN_MODULO,        "%",               1},
  {TOKEN_BANG_EQUAL,    "!=",              2},
  {TOKEN_EQUAL_EQUAL,   "==",              2},
  {TOKEN_EQUAL,         "=",               1},
  {TOKEN_LESS,          "<",               1},
  {TOKEN_LESS_EQUAL,    "<=",              2},
  {TOKEN_GREATER,       ">",               1},
  {TOKEN_GREATER_EQUAL, ">=",              2},
  {TOKEN_AND,           "and",             3},
  {TOKEN_BOOL,          "bool",            4},
  {TOKEN_BYTE,          "byte",            4},
  {TOKEN_EFFECT,        "effect",          6},
  {TOKEN_FALSE,         "false",           5},
  {TOKEN_GUARD_BLOCK,   "guardblock",     10},
  {TOKEN_GUARD_BLOCK,   "guardBlock",     10},
  {TOKEN_GUARD_COND,    "guardcondition", 14},
  {TOKEN_GUARD_COND,    "guardCondition", 14},
  {TOKEN_INT,           "int",             3},
  {TOKEN_OR,            "or",              2},
  {TOKEN_PROCESS,       "process",         7},
  {TOKEN_STATE,         "state",           5},
  {TOKEN_TEMP,          "temp",            4},
  {TOKEN_TRUE,          "true",            4},
  {TOKEN_TUPLE,         "tuple",           5},
  {TOKEN_IDENTIFIER,    "t_250",           5},
  {TOKEN_NUMBER,        "250.45",          6}
};

/* Setup/Teardown routine */
void setUp() {}
void tearDown() {}

/* Parsing utilities
================= */

void testAdvance() {
  for (int i = 0 ; i < 38 ; i++) {
    TestStub currentStub = stubs[i];
    initScanner(currentStub.source);
    advance();
    TEST_ASSERT_EQUAL(currentStub.type, parser.current.type);
  }
}

void testConsume() {
  for (int i = 0 ; i < 38 ; i++) {
    TestStub currentStub = stubs[i];
    initScanner(currentStub.source);
    parser.current = scanToken(); // Process a given token
    consume(currentStub.type, "Type not verified.");
    TEST_ASSERT_EQUAL(currentStub.type, parser.previous.type);
  }
}

// void testConsumeFailing() {
//   for (int j = 0 ; j < 38 ; j++){
//     for (int i = 0 ; i < 38 ; i++) {
//       if (j == i) continue;
//       TestStub currentStub = stubs[i];
//       TestStub otherStub   = stubs[j];
//       printf("%u\n", currentStub.type);
//       printf("%u\n", otherStub.type);
//       initScanner(currentStub.source);
//       parser.current = scanToken(); // Process a given token
//
//       /* Redirect sterr to the string buffer */
//       char string[100];
//       freopen("/dev/null", "a", stderr);
//       setbuf(stderr, string);
//       consume(otherStub.type, "Type not verified.");
//
//       /* Crafting the expected string */
//       char expected[100];
//       strcpy(expected, "[line 1] Error at '");
//       strcat(expected, currentStub.source);
//       strcat(expected,"': Type not verified.\n\x10");
//
//       TEST_ASSERT_EQUAL_STRING(expected, string);
//
//     }
//   }
// }

// void testCheck() {
//
// }
//
// void testCheckFailing() {
//
// }
//
// void testMatch() {
//
// }
//
// void testMatchFailing() {
//
// }
