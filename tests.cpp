#define HashProtection 1
#define CanaryProtection 1
#include "utils.h"

FILE *fp = nullptr;
void closeFile()
{
    if (fp != nullptr)
        fclose(fp);
}

bool test_1();
bool test_2();
bool test_3();
bool test_4();

bool test_1()
{
    Stack stack = {};

    size_t error = STACK_NO_ERRORS;
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
    error = stackDtor(&stack);
    return true;
}

bool test_2()
{
    Stack stack = {};

    size_t error = STACK_NO_ERRORS;

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
    stack.data = nullptr;
    error = stackPush(&stack, 1000-7);
    error = stackDtor(&stack);
    return true;
}

bool test_3()
{
    Stack stack = {};

    size_t error = STACK_NO_ERRORS;

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
    error = stackDtor(&stack);

    Elem_t value = POISON_INT_VALUE;
    error = stackPop(&stack, &value);


    error = stackPush(&stack, 69);
    return true;
}

bool test_4()
{
    Stack stack = {};

    size_t error = STACK_NO_ERRORS;

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
    stack.size = -69;
    stack.capacity = -69;
    error = stackDtor(&stack);
    return true;
}

int main()
{
    atexit(closeFile);
    fp = fopen("logs_tests.txt", "a");
    assert(test_1());
    assert(test_2());
    assert(test_3());
    assert(test_4());
}
