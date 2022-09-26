#include "utils.h"

void processError(size_t error)
{
    if (!error)
        fprintf(stderr,
                "No errors.\n");
    if (error & CANT_ALLOCATE_MEMORY_FOR_STACK)
        fprintf(stderr,
                "Can't allocate memory for stack.\n");
    if (error & CANT_ALLOCATE_MEMORY)
        fprintf(stderr,
                "Can't allocate memory.\n");
    if (error & STACK_IS_EMPTY)
        fprintf(stderr,
                "Can't pop element from stack. Stack is empty.\n");
    if (error & SIZE_MORE_THAN_CAPACITY)
        fprintf(stderr,
                "Size more than capacity.\n");
    if (error & POISON_PTR_ERR)
        fprintf(stderr,
                "Trying to write to poisoned pointer.\n");

    if (error & POISONED_SIZE_ERR)
        fprintf(stderr,
                "Get poisoned stack size.\n");

    if (error & POISONED_CAPACITY_ERR)
        fprintf(stderr,
                "Get poisoned stack capacity.\n");

}

void *recalloc(void *memory,
               size_t currentSize,
               size_t newSize,
               size_t *error)
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

size_t stackVerifier(Stack *stack)
{
    assert(stack != nullptr);

    size_t error = NO_ERRORS;
    if (stack->size == (size_t) POISON_INT_VALUE)
    {
        error |= POISONED_SIZE_ERR;
    }

    if (stack->capacity == (size_t) POISON_INT_VALUE)
    {
        error |= POISONED_CAPACITY_ERR;
    }

    if (stack->size > stack->capacity)
    {
        error |= SIZE_MORE_THAN_CAPACITY;
    }

    if (stack->data == POISON_PTR or stack->data == nullptr)
    {
        error |= POISON_PTR_ERR;
    }
    return error;
}

size_t __stackCtor(Stack *stack, size_t numOfElements)
{
    assert(stack != nullptr);

    size_t error = NO_ERRORS;

    stack->data = (Elem_t *) calloc(numOfElements + 1, sizeof(stack->data[0]));
    if (stack->data == nullptr)
        return CANT_ALLOCATE_MEMORY_FOR_STACK;
    stack->size = 0;
    stack->capacity = numOfElements;

    for (size_t i = 0; i < stack->capacity; i++)
    {
        stack->data[i] = POISON_VALUE;
    }
    ASSERT_OK(stack, &error)

    return error;
}

size_t stackPush(Stack *stack, Elem_t value)
{
    assert(stack != nullptr);

    size_t error = NO_ERRORS;

    ASSERT_OK(stack, &error)
    if (error)
        return error;

    if (stack->size == stack->capacity)
        error = stackResize(stack);

    if (error)
        return error;

    stack->data[stack->size++] = value;

    ASSERT_OK(stack, &error)

    return error;
}

size_t stackPop(Stack *stack, Elem_t *value)
{
    assert(stack != nullptr);
    assert(value != nullptr);

    size_t error = NO_ERRORS;
    ASSERT_OK(stack, &error)
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

    ASSERT_OK(stack, &error)

    return error;
}

size_t stackDtor(Stack *stack)
{
    assert(stack != nullptr);

    size_t error = NO_ERRORS;

    ASSERT_OK(stack, &error)

    if (error)
        return error;

    free(stack->data);

    stack->data = (Elem_t *) POISON_PTR;
    stack->size = (size_t) POISON_INT_VALUE;
    stack->capacity = (size_t) POISON_INT_VALUE;

    return error;
}

void stackDump(Stack *stack, StackInfo *info)
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
           "    Size = %zu \n"
           "    Capacity = %zu \n"
           "    Data [%p] \n",
           stack->size,
           stack->capacity,
           stack->data);
    if (stack->data == POISON_PTR or stack->data == nullptr)
    {
        printf("    Pointer poisoned.\n}\n");
        return;
    }
    for (size_t i = 0; i < stack->size; i++)
    {
        printf("    * [%zu] = %lg \n", i, stack->data[i]);
    }
    for (size_t i = stack->size; i < stack->capacity; i++)
    {
        printf("      [%zu] = %lg (POISON) \n", i, stack->data[i]);
    }
    printf("}\n");
}

size_t stackResizeMemory(Stack *stack, size_t newStackCapacity)
{
    size_t error = 0;
    Elem_t *newData =
        (Elem_t *) recalloc(stack->data,
                            sizeof(newData[0]) * stack->capacity,
                            sizeof(newData[0]) * newStackCapacity,
                            &error);
    if (newData == nullptr)
        return CANT_ALLOCATE_MEMORY_FOR_STACK;

    stack->data = newData;
    stack->capacity = newStackCapacity;

    ASSERT_OK(stack, &error)
    return error;
}

size_t stackResize(Stack *stack)
{
    assert(stack != nullptr);

    size_t error = NO_ERRORS;

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
