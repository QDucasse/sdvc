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

void setUp() {}
void tearDown() {}


/* ==================================
          NAME PROCESSING
=================================== */

/* Test string prefix */
void testStrCmpPrefix() {
  TEST_ASSERT_TRUE(prefix("blip","blip bloup"));
  TEST_ASSERT_FALSE(prefix("bipboup","blip bloup"));
}

/* Test if a string is a temporary variable identifier */
void testStrIsTemp() {
  char* tokenIdentifierName1 = "t_335";
  char* tokenIdentifierName2 = "blip";
  Token token1 = {.start = tokenIdentifierName1};
  Token token2 = {.start = tokenIdentifierName2};
  TEST_ASSERT_TRUE(isTemp(&token1));
  TEST_ASSERT_FALSE(isTemp(&token2));
}

/* ==================================
        COMPILER OPERATIONS
=================================== */

/* Error handling
============== */

void testErrorAt() {

}

void testError() {

}

void testErrorAtCurrent() {

}


/* Global declarations
=================== */

void testGlobalDeclaration() {

}

void testGlobalName() {

}

void testGlobalBoolDeclaration() {

}

void testGlobalByteDeclaration() {

}

void testGlobalByteArrayDeclaration() {

}

void testGlobalIntDeclaration() {

}

void testGlobalIntArrayDeclaration() {

}

void testGlobalStateDeclaration() {

}

/* Process processing
================== */

// GUARDBLOCK
void testGuardblock() {

}

void testGuardblockFailingKeyword() {

}

void testGuardblockFailingComma() {

}

void testGuardblockFailingSemicolon() {

}

// EFFECT
void testEffect() {

}

void testEffectFailingKeyword() {

}

void testEffectFailingComma() {

}

void testEffectFailingSemicolon() {

}

// PROCESS
void testProcess() {

}

void testProcessFailingKeyword() {

}

void testProcessFailingIdentifier() {

}

void testProcessFailingGuardconditionIdentifier() {

}

void testProcessFailingGuardconditionVariable() {

}

void testProcessFailingGuardconditionSemicolon() {

}

/* Assignments and Expressions
=========================== */



/* Compile routine
=============== */
