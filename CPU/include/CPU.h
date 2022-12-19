#ifndef _CPU_H
#define _CPU_H

//----------------------------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <malloc.h>
#include <sys\stat.h>
#include "Logger.h"
#include "Stack.h"

//----------------------------------------------------------------------------------------------------------------------

typedef unsigned char* Code;

static const size_t NUMBER_OF_REGISTERS = 5;
static const size_t SIZE_OF_RAM = 1024;

//----------------------------------------------------------------------------------------------------------------------

struct files_struct
{
    FILE* logfile;
    FILE* ASM_out;
};

//struct code_struct
//{
//    char* code;
//    size_t size;
//};

//----------------------------------------------------------------------------------------------------------------------

int open_logfile(files_struct* files);

int open_files(files_struct* files, char** argv);

int read_code(Code* code, files_struct* files);

int run_code(Code code, Stack* stack, int* REG, int* RAM);

void close_files(files_struct* files);

//----------------------------------------------------------------------------------------------------------------------

#define ASSERT(condition, err)                                                                                         \
                                                                                                                       \
if (!(condition))                                                                                                      \
{                                                                                                                      \
    dump_to_console(err);                                                                                              \
    dump_to_logfile(files.logfile, err);                                                                               \
    fclose(files.logfile);                                                                                             \
                                                                                                                       \
    return err;                                                                                                        \
}

//----------------------------------------------------------------------------------------------------------------------

#define VERIFY_err                                                                                                     \
                                                                                                                       \
if (err)                                                                                                               \
{                                                                                                                      \
    dump_to_console(err);                                                                                              \
    dump_to_logfile(files.logfile, err);                                                                               \
    close_files(&files);                                                                                               \
                                                                                                                       \
    return err;                                                                                                        \
}

//----------------------------------------------------------------------------------------------------------------------

#define CHECK_STACK(command)                                                                                           \
                                                                                                                       \
err = check_stack(stack, command);                                                                                     \
if (err)                                                                                                               \
{                                                                                                                      \
    free(code);                                                                                                        \
    return err;                                                                                                        \
}

//----------------------------------------------------------------------------------------------------------------------

#define CHECK_RAM_ADDRESS(address)                                                                                     \
                                                                                                                       \
err = check_RAM_address(number_1);                                                                                     \
if (err)                                                                                                               \
{                                                                                                                      \
    free(code);                                                                                                        \
    return err;                                                                                                        \
}

//----------------------------------------------------------------------------------------------------------------------

#endif // _CPU_H