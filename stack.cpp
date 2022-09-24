#include "utils.h"

int main()
{
    Stack stack = {};

    Errors error = NO_ERRORS;

    stackCtor(&stack, 0, &error);
    if (error)
        processError(error);

    for(int i=0; i<1024; i++)
    {
        error = stackPush(&stack, 666);
        if (error)
            processError(error);
    }
    for(int i=0; i<1000; i++)
    {
        Elem_t value = 0;
        error = stackPop(&stack, &value);
        if (error)
            processError(error);
    }

    error = stackDtor(&stack);
    if (error)
        processError(error);

//    stackPush(&stack, 69);
    return 0;
}
