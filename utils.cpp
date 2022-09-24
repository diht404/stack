#include "utils.h"

void processError(int error)
{
    switch (error)
    {
        case NO_ERRORS:break;
        case CANT_ALLOCATE_MEMORY_FOR_STACK:
            fprintf(stderr,
                    "Can't allocate memory for stack.\n");
            break;
        case CANT_ALLOCATE_MEMORY:
            fprintf(stderr,
                    "Can't allocate memory.\n");
            break;
        case STACK_IS_EMPTY:
            fprintf(stderr,
                    "Can't pop element from stack. Stack is empty.\n");
            break;
        case SIZE_MORE_THAN_CAPACITY:
            fprintf(stderr,
                    "Size more than capacity.\n");
            break;
        default:
            fprintf(stderr, "Unknown error code: %d.\n",
                    error);
            break;
    }
}

void *recalloc(void *memory,
               size_t currentSize,
               size_t newSize,
               Errors *error)
{
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
        *error = NO_ERRORS;

    return newMemory;
}

Errors stackVerifier(Stack *stack)
{
    assert(stack != nullptr);

    Errors error = NO_ERRORS;
    if (stack->size > stack->capacity)
    {
        error = SIZE_MORE_THAN_CAPACITY;
    }
    return error;
}

Errors __stackCtor(Stack *stack, size_t numOfElements)
{
    assert(stack != nullptr);

    Errors error = NO_ERRORS;

    stack->data = (Elem_t *) calloc(numOfElements, sizeof(Elem_t));
    if (stack->data == nullptr)
        return CANT_ALLOCATE_MEMORY_FOR_STACK;
    stack->size = 0;
    stack->capacity = numOfElements;

    for (size_t i = 0; i < stack->capacity; i++)
    {
        stack->data[i] = POISON_VALUE;
    }

    ASSERT_OK(stack)

    return error;
}

Errors stackPush(Stack *stack, Elem_t value)
{
    assert(stack != nullptr);

    Errors error = NO_ERRORS;

    ASSERT_OK(stack)
    if (error)
        return error;

    if (stack->size == stack->capacity)
        error = stackResize(stack);
    stack->data[stack->size++] = value;

    ASSERT_OK(stack)

    return error;
}

Errors stackPop(Stack *stack, Elem_t *value)
{
    assert(stack != nullptr);
    assert(value != nullptr);

    Errors error = NO_ERRORS;
    ASSERT_OK(stack)
    if (error)
        return error;

    if (stack->size == 0)
    {
        *value = POISON_VALUE;
        return STACK_IS_EMPTY;
    }

    *value = stack->data[stack->size--];

    if (stack->size * 4 <= stack->capacity)
        error = stackResize(stack);

    ASSERT_OK(stack)

    return error;
}

Errors stackDtor(Stack *stack)
{
    assert(stack != nullptr);

    Errors error = NO_ERRORS;

    ASSERT_OK(stack)
    if (error)
        return error;

    free(stack->data);

    stack->data = (Elem_t *) POISON_PTR;
    stack->size = POISON_INT_VALUE;
    stack->capacity = POISON_INT_VALUE;

    ASSERT_OK(stack)

    return error;
}

Errors stackDump(Stack *stack, StackInfo *info)
{
    assert(stack != nullptr);

    printf("Error in function '%s' at %s (%d)\n",
           info->initFunction,
           info->initFile,
           info->initLine);

    printf("Stack [%p] was initialized at %s at %s (%d)\n",
           stack,
           stack->info.initFunction,
           stack->info.initFile,
           stack->info.initLine);
    printf("{\n"
           "Size = %zu \n"
           "Capacity = %zu \n"
           "Data [%p] \n",
           stack->size,
           stack->capacity,
           stack->data);
    for (size_t i = 0; i < stack->size; i++)
    {
        printf("* [%zu] = %lg \n", i, stack->data[i]);
    }
    for (size_t i = stack->size; i < stack->capacity; i++)
    {
        printf("  [%zu] = %lg (POISON) \n", i, stack->data[i]);
    }
    printf("}\n");
    return NO_ERRORS;
}

Errors stackResize(Stack *stack)
{
    assert(stack != nullptr);

    Errors error = NO_ERRORS;

    ASSERT_OK(stack)

    if (error)
        return error;

    if (stack->capacity == 0)
    {
        stackCtor(stack, 1, &error)
        return error;
    }
    if (stack->size == stack->capacity)
    {
        size_t newStackCapacity = stack->capacity * 2;

        Elem_t *newData =
            (Elem_t *) recalloc(stack->data,
                                sizeof(Elem_t) * stack->capacity,
                                sizeof(Elem_t) * newStackCapacity,
                                &error);
        if (newData == nullptr)
            return CANT_ALLOCATE_MEMORY_FOR_STACK;

        stack->data = newData;
        stack->capacity = newStackCapacity;

        ASSERT_OK(stack)

        return error;
    }

    if (stack->size * 4 <= stack->capacity)
    {
        size_t newStackCapacity = stack->capacity / 2;

        Elem_t *newData =
            (Elem_t *) recalloc(stack->data,
                                sizeof(Elem_t) * stack->capacity,
                                sizeof(Elem_t) * newStackCapacity,
                                &error);
        if (newData == nullptr)
            return CANT_ALLOCATE_MEMORY_FOR_STACK;

        stack->data = newData;
        stack->capacity = newStackCapacity;

        ASSERT_OK(stack)
        if (error)
            return error;

        return error;
    }

    ASSERT_OK(stack)

    return error;
}
