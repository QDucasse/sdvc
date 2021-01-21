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

/* Scan Helpers
============ */

void testScannerAdvance() {
  char* source = "plip ploup\0";
  initScanner(source);
  char outChar = advance();
  TEST_ASSERT_EQUAL_CHAR('p', outChar);
  TEST_ASSERT_EQUAL_STRING("plip ploup\0", scanner.start);
  TEST_ASSERT_EQUAL_STRING("lip ploup\0", scanner.current);
}

void testScannerPeek() {
  char* source = "plip ploup\0";
  initScanner(source);
  char outChar = peek();
  TEST_ASSERT_EQUAL_CHAR('p', outChar);
  TEST_ASSERT_EQUAL_STRING("plip ploup\0", scanner.start);
  TEST_ASSERT_EQUAL_STRING("plip ploup\0", scanner.current);
}

void testScannerPeekNext() {
  char* source = "plip ploup\0";
  initScanner(source);
  char outChar = peekNext();
  TEST_ASSERT_EQUAL_CHAR('l', outChar);
  TEST_ASSERT_EQUAL_STRING("plip ploup\0", scanner.start);
  TEST_ASSERT_EQUAL_STRING("plip ploup\0", scanner.current);
}

void testScannerPeekNextEOF() {
  char* source = "\0";
  initScanner(source);
  char outChar = peekNext();
  TEST_ASSERT_EQUAL_CHAR('\0', outChar);
  TEST_ASSERT_EQUAL_STRING("\0", scanner.start);
  TEST_ASSERT_EQUAL_STRING("\0", scanner.current);
}

void testMatch() {
  char* source = "p\0";
  initScanner(source);
  bool matchFirst = match('p');
  TEST_ASSERT_TRUE(matchFirst);
  TEST_ASSERT_EQUAL_STRING("p\0", scanner.start);
  TEST_ASSERT_EQUAL_STRING("\0", scanner.current);
  bool matchSecond = match('\0');
  TEST_ASSERT_FALSE(matchSecond);
  TEST_ASSERT_EQUAL_STRING("p\0", scanner.start);
  TEST_ASSERT_EQUAL_STRING("\0", scanner.current);
}


void testSkipWhiteSpace() {
  char* source = "    \t\n\t\tp // a comment blib bloub\n\0";
  initScanner(source);
  skipWhitespace();
  char outChar = advance();
  TEST_ASSERT_EQUAL_CHAR('p', outChar);
  TEST_ASSERT_EQUAL_INT(2, scanner.line);
  skipWhitespace();
  TEST_ASSERT_EQUAL_INT(3, scanner.line);
  TEST_ASSERT_TRUE(isAtEnd());
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

/* Token Creation
=============== */

void testMakeToken() {
  char* source = "plip\0";
  initScanner(source);
  scanner.current = scanner.current + 4;
  Token token = makeToken(TOKEN_IDENTIFIER);
  TEST_ASSERT_EQUAL(TOKEN_IDENTIFIER, token.type);
  TEST_ASSERT_EQUAL(scanner.start, token.start);
  TEST_ASSERT_EQUAL_INT(4, token.length);
  TEST_ASSERT_EQUAL_INT(1, token.line);
}

void testErrorToken() {
  scanner.line = 10;
  Token error = errorToken("blip bloup");
  TEST_ASSERT_EQUAL(TOKEN_ERROR, error.type);
  TEST_ASSERT_EQUAL_STRING("blip bloup", error.start);
  TEST_ASSERT_EQUAL_INT(10, error.length);
  TEST_ASSERT_EQUAL_INT(10, error.line);
}

/* Token type identification
========================= */

void testCheckKeyword() {
  char* source = "effect";
  initScanner(source);
  scanner.current = scanner.current + 6;
  TEST_ASSERT_EQUAL(TOKEN_EFFECT, checkKeyword(1, 5, "ffect", TOKEN_EFFECT));
  TEST_ASSERT_EQUAL(TOKEN_EFFECT, checkKeyword(2, 4, "fect", TOKEN_EFFECT));
  TEST_ASSERT_EQUAL(TOKEN_EFFECT, checkKeyword(3, 3, "ect", TOKEN_EFFECT));
  TEST_ASSERT_EQUAL(TOKEN_EFFECT, checkKeyword(4, 2, "ct", TOKEN_EFFECT));
  TEST_ASSERT_EQUAL(TOKEN_EFFECT, checkKeyword(5, 1, "t", TOKEN_EFFECT));
}

void testIdentifierType() {

}

void testIdentifier() {
  char* source = "t_blipbloup240.45wdfs";
  initScanner(source);
  Token idToken = identifier();
  TEST_ASSERT_EQUAL(TOKEN_IDENTIFIER, idToken.type);
  TEST_ASSERT_EQUAL_STRING("t_blipbloup240.45wdfs", idToken.start);
  TEST_ASSERT_EQUAL_INT(21, idToken.length);
}

void testNumber() {
  char* source = "356.3742";
  initScanner(source);
  Token numToken = number();
  TEST_ASSERT_EQUAL(TOKEN_NUMBER, numToken.type);
  TEST_ASSERT_EQUAL_STRING("356.3742", numToken.start);
  TEST_ASSERT_EQUAL_INT(8, numToken.length);
}


/* Scanning routine
================ */


void testScanToken() {
  
}
