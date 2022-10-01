#ifndef STACK_LOGS_H
#define STACK_LOGS_H

#include "stack.h"
#include "stack_verification.h"

/**
 * @brief sets logfile
 *
 * @param filename name of logfile
 */
void setLogFile(const char *filename);

/**
 * @brief closes logfile
 */
void closeLogFile();

/**
 * @brief print Elem_t as double
 *
 * @param value Elem_t to print
 * @param fp file to write
 * @return void
 */
void printElem_t(FILE *fp, Elem_t value);

/**
 *
 * @param fp file for logs
 * @param formatString formatting string to log
 * @param ... argument of formatting string
 */
void logStack(FILE *fp, const char *formatString, ...);

/**
 * @brief prints array of Elem_t
 *
 * @param data array to print
 * @param size number of elements to print
 * @param alive prints * if array is alive and ' ' if not
 * @param print function to print element
 */
void printData(Elem_t *data,
               size_t size,
               bool alive,
               void (*print)(FILE *, Elem_t) = printElem_t);

/**
 * @brief generates dump of stack
 *
 * @param stack stack for dumping
 * @param info struct with info about stack
 * @return void
 */
void stackDump(Stack *stack,
               StackInfo *info,
               size_t error,
               void (*print)(FILE *, Elem_t) = printElem_t);

/**
 * @brief logs error to file
 *
 * @param error error code to process
 */
void processError(size_t error);

#endif