#ifndef STACK_VERIFICATION_H
#define STACK_VERIFICATION_H

#include "stack.h"

/**
 * @brief check if value is poisoned
 *
 * @param value value to check
 * @return if value is poisoned
 */
bool isPoison(Elem_t value);

/**
 * @brief check is stack data is poisoned
 *
 * @param stack stack to check
 * @param error error code
 */
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
 * @param error error code
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