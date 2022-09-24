#include "utils.h"

int main()
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
    stack.size = 69;
    error = stackDtor(&stack);

    Elem_t value = POISON_INT_VALUE;
    error = stackPop(&stack, &value);


    error = stackPush(&stack, 69);


    return 0;
}
