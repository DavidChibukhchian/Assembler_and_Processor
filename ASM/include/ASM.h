#ifndef _ASM_H_
#define _ASM_H_

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "Buffer.h"
#include "Logger.h"

//----------------------------------------------------------------------------------------------------------------------

struct files_struct
{
    FILE* logfile;
    FILE* ASM_in;
    FILE* ASM_out;
};

struct commands_struct
{
    char** array_of_commands;
    char* buffer;
    size_t number_of_commands;
};

struct code_struct
{
    char* pointer;
    size_t size;
    size_t offset;
    int err;
};

//----------------------------------------------------------------------------------------------------------------------

void close_files(files_struct* files);

void open_logfile(files_struct* files);

int open_files(files_struct* files, char** argv);

int record_commands_to_buffer(FILE* ASM_in, commands_struct* commands);

int create_code_array(code_struct* code, commands_struct* commands);

int write_code_to_file(code_struct* code, FILE* ASM_out);

//----------------------------------------------------------------------------------------------------------------------

#define CHECK_LOGFILE(files)                                 \
if (files.logfile == nullptr)                                \
{                                                            \
    printf("---\nERROR: Failed to create logfile\n---");     \
    return Failed_To_Create_Logfile;                         \
}

//----------------------------------------------------------------------------------------------------------------------

#define VERIFY(err)                                          \
if (err)                                                     \
{                                                            \
    dump_to_console(err);                                    \
    dump_to_logfile(files.logfile, err);                     \
    fclose(files.logfile);                                   \
    return err;                                              \
}

//----------------------------------------------------------------------------------------------------------------------

#define ASSERT(condition, err)                               \
if (!(condition))                                            \
{                                                            \
    dump_to_console(err);                                    \
    dump_to_logfile(files.logfile, err);                     \
    fclose(files.logfile);                                   \
    return err;                                              \
}

//----------------------------------------------------------------------------------------------------------------------

#define VERIFY_CODE_ERR                                      \
if (code->err)                                               \
{                                                            \
    if (code->pointer != nullptr)                            \
        free(code->pointer);                                 \
    free_buffer(commands);                                   \
    return code->err;                                        \
}

//----------------------------------------------------------------------------------------------------------------------

#endif