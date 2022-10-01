#include "stack_logs.h"
#include "stack_verification.h"

FILE *STACK_LOG_FILE = stderr;

void setLogFile(const char *filename)
{
    if (filename == nullptr)
        return;

    FILE *fp = fopen(filename, "w");
    if (fp == nullptr)
        return;

    STACK_LOG_FILE = fp;
    setvbuf(STACK_LOG_FILE, nullptr, _IONBF, 0);
}

void closeLogFile()
{
    if (STACK_LOG_FILE != nullptr)
        fclose(STACK_LOG_FILE);
}

void printElem_t(FILE *fp, Elem_t value)
{
    if (fp != nullptr)
        logStack(fp, "%d", value);
    else
        logStack(stderr, "%d", value);
}

void logStack(FILE *fp, const char *formatString, ...)
{
    assert(fp != nullptr);
    assert(formatString != nullptr);

    va_list args;
    va_start(args, formatString);
    vfprintf(fp, formatString, args);
    fflush(STACK_LOG_FILE);
    va_end(args);
}

void printData(Elem_t *data,
               size_t size,
               bool alive,
               void (*print)(FILE *, Elem_t))
{
    for (size_t i = 0; i < size; i++)
    {
        logStack(STACK_LOG_FILE,
                 "    %c [%zu] = ",
                 alive ? '*' : ' ',
                 i);
        print(STACK_LOG_FILE, data[i]);
#if (PoisonProtection)
        logStack(STACK_LOG_FILE,
                 " %s\n",
                 isPoison(data[i]) ? "(Poisoned)" : "");
#else
        logStack(STACK_LOG_FILE, "\n");
#endif
    }
}

void stackDump(Stack *stack,
               StackInfo *info,
               size_t error,
               void (*print)(FILE *, Elem_t))
{
    if (STACK_LOG_FILE == nullptr)
        STACK_LOG_FILE = stderr;

    logStack(STACK_LOG_FILE, "-----START LOGGING STACK-----\n");
    if (stack == nullptr)
    {
        logStack(STACK_LOG_FILE,
                 "Can't log stack with pointer == nullptr");
        logStack(STACK_LOG_FILE, "-----END LOGGING STACK-----\n");
    }
    if (error & STACK_NOT_ALIVE)
    {
        processError(error);
        logStack(STACK_LOG_FILE, "-----END LOGGING STACK-----\n");
        return;
    }

    if (error & STACK_SIZE_MORE_THAN_CAPACITY)
    {
        logStack(STACK_LOG_FILE, "-----END LOGGING STACK-----\n");
        return;
    }

    if (info == nullptr)
    {
        logStack(STACK_LOG_FILE,
                 "Info pointer is nullptr. Can't log info.");
    }
    else
    {
        logStack(STACK_LOG_FILE, "Error code %zu.\n", error);
        logStack(STACK_LOG_FILE,
                 "Error in stack '%s' in function '%s' at %s (%d)\n",
                 info->name,
                 info->initFunction,
                 info->initFile,
                 info->initLine);

        logStack(STACK_LOG_FILE,
                 "Stack [%p] '%s' was initialized at %s at %s (%d)\n",
                 stack,
                 stack->info.name,
                 stack->info.initFunction,
                 stack->info.initFile,
                 stack->info.initLine);
    }
    if (error & STACK_POISONED_DATA or error & STACK_POISON_PTR_ERR)
    {
        logStack(STACK_LOG_FILE,
                 "Data is nullptr. Can't log stack data.");
        return;
    }

# if (HashProtection)
    logStack(STACK_LOG_FILE, "{\n"
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
    logStack(STACK_LOG_FILE, "{\n"
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
        logStack(STACK_LOG_FILE, "    Pointer poisoned.\n}\n");
        return;
    }
#endif
    printData(stack->data, stack->size, true, print);
    printData(stack->data + stack->size,
              stack->capacity - stack->size,
              false,
              print);

#if (CanaryProtection)
    logStack(STACK_LOG_FILE,
             "Data Canary end %zu\n",
             (Canary) (stack->data + stack->capacity));
    logStack(STACK_LOG_FILE,
             "Correct data Canary end %zu\n",
             CANARY_END);
#endif
    processError(error);
    logStack(STACK_LOG_FILE, "-----END LOGGING STACK-----\n");
}

void processError(size_t error)
{
    if (!error)
    {
        logStack(STACK_LOG_FILE,
                 "No errors.\n");
        return;
    }

    if (error & CANT_ALLOCATE_MEMORY_FOR_STACK)
        logStack(STACK_LOG_FILE,
                 "Can't allocate memory for stack.\n");
    if (error & CANT_ALLOCATE_MEMORY)
        logStack(STACK_LOG_FILE,
                 "Can't allocate memory.\n");
    if (error & STACK_IS_EMPTY)
        logStack(STACK_LOG_FILE,
                 "Can't pop element from stack. Stack is empty.\n");
    if (error & STACK_SIZE_MORE_THAN_CAPACITY)
        logStack(STACK_LOG_FILE,
                 "Size more than capacity.\n");
    if (error & STACK_POISON_PTR_ERR)
        logStack(STACK_LOG_FILE,
                 "Trying to write to poisoned pointer.\n");

    if (error & STACK_POISONED_SIZE_ERR)
        logStack(STACK_LOG_FILE,
                 "Get poisoned stack size.\n");

    if (error & STACK_POISONED_CAPACITY_ERR)
        logStack(STACK_LOG_FILE,
                 "Get poisoned stack capacity.\n");

    if (error & STACK_INCORRECT_HASH)
        logStack(STACK_LOG_FILE,
                 "Incorrect hash of stack.\n");

    if (error & STACK_NOT_ALIVE)
        logStack(STACK_LOG_FILE,
                 "Stack not alive. Can't push and pop.\n");

    if (error & STACK_START_STRUCT_CANARY_DEAD)
        logStack(STACK_LOG_FILE,
                 "Start canary in struct was destroyed.\n");
    if (error & STACK_START_STRUCT_CANARY_POISONED)
        logStack(STACK_LOG_FILE,
                 "Start canary in struct was poisoned.\n");

    if (error & STACK_END_STRUCT_CANARY_DEAD)
        logStack(STACK_LOG_FILE,
                 "End canary in struct was destroyed.\n");
    if (error & STACK_END_STRUCT_CANARY_POISONED)
        logStack(STACK_LOG_FILE,
                 "End canary in struct was poisoned.\n");

    if (error & STACK_START_DATA_CANARY_DEAD)
        logStack(STACK_LOG_FILE,
                 "Start canary in data was destroyed.\n");
    if (error & STACK_START_DATA_CANARY_POISONED)
        logStack(STACK_LOG_FILE,
                 "Start canary in data was poisoned.\n");

    if (error & STACK_END_DATA_CANARY_DEAD)
        logStack(STACK_LOG_FILE,
                 "End canary in data was destroyed.\n");
    if (error & STACK_END_DATA_CANARY_POISONED)
        logStack(STACK_LOG_FILE,
                 "End canary in data was poisoned.\n");

    if (error & STACK_DATA_INCORRECT_HASH)
        logStack(STACK_LOG_FILE,
                 "Incorrect hash of stack data.\n");

    if (error & STACK_NULLPTR)
        logStack(STACK_LOG_FILE,
                 "Got stack nullptr.\n");
}
