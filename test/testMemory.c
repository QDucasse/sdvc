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

/* Reallocate tests
================ */

/* Allocate a given object */
void testAllocateObject(){
  TestObject* testObj = ALLOCATE_OBJ(TestObject);
  TEST_ASSERT_EQUAL(NULL, FREE(testObj));
}

void testAllocateArray(){
  TestObject* testObjs = ALLOCATE_ARRAY(TestObject, 32);
  TEST_ASSERT_EQUAL(NULL, FREE(testObjs));
}

void testGrowArray() {
  TestObject* testObjs = ALLOCATE_ARRAY(TestObject, 2);
  TestObject* newTestObjs = GROW_ARRAY(TestObject, testObjs, 12);
  TEST_ASSERT_EQUAL(testObjs, newTestObjs);
  TEST_ASSERT_EQUAL(NULL, FREE(newTestObjs));
}

void testGrowCapacityFromZero() {
  int oldCapacity = 0;
  TEST_ASSERT_EQUAL_INT(8, GROW_CAPACITY(oldCapacity));
}

void testGrowCapacityFromEight() {
  int oldCapacity = 8;
  TEST_ASSERT_EQUAL_INT(16, GROW_CAPACITY(oldCapacity));
}
