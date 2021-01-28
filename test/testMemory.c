#include <stdlib.h>

#include "unity.h"
#include "mmemory.h"

/* Setup and teardown routine */
void setUp() {}
void tearDown() {}


typedef struct {
  int index;
  char* name;
} TestObject;

TestObject* initTestObj() {
  TestObject* testObj = ALLOCATE_OBJ(TestObject);
  testObj->index = 0;
  testObj->name = NULL;
}

void freeTestObj(TestObject* testObj) {
  FREE(testObj->name);
  FREE(testObj);
}

/* Reallocate tests
================ */

/* Allocate a given object */
TestObject* testAllocateObject(){
  TestObject* testObj = ALLOCATE_OBJ(TestObject);
  TEST_ASSERT_EQUAL(NULL, FREE(testObj));
}

void testAllocateArray(){
  TestObject* testObjs = ALLOCATE_ARRAY(TestObject, 32);
  TEST_ASSERT_EQUAL(NULL, FREE(testObjs));
}

void testGrowArrayInStruct() {
  TestObject* testObj = initTestObj();
  testObj->name = GROW_ARRAY(char, testObj->name, 8);
  freeTestObj(testObj);
}


void testGrowArray() {
  TestObject* testObjs = ALLOCATE_ARRAY(TestObject, 12);
  printf("testObjs old address: %p\n", (void *) testObjs);
  testObjs = GROW_ARRAY(TestObject, testObjs, 24);
  printf("testObjs new address: %p\n", (void *) testObjs);
  TEST_ASSERT_EQUAL(NULL, FREE(testObjs));
}

void testGrowArrayFromNULL() {
  TestObject* testObjs = NULL;
  printf("testObjs old address: %p\n", (void *) testObjs);
  testObjs = GROW_ARRAY(TestObject, testObjs, 24);
  printf("testObjs new address: %p\n", (void *) testObjs);
  TEST_ASSERT_EQUAL(NULL, FREE(testObjs));
}

void testGrowCapacityFromZero() {
  int oldCapacity = 0;
  TEST_ASSERT_EQUAL_INT(8, GROW_CAPACITY(oldCapacity));
}

void testGrowCapacityFromEight() {
  int oldCapacity = 8;
  TEST_ASSERT_EQUAL_INT(16, GROW_CAPACITY(oldCapacity));
}

void testMacros() {
  int* blip = ALLOCATE_ARRAY(int, 8);
  int* bloup = GROW_ARRAY(int, blip, GROW_CAPACITY(8));
  printf("Blip address:  %p\n", (void *) blip);
  printf("Bloup address: %p\n", (void *) bloup);
  FREE(bloup);
}
