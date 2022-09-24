#include "stdio.h"
#include "stdlib.h"
#include "assert.h"
#include "string.h"
#include "math.h"

typedef double Elem_t;
const double POISON_VALUE = NAN;
const size_t POISON_SIZES = -7;
const double *const POISON_PTR = &POISON_VALUE;

struct StackInfo
{

    int initLine = 0;
    const char *initFile = "";
    const char *initFunction = "";
};

struct Stack
{
    Elem_t *data = nullptr;
    size_t size = 0;
    size_t capacity = 0;
    StackInfo info = {};
};

enum Errors
{
    NO_ERRORS = 0,
    CANT_ALLOCATE_MEMORY_FOR_STACK = 1,
    CANT_ALLOCATE_MEMORY = 2,
    STACK_IS_EMPTY = 3,
    SIZE_MORE_THAN_CAPACITY = 4,
};

void processError(int error);

void *recalloc(void *memory,
               size_t currentSize,
               size_t newSize,
               Errors *error = nullptr);

Errors stackVerifier(Stack *stack);

Errors __stackCtor(Stack *stack, size_t numOfElements);

#define stackCtor(stack, numOfElements, error)            \
{                                                         \
    (stack)->info.initFile = __FILE__;                    \
    (stack)->info.initFunction = __PRETTY_FUNCTION__;     \
    (stack)->info.initLine = __LINE__;                    \
    *(error) = __stackCtor((stack), (numOfElements));     \
}

#define ASSERT_OK(stack)                                        \
{                                                               \
    StackInfo info = {__LINE__, __FILE__, __PRETTY_FUNCTION__}; \
    error = stackVerifier((stack));                             \
    if (error)                                                  \
        stackDump(stack, &info);                                \
}

Errors stackPush(Stack *stack, Elem_t value);

Errors stackPop(Stack *stack, Elem_t *value);

Errors stackDtor(Stack *stack);

Errors stackDump(Stack *stack, StackInfo *info);

Errors stackResize(Stack *stack);
