#include <string.h>

#include "unity.h"
#include "scanner.h"
#include "scanner.c"

/* Setup and teardown routine */
void setUp() {}
void tearDown() {}

/* Confirm scanner initialization */
void testScannerInitialization() {
  char* source = "source string test";
  initScanner(source);
  TEST_ASSERT_EQUAL_STRING(source, scanner.start);
  TEST_ASSERT_EQUAL_STRING(source, scanner.current);
  TEST_ASSERT_EQUAL_INT(1, scanner.line);
}

/* Scanning Routine
================ */

void testScannerAdvance() {
  char* source = "plip ploup\0";
  initScanner(source);
  char outChar = advance();
  TEST_ASSERT_EQUAL_CHAR('p', outChar);
  TEST_ASSERT_EQUAL_STRING("plip ploup\0", scanner.start);
  TEST_ASSERT_EQUAL_STRING("lip ploup\0", scanner.current);
}

void testScannerPeek() {

}

void testScannerPeekNext() {

}

/* Character checks
================ */
void testScannerIsEOFTrue() {
  char* source = "\0";
  initScanner(source);
  TEST_ASSERT_TRUE(isAtEnd());
}

void testScannerIsEOFFalse() {
  char* source = "plip ploup";
  initScanner(source);
  TEST_ASSERT_FALSE(isAtEnd());
}

void testScannerIsAlpha() {
  char* source = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ\0";
  initScanner(source);
  while (*scanner.current != '\0') {
    TEST_ASSERT_TRUE(isAlpha(advance()));
  }
}

void testScannerIsDigit() {
  char* source = "0123456789\0";
  initScanner(source);
  while (*scanner.current != '\0') {
    TEST_ASSERT_TRUE(isDigit(advance()));
  }
}

void testScannerIsIDPunctuation() {
  char* source = "_.\0";
  initScanner(source);
  while (*scanner.current != '\0') {
    TEST_ASSERT_TRUE(isIDPunctuation(advance()));
  }
}
