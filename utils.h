#include "stdio.h"
#include "stdlib.h"
#include "assert.h"
#include "string.h"
#include "math.h"

typedef double Elem_t;
const Elem_t POISON_VALUE = NAN;
const int POISON_INT_VALUE = -7;
const Elem_t *const POISON_PTR = &POISON_VALUE;
const char *const POISON_STRING = "1000-7";

struct StackInfo
{
    int initLine = POISON_INT_VALUE;
    const char *initFile = POISON_STRING;
    const char *initFunction = POISON_STRING;
};

struct Stack
{
    Elem_t *data = (Elem_t *) POISON_PTR;
    size_t size = (size_t) POISON_INT_VALUE;
    size_t capacity = (size_t) POISON_INT_VALUE;
    StackInfo info = {};
    size_t hash = 0;
};

enum Errors
{
    NO_ERRORS = 0,
    CANT_ALLOCATE_MEMORY_FOR_STACK = 1 << 0,
    CANT_ALLOCATE_MEMORY = 1 << 1,
    STACK_IS_EMPTY = 1 << 2,
    SIZE_MORE_THAN_CAPACITY = 1 << 3,
    POISON_PTR_ERR = 1 << 4,
    POISONED_SIZE_ERR = 1 << 5,
    POISONED_CAPACITY_ERR = 1 << 6,
    INCORRECT_HASH = 1 << 7,
};

void processError(size_t error);

/**
 * @brief Recalloc implementation
 *
 * @param memory memory to resize
 * @param currentSize current size of memory
 * @param newSize required size of memory
 * @param error error code
 * @return pointer to new memory if success, nullptr if can't allocate memory
 */
void *recalloc(void *memory,
               size_t currentSize,
               size_t newSize,
               size_t *error = nullptr);

/**
 * @brief Checks if stack is correct
 *
 * @param stack stack for checking
 * @return error code
 */
size_t stackVerifier(Stack *stack);

/**
 * @brief constructor for stack
 *
 * @param stack stack for constructing
 * @param numOfElements number of elements in stack
 * @return error code
 */
size_t __stackCtor(Stack *stack, size_t numOfElements);

/**
 * @brief macro constructor for stack
 *
 * @param stack stack for constructing
 * @param numOfElements number of elements in stack
 * @param error error code
 * @return void
 */
#define stackCtor(stack, numOfElements, error)            \
{                                                         \
    (stack)->info.initFile = __FILE__;                    \
    (stack)->info.initFunction = __PRETTY_FUNCTION__;     \
    (stack)->info.initLine = __LINE__;                    \
    *(error) = __stackCtor((stack), (numOfElements));     \
}

/**
 * @brief macro for checking if stack is correct
 *
 * @param stack stack for checking
 * @return void
 */
#define ASSERT_OK(stack, error)                                 \
{                                                               \
    StackInfo info = {__LINE__, __FILE__, __PRETTY_FUNCTION__}; \
    *(error) = stackVerifier((stack));                          \
    if (*(error))                                                 \
    {                                                           \
        stackDump((stack), &(info));                            \
    }                                                           \
}

/**
 * @brief pushes element to stack
 *
 * @param stack stack for pushing
 * @param value pushing value
 * @return error code
 */
size_t stackPush(Stack *stack, Elem_t value);

/**
 * @brief extract last element from stack
 *
 * @param stack stack for extracting
 * @param value variable for storing extracted element
 * @return error code
 */
size_t stackPop(Stack *stack, Elem_t *value);

/**
 * @brief destructor for stack
 *
 * @param stack stack for destructing
 * @return error code
 */
size_t stackDtor(Stack *stack);

/**
 * @brief generates dump of stack
 *
 * @param stack stack for dumping
 * @return void
 */
void stackDump(Stack *stack, StackInfo *info);

/**
 * @brief resizes stack to certain len
 *
 * @param stack stack for resizing
 * @param newStackCapacity new size
 * @return error code
 */
size_t stackResizeMemory(Stack *stack, size_t newStackCapacity);

/**
 * @brief resizes stack
 *
 * @param stack stack for resizing
 * @return error code
 */
size_t stackResize(Stack *stack);

size_t hashData(void *data, size_t size);

size_t stackHash(Stack *stack);
