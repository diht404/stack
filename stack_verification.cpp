#include "stack_verification.h"

#if (PoisonProtection)
bool isPoison(Elem_t value)
{
    return value == POISON_VALUE;
}
#endif

#if (PoisonProtection)
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
#endif

size_t stackVerifier(Stack *stack)
{
    size_t error = STACK_NO_ERRORS;
    if (stack == nullptr)
    {
        error |= STACK_NULLPTR;
        return error;
    }

    if (!stack->alive)
    {
        error |= STACK_NOT_ALIVE;
        return error;
    }

    if (stack->size == (size_t) POISON_INT_VALUE)
    {
        error |= STACK_POISONED_SIZE_ERR;
        return error;
    }

    if (stack->capacity == (size_t) POISON_INT_VALUE)
    {
        error |= STACK_POISONED_CAPACITY_ERR;
        return error;
    }

    if (stack->size > stack->capacity)
    {
        error |= STACK_SIZE_MORE_THAN_CAPACITY;
        return error;
    }
#if (PoisonProtection)
    if (stack->data == POISON_PTR or stack->data == nullptr)
    {
        error |= STACK_POISON_PTR_ERR;
        return error;
    }
#endif

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
# endif

    return error;
}