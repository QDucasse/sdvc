#include <stdlib.h>

#include "memory.h"

#define GC_HEAP_GROW_FACTOR 2

/* ======================
      REALLOCATION
====================== */

/* Memory reallocation */
void* reallocate(void* pointer, size_t oldSize, size_t newSize) {
  /* Free option */
  if (newSize == 0) {
    free(pointer);
    return NULL;
  }
  /* Allocation options */
  void* result = realloc(pointer, newSize);
  /* In case there isnt enough memory */
  if (result == NULL) exit(1);
  return result;
}

/* ======================
      FREE OBJECTS
====================== */

// /* Free an object in the linked list */
// static void freeObject(Obj* object) {
// #ifdef DEBUG_LOG_
//   printf("%p free type %d\n", (void*)object, object->type);
// #endif
//   switch (object->type) {
//     case OBJ_BOUND_METHOD: {
//       FREE(ObjBoundMethod, object);
//       break;
//     }
//     case OBJ_INSTANCE: {
//       ObjInstance* instance = (ObjInstance*)instance;
//       freeTable(&instance->fields);
//       FREE(ObjInstance, object);
//       break;
//     }
//     case OBJ_CLASS: {
//       ObjClass* klass = (ObjClass*)object;
//       freeTable(&klass->methods);
//       FREE(ObjClass, object);
//       break;
//     }
//     case OBJ_CLOSURE: {
//       ObjClosure* closure = (ObjClosure*)object;
//       /* Free upvalues array */
//       FREE_ARRAY(ObjUpvalue*, closure->upvalues, closure->upvalueCount);
//       FREE(ObjClosure, object);
//       break;
//     }
//     case OBJ_UPVALUE: {
//       FREE(ObjUpvalue, object);
//       break;
//     }
//     case OBJ_FUNCTION: {
//       ObjFunction* function = (ObjFunction*)object;
//       freeChunk(&function->chunk);
//       FREE(ObjFunction, object);
//       break;
//     }
//     case OBJ_NATIVE: {
//       FREE(ObjNative, object);
//       break;
//     }
//     case OBJ_STRING: {
//       ObjString* string = (ObjString*)object;
//       FREE_ARRAY(char, string->chars, string->length + 1);
//       FREE(ObjString, object);
//       break;
//     }
//   }
// }
//
// /* Free the objects from the vm objects linked list */
// void freeObjects() {
//   /* Free VM Objects */
//   Obj* object = vm.objects;
//   while (object != NULL) {
//     Obj* next = object->next;
//     freeObject(object);
//     object = next;
//   }
// }
