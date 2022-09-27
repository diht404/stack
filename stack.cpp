#include "utils.h"

FILE *fp = nullptr;
void closeFile()
{
    if (fp != nullptr)
        fclose(fp);
}

int main()
{
    atexit(closeFile);
    Stack stack = {};
    FILE *fp = fopen("logs.txt", "w");
    if (fp == nullptr)
        return 1;
    size_t error = NO_ERRORS;

    stackCtor(&stack, 0, &error, fp)

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
