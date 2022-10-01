#include "stack.h"
#include "stack_logs.h"

//FILE *fp = nullptr;
//void closeFile()
//{
//    if (fp != nullptr)
//        fclose(fp);
//}

int main()
{
    Stack stack = {};
    FILE *fp = openLogs("logs.txt");
    if (fp == nullptr)
        return 1;

    size_t error = STACK_NO_ERRORS;

    stackCtor(&stack, 0, &error, fp)

    for (int i = 0; i < 1024; i++)
    {
        error = stackPush(&stack, i);
    }
    for (int i = 0; i < 1024; i++)
    {
        Elem_t value = 0;
        error = stackPop(&stack, &value);
    }
    stackShrinkToFit(&stack);
    error = stackDtor(&stack);

    Elem_t value = POISON_INT_VALUE;
    error = stackPop(&stack, &value);

    error = stackPush(&stack, 666);

    closeLogs(fp);
    return 0;
}
