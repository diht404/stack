#include "utils.h"

//void *recalloc(void *memory,
//               size_t currentSize,
//               size_t newSize,
//               size_t *error)
//{
//    assert(memory != nullptr);
////    printf("%zu, %zu\n", _msize(memory), currentSize);
//    if (error != nullptr and *error)
//        return error;
//
//    void *newMemory = realloc(memory, newSize);
//    if (newMemory == nullptr)
//    {
//        if (error != nullptr)
//            *error = CANT_ALLOCATE_MEMORY;
//        return memory;
//    }
//
//    if (currentSize < newSize)
//        memset((char *) newMemory + currentSize,
//               0,
//               newSize - currentSize);
//
//    if (error != nullptr)
//        *error = STACK_NO_ERRORS;
//
//    return newMemory;
//}

size_t stackCtor__(Stack *stack, size_t numOfElements, FILE *logFile)
{
    assert(stack != nullptr);
    assert(logFile != nullptr);

    size_t error = STACK_NO_ERRORS;

#if (CanaryProtection)
    size_t dataSize = numOfElements * sizeof(Elem_t);
    char *canary_data_canary = (char *) calloc(
        dataSize + 2 * sizeof(Canary), sizeof(char));
    stack->data = (Elem_t *)(canary_data_canary + sizeof(Canary));

    if (canary_data_canary == nullptr)
        return CANT_ALLOCATE_MEMORY_FOR_STACK;

    Canary *canary_start = (Canary *)canary_data_canary;
    *canary_start = CANARY_START;
    Canary *canary_end = (Canary *)(canary_data_canary + sizeof(Canary) + dataSize);
    *canary_end = CANARY_END;
    stack->canary_start = CANARY_START;
    stack->canary_end = CANARY_END;
//    printf("CTOR START: %p VALUE: %zu\n", canary_start, *canary_start);
//    printf("CTOR END: %p VALUE: %zu\n\n", canary_end, *canary_end);
    stackPoisonData(stack, &error);
#else
    stack->data =
        (Elem_t *) calloc(numOfElements, sizeof(stack->data[0]));
    if (stack->data == nullptr)
        return CANT_ALLOCATE_MEMORY_FOR_STACK;
#endif
    stack->size = 0;
    stack->capacity = numOfElements;
    stack->alive = true;
    stack->logFile = logFile;
# if (HashProtection)
    stack->dataHash = stackHashBuffer(stack);
    stack->hash = stackHash(stack);
#endif

    ASSERT_OK(stack, &error)

    return error;
}

size_t stackPush(Stack *stack, Elem_t value)
{
    assert(stack != nullptr);

    size_t error = STACK_NO_ERRORS;

    ASSERT_OK(stack, &error)
    if (error)
        return error;

    if (stack->size == stack->capacity)
        error = stackResize(stack);

    if (error)
        return error;

    stack->data[stack->size++] = value;
#if (HashProtection)
    stack->dataHash = stackHashBuffer(stack);
    stack->hash = stackHash(stack);
#endif
    ASSERT_OK(stack, &error)

    return error;
}

size_t stackPop(Stack *stack, Elem_t *value)
{
    assert(stack != nullptr);
    assert(value != nullptr);

    size_t error = STACK_NO_ERRORS;
    ASSERT_OK(stack, &error)
    if (error)
        return error;

    if (stack->size == 0)
    {
        *value = POISON_VALUE;
        return STACK_IS_EMPTY;
    }

    *value = stack->data[stack->size--];

#if (HashProtection)
    stack->dataHash = stackHashBuffer(stack);
    stack->hash = stackHash(stack);
#endif
    if (stack->size * 4 <= stack->capacity)
        error = stackResize(stack);

#if (HashProtection)
    stack->dataHash = stackHashBuffer(stack);
    stack->hash = stackHash(stack);
#endif
    ASSERT_OK(stack, &error)

    return error;
}

size_t stackShrinkToFit(Stack *stack)
{
    assert(stack != nullptr);

    size_t error = STACK_NO_ERRORS;
    error = stackResizeMemory(stack, stack->size);
    if (error)
        return error;

    ASSERT_OK(stack, &error)
    return error;
}

void stackPoisonData(Stack *stack, size_t *error)
{
    for (size_t i = stack->size; i < stack->capacity; i++)
    {
        stack->data[i] = POISON_VALUE;
    }
}

