#include <cstdint>
#include "stdio.h"
#include "stdlib.h"
#include "assert.h"
#include "string.h"
#include "math.h"
#include <cstdarg>
#include "config.h"

typedef int Elem_t;
typedef uint64_t Canary;
#if (PoisonProtection)
const Elem_t POISON_VALUE = INT_MAX;
const Elem_t *const POISON_PTR = &POISON_VALUE;
#endif
const int POISON_INT_VALUE = -7;
const char *const POISON_STRING = "1000-7";

#if (CanaryProtection)
const uint64_t CANARY_START = 0x8BADF00D;
const uint64_t CANARY_END = 0xBAADF00D;
const uint64_t CANARY_POISONED = 0xDEADBEEF;
#endif

struct StackInfo
{
    int initLine = POISON_INT_VALUE;
    const char *initFile = POISON_STRING;
    const char *initFunction = POISON_STRING;
    const char *name = POISON_STRING;
};

struct Stack
{
#if (CanaryProtection)
    Canary canary_start = CANARY_START;
#endif
#if (PoisonProtection)
    Elem_t *data = (Elem_t *) POISON_PTR;
    FILE *logFile = nullptr;
#else
    Elem_t *data = nullptr;
    FILE *logFile = nullptr;
#endif
    size_t capacity = (size_t) 0;
    size_t size = (size_t) 0;

    StackInfo info = {};
    bool alive = false;
#if (HashProtection)
    size_t dataHash = 0;
    size_t hash = 0;
#endif
#if (CanaryProtection)
    Canary canary_end = CANARY_END;
#endif
};

enum Errors
{
    STACK_NO_ERRORS                    =       0,
    CANT_ALLOCATE_MEMORY_FOR_STACK     = 1 <<  0,
    CANT_ALLOCATE_MEMORY               = 1 <<  1,
    STACK_IS_EMPTY                     = 1 <<  2,
    STACK_SIZE_MORE_THAN_CAPACITY      = 1 <<  3,
    STACK_POISON_PTR_ERR               = 1 <<  4,
    STACK_POISONED_SIZE_ERR            = 1 <<  5,
    STACK_POISONED_CAPACITY_ERR        = 1 <<  6,
    STACK_INCORRECT_HASH               = 1 <<  7,
    STACK_NOT_ALIVE                    = 1 <<  8,
    STACK_START_STRUCT_CANARY_DEAD     = 1 <<  9,
    STACK_START_STRUCT_CANARY_POISONED = 1 << 10,
    STACK_END_STRUCT_CANARY_DEAD       = 1 << 11,
    STACK_END_STRUCT_CANARY_POISONED   = 1 << 12,
    STACK_START_DATA_CANARY_DEAD       = 1 << 13,
    STACK_START_DATA_CANARY_POISONED   = 1 << 14,
    STACK_END_DATA_CANARY_DEAD         = 1 << 15,
    STACK_END_DATA_CANARY_POISONED     = 1 << 16,
    STACK_DATA_INCORRECT_HASH          = 1 << 17,
    STACK_POISONED_DATA                = 1 << 18,
    STACK_NULLPTR                      = 1 << 19,
};

/**
 * @brief constructor for stack
 *
 * @param stack stack for constructing
 * @param numOfElements number of elements in stack
 * @param logFile - file for stack logging
 * @return error code
 */
size_t stackCtor__(Stack *stack, size_t numOfElements, FILE *logFile);

/**
 * @brief macro constructor for stack
 *
 * @param stack stack for constructing
 * @param numOfElements number of elements in stack
 * @param error error code
 * @param logFile file for logs
 * @return void
 */
#define stackCtor(stack, numOfElements, error, logFile)                \
{                                                                      \
    (stack)->info = {__LINE__, __FILE__, __PRETTY_FUNCTION__, #stack}; \
    *(error) = stackCtor__((stack), (numOfElements), (logFile));       \
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
 * @brief shrink stack to size
 *
 * @param stack stack to shrink
 * @return error code
 */
size_t stackShrinkToFit(Stack *stack);

void stackPoisonData(Stack *stack, size_t *error);

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

#if (HashProtection)
/**
 * @brief hashes data
 *
 * @param data data to hash
 * @param size size of data
 * @return hash of data
 */
size_t hashData(void *data, size_t size);

/**
 * @brief hashes stack data
 *
 * @param stack stack to hash its data
 * @return hash of stack data
 */
size_t stackHashBuffer(Stack *stack);

/**
 * @brief hashes stack
 *
 * @param stack stack to hash
 * @return hash of stack
 */
size_t stackHash(Stack *stack);
#endif

/**
 * @brief destructor for stack
 *
 * @param stack stack for destructing
 * @return error code
 */
size_t stackDtor(Stack *stack);

bool isPoison(Elem_t value);

void stackVerifyPoison(Stack *stack, size_t *error);

/**
 * @brief Checks if stack is correct
 *
 * @param stack stack for checking
 * @return error code
 */
size_t stackVerifier(Stack *stack);

/**
 *
 * @param fp file for logs
 * @param formatString formatting string to log
 * @param ... argument of formattim string
 */
void logStack(FILE *fp, const char *formatString, ...);

/**
 * @brief print Elem_t as double
 *
 * @param value Elem_t to print
 * @param fp file to write
 * @return void
 */
void printElem_t(Elem_t value, FILE *fp);

/**
 * @brief generates dump of stack
 *
 * @param stack stack for dumping
 * @param info struct with info about stack
 * @return void
 */
void stackDump(Stack *stack,
               StackInfo *info,
               size_t error,
               void (*print)(Elem_t, FILE *) = printElem_t);

/**
 * @brief macro for checking if stack is correct
 *
 * @param stack stack for checking
 * @return void
 */
#define ASSERT_OK(stack, error)                                        \
{                                                                      \
    StackInfo info = {__LINE__, __FILE__, __PRETTY_FUNCTION__, #stack};\
    *(error) = stackVerifier((stack));                                 \
    if (*(error))                                                      \
    {                                                                  \
        stackDump((stack), &(info), *(error), printElem_t);            \
    }                                                                  \
}

/**
 * @brief logs error to file
 *
 * @param fp - file to write logs
 * @param error error code to process
 */
void processError(FILE *fp, size_t error);
