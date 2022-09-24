#include "utils.h"

bool test_1()
{
    Stack stack = {};

    size_t error = NO_ERRORS;
    stackCtor(&stack, 0, &error)

    for (int i = 0; i < 1024; i++)
    {
        error = stackPush(&stack, i);
    }
    for (int i = 0; i < 1000; i++)
    {
        Elem_t value = 0;
        error = stackPop(&stack, &value);
    }
    error = stackDtor(&stack);
    return true;
}

bool test_2()
{
    Stack stack = {};

    size_t error = NO_ERRORS;

    stackCtor(&stack, 0, &error)

    for (int i = 0; i < 1024; i++)
    {
        error = stackPush(&stack, i);
    }
    for (int i = 0; i < 1000; i++)
    {
        Elem_t value = 0;
        error = stackPop(&stack, &value);
    }
    stack.data = nullptr;
    error = stackPush(&stack, 1000-7);
    error = stackDtor(&stack);
    return true;
}

bool test_3()
{
    Stack stack = {};

    size_t error = NO_ERRORS;

    stackCtor(&stack, 0, &error)

    for (int i = 0; i < 1024; i++)
    {
        error = stackPush(&stack, i);
    }
    for (int i = 0; i < 1000; i++)
    {
        Elem_t value = 0;
        error = stackPop(&stack, &value);
    }
    error = stackDtor(&stack);

    Elem_t value = POISON_INT_VALUE;
    error = stackPop(&stack, &value);


    error = stackPush(&stack, 69);
    return true;
}

bool test_4()
{
    Stack stack = {};

    size_t error = NO_ERRORS;

    stackCtor(&stack, 0, &error)

    for (int i = 0; i < 1024; i++)
    {
        error = stackPush(&stack, i);
    }
    for (int i = 0; i < 1000; i++)
    {
        Elem_t value = 0;
        error = stackPop(&stack, &value);
    }
    stack.size = -69;
    stack.size = -69;
    error = stackDtor(&stack);
    return true;
}

int main()
{
    assert(test_1());
    assert(test_2());
    assert(test_3());
    assert(test_4());
}