size_t stackResizeMemory(Stack *stack, size_t newStackCapacity)
{
    size_t error = 0;
    size_t newCapacity =
        sizeof(Elem_t) * newStackCapacity;

#if (CanaryProtection)
    char *newData =
        (char *) realloc((char *) stack->data - sizeof(Canary),
                         newCapacity + 2 * sizeof(Canary));
#else
    Elem_t *newData =
        (Elem_t *) realloc(stack->data, newCapacity);
#endif

    if (newData == nullptr)
        return CANT_ALLOCATE_MEMORY_FOR_STACK;

#if (CanaryProtection)
    stack->data = (Elem_t *)(newData+sizeof(Canary));

    Canary *canary_start = (Canary *) newData;
    *canary_start = CANARY_START;

    Canary *canary_end = (Canary *)(stack->data + newStackCapacity);
    *canary_end = CANARY_END;

//    printf("RESIZE START: %p VALUE: %zu\n", canary_start, *canary_start);
//    printf("RESIZE END: %p VALUE: %zu\n", canary_end, *canary_end);

#else
    stack->data = (Elem_t *) newData;
#endif

#if (PoisonProtection)
    stackPoisonData(stack, &error);
#endif

    stack->capacity = newStackCapacity;

#if (HashProtection)
    stack->dataHash = stackHashBuffer(stack);
    stack->hash = stackHash(stack);
#endif

    ASSERT_OK(stack, &error)
    return error;
}

size_t stackResize(Stack *stack)
{
    assert(stack != nullptr);

    size_t error = STACK_NO_ERRORS;

    ASSERT_OK(stack, &error)

    if (error)
        return error;

    if (stack->capacity == 0)
    {
        error = stackResizeMemory(stack, 1);
        return error;
    }
    if (stack->size >= stack->capacity)
    {
        size_t newStackCapacity = stack->capacity * 2;
        error = stackResizeMemory(stack, newStackCapacity);
        return error;
    }

    if (stack->size * 4 <= stack->capacity)
    {
        size_t newStackCapacity = stack->capacity / 2;
        error = stackResizeMemory(stack, newStackCapacity);
        return error;
    }

    ASSERT_OK(stack, &error)

    return error;
}

#if (HashProtection)
size_t hashData(void *data, size_t size)
{
    assert(data != nullptr);

    size_t hash = 5381;
    for (size_t i = 0; i < size; i++)
    {
        hash = 33 * hash + ((char *) data)[i];
    }
    return hash;
}

size_t stackHashBuffer(Stack *stack)
{
    return hashData((char *) stack->data,
                    stack->capacity * sizeof(Elem_t));
}

size_t stackHash(Stack *stack)
{
    assert(stack != nullptr);

    size_t old_hash = stack->hash;
    stack->hash = 0;
    size_t hash = hashData(stack, sizeof(*stack));
    stack->hash = old_hash;
    return hash;
}
#endif

size_t stackDtor(Stack *stack)
{
    assert(stack != nullptr);

    size_t error = STACK_NO_ERRORS;

    ASSERT_OK(stack, &error)

    if (error)
        return error;
# if (CanaryProtection)
    free(stack->data - sizeof(Canary));
# else
    free(stack->data);
# endif
    stack->data = (Elem_t *) POISON_PTR;
    stack->size = (size_t) POISON_INT_VALUE;
    stack->capacity = (size_t) POISON_INT_VALUE;

    stack->alive = false;

# if (CanaryProtection)
    stack->canary_start = CANARY_POISONED;
    stack->canary_end = CANARY_POISONED;
#endif

# if (HashProtection)
    stack->hash = (size_t) POISON_INT_VALUE;
    stack->dataHash = (size_t) POISON_INT_VALUE;
# endif
    return error;
}

bool isPoison(Elem_t value)
{
    return value == POISON_VALUE;//TODO: add for other types
}

void stackVerifyPoison(Stack *stack, size_t *error)
{
    assert(stack != nullptr);
    assert(error != nullptr);

    for (size_t i = 0; i < stack->size; i++)
    {
        if (isPoison(stack->data[i]))
        {
            *error |= STACK_POISONED_DATA;
            break;
        }
    }
}

