#include "stack.h"
#include "stack_verification.h"
#include "stack_logs.h"

size_t stackCtor__(Stack *stack, size_t numOfElements)
{
    assert(stack != nullptr);

    size_t error = STACK_NO_ERRORS;

    size_t dataSize = numOfElements * sizeof(Elem_t);

#if (CanaryProtection)
    char *canary_data_canary = (char *) calloc(
        dataSize + 2 * sizeof(Canary), sizeof(char));
    stack->data = (Elem_t *) (canary_data_canary + sizeof(Canary));

    if (canary_data_canary == nullptr)
        return CANT_ALLOCATE_MEMORY_FOR_STACK;

    *(Canary *) canary_data_canary = CANARY_START;
    *(Canary *) (canary_data_canary + sizeof(Canary) + dataSize) =
        CANARY_END;

    stack->canary_start = CANARY_START;
    stack->canary_end = CANARY_END;

#else
    stack->data = (Elem_t *) calloc(dataSize, 1);
    if (stack->data == nullptr)
        return CANT_ALLOCATE_MEMORY_FOR_STACK;
#endif

#if (PoisonProtection)
    stackPoisonData(stack);
#endif

    stack->size = 0;
    stack->capacity = numOfElements;
    stack->alive = true;

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
        *value = 0;
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
    if (stack->size == 0)
    {
#if (CanaryProtection)
        free((char *)stack->data - sizeof(Canary));
#else
        free(stack->data);
#endif
        stack->data = nullptr;
        ASSERT_OK(stack, &error)
        return error;
    }
    error = stackResizeMemory(stack, stack->size);
    if (error)
        return error;

    return error;
}

#if (PoisonProtection)
void stackPoisonData(Stack *stack)
{
    for (size_t i = stack->size; i < stack->capacity; i++)
    {
        stack->data[i] = POISON_VALUE;
    }
}
#endif

size_t stackResizeMemory(Stack *stack, size_t newStackCapacity)
{
    size_t error = 0;
    size_t newCapacity = sizeof(Elem_t) * newStackCapacity;

#if (CanaryProtection)
    char *newData =
        (char *) realloc((char *) stack->data - sizeof(Canary),
                         newCapacity + 2 * sizeof(Canary));
#else
    Elem_t *newData = (Elem_t *) realloc(stack->data, newCapacity);
#endif

    if (newData == nullptr)
        return CANT_ALLOCATE_MEMORY_FOR_STACK;

#if (CanaryProtection)
    stack->data = (Elem_t *) (newData + sizeof(Canary));

    *(Canary *) newData = CANARY_START;
    *(Canary *) (stack->data + newStackCapacity) = CANARY_END;

#else
    stack->data = (Elem_t *) newData;
#endif
    stack->capacity = newStackCapacity;
#if (PoisonProtection)
    stackPoisonData(stack);
#endif

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
    free((char *) stack->data - sizeof(Canary));
# else
    free(stack->data);
# endif

#if (PoisonProtection)
    stack->data = (Elem_t *) POISON_PTR;
#endif

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