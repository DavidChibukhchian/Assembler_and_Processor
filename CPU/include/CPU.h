#ifndef CPU_CPU_H
#define CPU_CPU_H

#include <stdio.h>
#include <malloc.h>
#include <sys\stat.h>

#include "Logger.h"
#include "Stack.h"


void func(char* stroka);

//----------------------------------------------------------------------------------------------------------------------

struct code_struct
{
    char* code;
    size_t size;
};

int check_file(FILE* ASM_out);

char* read_code_to_buffer(FILE* ASM_out, int* err);

int run_code(char* code, Stack* stk, int* REG);

//----------------------------------------------------------------------------------------------------------------------

#define CHECK_LOGFILE(logfile)                               \
if (logfile == nullptr)                                      \
{                                                            \
    printf("---\nERROR: Failed to create logfile\n---");     \
    return Failed_To_Create_Logfile;                         \
}

//----------------------------------------------------------------------------------------------------------------------

#define VERIFY(err)                                          \
if (err)                                                     \
{                                                            \
    dump_to_console(err);                                    \
    dump_to_logfile(logfile, err);                           \
    fclose(logfile);                                         \
    return err;                                              \
}

//----------------------------------------------------------------------------------------------------------------------

#define ASSERT(condition, err)                               \
if (!(condition))                                            \
{                                                            \
    dump_to_console(err);                                    \
    dump_to_logfile(logfile, err);                           \
    fclose(logfile);                                         \
    return err;                                              \
}

//----------------------------------------------------------------------------------------------------------------------

#define CHECK_STACK(command)                                 \
err = check_stack(stk, command);                             \
if (err)                                                     \
{                                                            \
    free(code);                                              \
    return err;                                              \
}

//----------------------------------------------------------------------------------------------------------------------

#endif