size_t stackVerifier(Stack *stack)
{
    if (stack == nullptr)
        return STACK_NULLPTR;

    size_t error = STACK_NO_ERRORS;
    if (stack->size == (size_t) POISON_INT_VALUE)
    {
        error |= STACK_POISONED_SIZE_ERR;
    }

    if (stack->capacity == (size_t) POISON_INT_VALUE)
    {
        error |= STACK_POISONED_CAPACITY_ERR;
    }

    if (stack->size > stack->capacity)
    {
        error |= STACK_SIZE_MORE_THAN_CAPACITY;
    }

    if (stack->data == POISON_PTR or stack->data == nullptr)
    {
        error |= STACK_POISON_PTR_ERR;
    }

#if (PoisonProtection)
    stackVerifyPoison(stack, &error);
#endif
# if (HashProtection)
    if (stack->dataHash != stackHashBuffer(stack))
    {
        error |= STACK_DATA_INCORRECT_HASH;
    }

    if (stack->hash != stackHash(stack))
    {
        error |= STACK_INCORRECT_HASH;
    }
# endif

    if (!stack->alive)
    {
        error |= STACK_NOT_ALIVE;
    }

# if (CanaryProtection)
    if (stack->canary_start != CANARY_START)
    {
        if (stack->canary_start == CANARY_POISONED)
        {
            error |= STACK_START_STRUCT_CANARY_POISONED;
        }
        else
            error |= STACK_START_STRUCT_CANARY_DEAD;
    }

    if (stack->canary_end != CANARY_END)
    {
        if (stack->canary_end == CANARY_POISONED)
        {
            error |= STACK_END_STRUCT_CANARY_POISONED;
        }
        else
            error |= STACK_END_STRUCT_CANARY_DEAD;
    }

    Canary *canary_start =
        (Canary *) ((char *) stack->data - sizeof(Canary));
    if (*canary_start != CANARY_START)
    {
        if (*canary_start == CANARY_POISONED)
        {
            error |= STACK_START_DATA_CANARY_POISONED;
        }
        else
            error |= STACK_START_DATA_CANARY_DEAD;
    }

    Canary *canary_end = (Canary *) ((char *) stack->data
        + sizeof(Elem_t) * stack->capacity);

    if (*canary_end != CANARY_END)
    {
        if (*canary_end == CANARY_POISONED)
        {
            error |= STACK_END_DATA_CANARY_POISONED;
        }
        else
            error |= STACK_END_DATA_CANARY_DEAD;
    }
//    printf("START CHECK: %p VALUE %zu\n", canary_start, *canary_start);
//    printf("END CHECK: %p VALUE %zu\n", canary_end, *canary_end);
# endif

    return error;
}

void logStack(FILE *fp, const char *formatString, ...)
{
    assert(fp != nullptr);
    assert(formatString != nullptr);

    va_list args;
    va_start(args, formatString);
    vfprintf(fp, formatString, args);
    va_end(args);
}

void printElem_t(Elem_t value, FILE *fp)
{
    assert(fp != nullptr);

    logStack(fp, "%d", value);
}

