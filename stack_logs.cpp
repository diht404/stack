#include "stack_logs.h"
#include "stack_verification.h"

FILE *openLogs(const char *filename)
{
    if (filename == nullptr)
        return stderr;

    FILE *fp = fopen(filename, "w");
    if (fp == nullptr)
        return stderr;

    return fp;
}

void closeLogs(FILE *fp)
{
    if (fp != nullptr)
        fclose(fp);
}

void printElem_t(FILE *fp, Elem_t value)
{
    assert(fp != nullptr);

    logStack(fp, "%d", value);
}

void logStack(FILE *fp, const char *formatString, ...)
{
    assert(fp != nullptr);
    assert(formatString != nullptr);

    va_list args;
    va_start(args, formatString);
    vfprintf(fp, formatString, args);
    va_end(args);
}

void printData(Elem_t *data,
               FILE *fp,
               size_t size,
               bool alive,
               void (*print)(FILE *, Elem_t))
{
    for (size_t i = 0; i < size; i++)
    {

        logStack(fp, "    %c [%zu] = ", alive ? '*' : ' ', i);
        print(fp, data[i]);
#if (PoisonProtection)
        logStack(fp,
                 " %s\n",
                 isPoison(data[i]) ? "(Poisoned)" : "");
#else
        logStack(fp, "\n");
#endif
    }
}

void stackDump(Stack *stack,
               StackInfo *info,
               size_t error,
               void (*print)(FILE *, Elem_t))
{
    FILE *fp = stack->logFile;
    if (fp == nullptr)
        fp = stderr;

    logStack(fp, "-----START LOGGING STACK-----\n");
    if (stack == nullptr)
    {
        logStack(fp, "Can't log stack with pointer == nullptr");
        logStack(fp, "-----END LOGGING STACK-----\n");
    }
    if (error & STACK_NOT_ALIVE)
    {
        processError(fp, error);
        logStack(fp, "-----END LOGGING STACK-----\n");
        return;
    }

    if (error & STACK_SIZE_MORE_THAN_CAPACITY)
    {
        logStack(fp, "-----END LOGGING STACK-----\n");
        return;
    }

    if (info == nullptr)
    {
        logStack(fp, "Info pointer is nullptr. Can't log info.");
    }
    else
    {
        logStack(fp, "Error code %zu.\n", error);
        logStack(fp,
                 "Error in stack '%s' in function '%s' at %s (%d)\n",
                 info->name,
                 info->initFunction,
                 info->initFile,
                 info->initLine);

        logStack(fp,
                 "Stack [%p] '%s' was initialized at %s at %s (%d)\n",
                 stack,
                 stack->info.name,
                 stack->info.initFunction,
                 stack->info.initFile,
                 stack->info.initLine);
    }
    if (error & STACK_POISONED_DATA or error & STACK_POISON_PTR_ERR)
    {
        logStack(fp, "Data is nullptr. Can't log stack data.");
        return;
    }

# if (HashProtection)
    logStack(fp, "{\n"
                 "    Size = %zu \n"
                 "    Capacity = %zu \n"
                 "    Stack hash = %zu \n"
                 "    Correct stack hash = %zu \n"
                 "    Stack data hash = %zu \n"
                 "    Correct stack data hash = %zu \n"
                 "    Data [%p] \n",
             stack->size,
             stack->capacity,
             stackHash(stack),
             stack->hash,
             stackHashBuffer(stack),
             stack->dataHash,
             stack->data);
#else
    logStack(fp, "{\n"
                 "    Size = %zu \n"
                 "    Capacity = %zu \n"
                 "    Data [%p] \n",
             stack->size,
             stack->capacity,
             stack->data);
#endif

#if (PoisonProtection)
    if (stack->data == POISON_PTR or stack->data == nullptr)
    {
        logStack(fp, "    Pointer poisoned.\n}\n");
        return;
    }
#endif
    printData(stack->data, fp, stack->size, true, print);
    printData(stack->data + stack->size,
              fp,
              stack->capacity - stack->size,
              false,
              print);

#if (CanaryProtection)
    logStack(fp,
             "Data Canary end %zu\n",
             (Canary) (stack->data + stack->capacity));
    logStack(fp, "Correct data Canary end %zu\n", CANARY_END);
#endif
    processError(fp, error);
    logStack(fp, "-----END LOGGING STACK-----\n");
}

void processError(FILE *fp, size_t error)
{
    assert(fp != nullptr);

    if (!error)
        logStack(fp,
                 "No errors.\n");
    if (error & CANT_ALLOCATE_MEMORY_FOR_STACK)
        logStack(fp,
                 "Can't allocate memory for stack.\n");
    if (error & CANT_ALLOCATE_MEMORY)
        logStack(fp,
                 "Can't allocate memory.\n");
    if (error & STACK_IS_EMPTY)
        logStack(fp,
                 "Can't pop element from stack. Stack is empty.\n");
    if (error & STACK_SIZE_MORE_THAN_CAPACITY)
        logStack(fp,
                 "Size more than capacity.\n");
    if (error & STACK_POISON_PTR_ERR)
        logStack(fp,
                 "Trying to write to poisoned pointer.\n");

    if (error & STACK_POISONED_SIZE_ERR)
        logStack(fp,
                 "Get poisoned stack size.\n");

    if (error & STACK_POISONED_CAPACITY_ERR)
        logStack(fp,
                 "Get poisoned stack capacity.\n");

    if (error & STACK_INCORRECT_HASH)
        logStack(fp,
                 "Incorrect hash of stack.\n");

    if (error & STACK_NOT_ALIVE)
        logStack(fp,
                 "Stack not alive. Can't push and pop.\n");

    if (error & STACK_START_STRUCT_CANARY_DEAD)
        logStack(fp,
                 "Start canary in struct was destroyed.\n");
    if (error & STACK_START_STRUCT_CANARY_POISONED)
        logStack(fp,
                 "Start canary in struct was poisoned.\n");

    if (error & STACK_END_STRUCT_CANARY_DEAD)
        logStack(fp,
                 "End canary in struct was destroyed.\n");
    if (error & STACK_END_STRUCT_CANARY_POISONED)
        logStack(fp,
                 "End canary in struct was poisoned.\n");

    if (error & STACK_START_DATA_CANARY_DEAD)
        logStack(fp,
                 "Start canary in data was destroyed.\n");
    if (error & STACK_START_DATA_CANARY_POISONED)
        logStack(fp,
                 "Start canary in data was poisoned.\n");

    if (error & STACK_END_DATA_CANARY_DEAD)
        logStack(fp,
                 "End canary in data was destroyed.\n");
    if (error & STACK_END_DATA_CANARY_POISONED)
        logStack(fp,
                 "End canary in data was poisoned.\n");

    if (error & STACK_DATA_INCORRECT_HASH)
        logStack(fp,
                 "Incorrect hash of stack data.\n");

    if (error & STACK_NULLPTR)
        logStack(fp,
                 "Got stack nullptr.\n");
}
