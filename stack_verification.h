#ifndef STACK_VERIFICATION_H
#define STACK_VERIFICATION_H

#include "stack.h"

bool isPoison(Elem_t value);

void stackVerifyPoison(Stack *stack, size_t *error);

/**
 * @brief Checks if stack is correct
 *
 * @param stack stack for checking
 * @return error code
 */
size_t stackVerifier(Stack *stack);

/**
 * @brief macro for checking if stack is correct
 *
 * @param stack stack for checking
 * @return void
 */
#define ASSERT_OK(stack, error)                                        \
{                                                                      \
    StackInfo info = {__LINE__, __FILE__, __PRETTY_FUNCTION__, #stack};\
    *(error) = stackVerifier((stack));                                 \
    if (*(error))                                                      \
    {                                                                  \
        stackDump((stack), &(info), *(error), printElem_t);            \
    }                                                                  \
}

#endif