void stackDump(Stack *stack,
               StackInfo *info,
               size_t error,
               void (*print)(Elem_t, FILE *))
{
    assert(stack != nullptr);
    assert(info != nullptr);

    FILE *fp = stack->logFile;
//    if (fp == nullptr)
        fp = stderr;

    logStack(fp, "-----START LOGGING STACK-----\n");
    logStack(fp, "Error code %zu.\n", error);
    logStack(fp, "Error in stack '%s' in function '%s' at %s (%d)\n",
             info->name,
             info->initFunction,
             info->initFile,
             info->initLine);

    if (!stack->alive)
    {
        processError(fp, error);
        logStack(fp, "-----END LOGGING STACK-----\n");
        return;
    }

    logStack(fp, "Stack [%p] '%s' was initialized at %s at %s (%d)\n",
             stack,
             stack->info.name,
             stack->info.initFunction,
             stack->info.initFile,
             stack->info.initLine);
# if (HashProtection)
    logStack(fp, "{\n"
                 "    Size = %zu \n"
                 "    Capacity = %zu \n"
                 "    Stack hash = %zu \n"
                 "    Correct stack hash = %zu \n"
                 "    Stack data hash = %zu \n"
                 "    Correct stack data hash = %zu \n"
                 "    Data [%p] \n",
             stack->size,
             stack->capacity,
             stackHash(stack),
             stack->hash,
             stackHashBuffer(stack),
             stack->dataHash,
             stack->data);
#else
    logStack(fp, "{\n"
                 "    Size = %zu \n"
                 "    Capacity = %zu \n"
                 "    Data [%p] \n",
             stack->size,
             stack->capacity,
             stack->data);
#endif
    if (stack->data == POISON_PTR or stack->data == nullptr)
    {
        logStack(fp, "    Pointer poisoned.\n}\n");
        return;
    }
    for (size_t i = 0; i < stack->size; i++)
    {

        logStack(fp, "    * [%zu] = ", i);
        print(stack->data[i], fp);
        logStack(fp,
                 " %s\n",
                 isPoison(stack->data[i]) ? "(Poisoned)" : "");
    }
    for (size_t i = stack->size; i < stack->capacity; i++)
    {
        logStack(fp, "      [%zu] = ", i);
        print(stack->data[i], fp);
        logStack(fp,
                 " %s\n",
                 isPoison(stack->data[i]) ? "(Poisoned)" : "");
    }
    logStack(fp, "}\n");
#if (CanaryProtection)
    logStack(fp,
             "Data Canary end %zu\n",
             (Canary) (stack->data + stack->capacity));
    logStack(fp, "Correct data Canary end %zu\n", CANARY_END);
#endif
    processError(fp, error);
    logStack(fp, "-----END LOGGING STACK-----\n");
}

void processError(FILE *fp, size_t error)
{
    assert(fp != nullptr);

    if (!error)
        logStack(fp,
                 "No errors.\n");
    if (error & CANT_ALLOCATE_MEMORY_FOR_STACK)
        logStack(fp,
                 "Can't allocate memory for stack.\n");
    if (error & CANT_ALLOCATE_MEMORY)
        logStack(fp,
                 "Can't allocate memory.\n");
    if (error & STACK_IS_EMPTY)
        logStack(fp,
                 "Can't pop element from stack. Stack is empty.\n");
    if (error & STACK_SIZE_MORE_THAN_CAPACITY)
        logStack(fp,
                 "Size more than capacity.\n");
    if (error & STACK_POISON_PTR_ERR)
        logStack(fp,
                 "Trying to write to poisoned pointer.\n");

    if (error & STACK_POISONED_SIZE_ERR)
        logStack(fp,
                 "Get poisoned stack size.\n");

    if (error & STACK_POISONED_CAPACITY_ERR)
        logStack(fp,
                 "Get poisoned stack capacity.\n");

    if (error & STACK_INCORRECT_HASH)
        logStack(fp,
                 "Incorrect hash of stack.\n");

    if (error & STACK_NOT_ALIVE)
        logStack(fp,
                 "Stack not alive. Can't push and pop.\n");

    if (error & STACK_START_STRUCT_CANARY_DEAD)
        logStack(fp,
                 "Start canary in struct was destroyed.\n");
    if (error & STACK_START_STRUCT_CANARY_POISONED)
        logStack(fp,
                 "Start canary in struct was poisoned.\n");

    if (error & STACK_END_STRUCT_CANARY_DEAD)
        logStack(fp,
                 "End canary in struct was destroyed.\n");
    if (error & STACK_END_STRUCT_CANARY_POISONED)
        logStack(fp,
                 "End canary in struct was poisoned.\n");

    if (error & STACK_START_DATA_CANARY_DEAD)
        logStack(fp,
                 "Start canary in data was destroyed.\n");
    if (error & STACK_START_DATA_CANARY_POISONED)
        logStack(fp,
                 "Start canary in data was poisoned.\n");

    if (error & STACK_END_DATA_CANARY_DEAD)
        logStack(fp,
                 "End canary in data was destroyed.\n");
    if (error & STACK_END_DATA_CANARY_POISONED)
        logStack(fp,
                 "End canary in data was poisoned.\n");

    if (error & STACK_DATA_INCORRECT_HASH)
        logStack(fp,
                 "Incorrect hash of stack data.\n");

    if (error & STACK_NULLPTR)
        logStack(fp,
                 "Got stack nullptr.\n");
}
