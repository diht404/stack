#include "utils.h"

void *recalloc(void *memory,
               size_t currentSize,
               size_t newSize,
               size_t *error)
{
    assert(memory != nullptr);

    if (error != nullptr and *error)
        return error;

    void *newMemory = realloc(memory, newSize);
    if (newMemory == nullptr)
    {
        if (error != nullptr)
            *error = CANT_ALLOCATE_MEMORY;
        return memory;
    }

    if (currentSize < newSize)
        memset((char *) newMemory + currentSize,
               0,
               newSize - currentSize);

    if (error != nullptr)
        *error = STACK_NO_ERRORS;

    return newMemory;
}

size_t stackCtor__(Stack *stack, size_t numOfElements, FILE *logFile)
{
    assert(stack != nullptr);
    assert(logFile != nullptr);

    size_t error = STACK_NO_ERRORS;

#if (CanaryProtection)
    size_t dataSize = (numOfElements) * sizeof(Elem_t);
    char *canary_data_canary = (char *) calloc(
        dataSize + 2 * sizeof(Canary), 1);
    stack->data = (Elem_t *) (canary_data_canary + sizeof(Canary));

    if (canary_data_canary == nullptr)
        return CANT_ALLOCATE_MEMORY_FOR_STACK;

    *(Canary *) canary_data_canary = CANARY_START;

    *(Canary *) (canary_data_canary + sizeof(Canary) + dataSize) =
        CANARY_END;
#else
    stack->data =
        (Elem_t *) calloc(numOfElements + 1, sizeof(stack->data[0]));
    if (stack->data == nullptr)
        return CANT_ALLOCATE_MEMORY_FOR_STACK;
#endif

    stack->size = 0;
    stack->capacity = numOfElements;

    for (size_t i = 0; i < stack->capacity; i++)
    {
        stack->data[i] = POISON_VALUE;
    }
    stack->alive = true;

# if (CanaryProtection)
    stack->canary_start = CANARY_START;
    stack->canary_end = CANARY_END;
# endif
    stack->logFile = logFile;
# if (HashProtection)
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

    *value = stack->data[stack->size];

#if (HashProtection)
    stack->hash = stackHash(stack);
#endif
    if (stack->size * 4 <= stack->capacity)
        error = stackResize(stack);

#if (HashProtection)
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

size_t stackResizeMemory(Stack *stack, size_t newStackCapacity)
{
    size_t error = 0;
    size_t dataSize =
        sizeof(Elem_t) * stack->capacity + 2 * sizeof(Canary);
    size_t newCapacity =
        sizeof(Elem_t) * (newStackCapacity - 7);

#if (CanaryProtection)
    char *newData =
        (char *) recalloc((char *) stack->data - sizeof(Canary),
                          dataSize,
                          newCapacity + 2 * sizeof(Canary),
                          &error);
#else
    Elem_t *newData =
        (Elem_t *) recalloc(stack->data,
                            dataSize,
                            newCapacity,
                            &error);
#endif

    if (newData == nullptr)
        return CANT_ALLOCATE_MEMORY_FOR_STACK;
#if (CanaryProtection)
    stack->data = (Elem_t *) (newData + sizeof(Canary));
    *(Canary *) newData = CANARY_START;

    *(Canary *) (newData + sizeof(Canary) + newCapacity) =
        CANARY_END;
#else
    stack->data = (Elem_t *) newData;
# endif

    stack->capacity = newStackCapacity;

#if (HashProtection)
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
    if (stack->size == stack->capacity)
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
# endif
    return error;
}

size_t stackVerifier(Stack *stack)
{
    assert(stack != nullptr);

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

# if (HashProtection)
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

    Canary *canary_start = (Canary *)((char *)stack->data - sizeof(canary_start[0]));
    if (*canary_start != CANARY_START)
    {
        if (*canary_start == CANARY_POISONED)
        {
            error |= STACK_START_DATA_CANARY_POISONED;
        }
        else
            error |= STACK_START_DATA_CANARY_DEAD;
    }

    Canary *canary_end = (Canary *)((stack->data + stack->capacity));
    if (*canary_end != CANARY_END)
    {
        if (*canary_end == CANARY_POISONED)
        {
            error |= STACK_END_DATA_CANARY_POISONED;
        }
        else
            error |= STACK_END_DATA_CANARY_DEAD;
    }
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

    logStack(fp, "%lg \n", value);
}

void stackDump(Stack *stack,
               StackInfo *info,
               size_t error,
               void (*print)(Elem_t, FILE *))
{
    assert(stack != nullptr);
    assert(info != nullptr);

    FILE *fp = stack->logFile;
    if (fp == nullptr)
        fp = stderr;

    logStack(fp, "-----START LOGGING STACK-----\n");
    logStack(fp, "Error in function '%s' at %s (%d)\n",
             info->initFunction,
             info->initFile,
             info->initLine);

    if (!stack->alive)
    {
        processError(fp, error);
        logStack(fp, "-----END LOGGING STACK-----\n");
        return;
    }

    logStack(fp, "Stack [%p] was initialized at %s at %s (%d)\n",
             stack,
             stack->info.initFunction,
             stack->info.initFile,
             stack->info.initLine);
# if (HashProtection)
    logStack(fp, "{\n"
                 "    Size = %zu \n"
                 "    Capacity = %zu \n"
                 "    Hash = %zu \n"
                 "    Correct Hash = %zu \n"
                 "    Data [%p] \n",
             stack->size,
             stack->capacity,
             stackHash(stack),
             stack->hash,
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
    }
    for (size_t i = stack->size; i < stack->capacity; i++)
    {
        logStack(fp, "      [%zu] = ", i);
        print(stack->data[i], fp);
    }
    logStack(fp, "}\n");
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
                 "Incorrect hash.\n");

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
}
