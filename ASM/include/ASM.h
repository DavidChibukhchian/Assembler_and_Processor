#ifndef _ASM_H_
#define _ASM_H_

#include "Logger.h"

//----------------------------------------------------------------------------------------------------------------------

struct commands_struct
{
    char** array_of_commands;
    char* buffer;
    size_t number_of_commands;
};

int record_commands_to_buffer(FILE* ASM_in, commands_struct* commands);

struct code_struct
{
    char* pointer;
    size_t size;
    size_t offset;
    int err;
};

int create_code_array(commands_struct* commands, code_struct* code);

int write_code_to_file(code_struct* code, FILE* ASM_out);

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

#define ASSERT(condition, err, code)                         \
if (!(condition))                                            \
{                                                            \
    code;                                                    \
    dump_to_console(err);                                    \
    dump_to_logfile(logfile, err);                           \
    fclose(logfile);                                         \
    return err;                                              \
}

//----------------------------------------------------------------------------------------------------------------------

#define VERIFY_CODE_ERR                                      \
if (code->err)                                               \
{                                                            \
    if (code->pointer != nullptr)                            \
        free(code->pointer);                                 \
                                                             \
    free_buffer(commands);                                   \
    return code->err;                                        \
}

//----------------------------------------------------------------------------------------------------------------------

#endif