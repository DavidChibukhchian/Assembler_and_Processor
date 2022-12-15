#ifndef _ASM_H_
#define _ASM_H_

//----------------------------------------------------------------------------------------------------------------------

#include "Buffer.h"
#include "Labels.h"

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
    int offset;
    int err;
    size_t wrong_line;
};

struct jump_struct
{
    int code_address;
    size_t line;
    char* label_name;
};

struct jumps_struct
{
    jump_struct* jump;
    size_t number_of_addresses;
    size_t number_of_free_addresses;
    int err;
};

//----------------------------------------------------------------------------------------------------------------------

int open_logfile(files_struct* files);

int open_files(files_struct* files, char** argv);

int record_commands_to_buffer(files_struct* files, commands_struct* commands);

int create_code(code_struct* code, commands_struct* commands, size_t* err_line);

int write_code_to_file(code_struct* code, files_struct* files);

void close_files(files_struct* files);

//----------------------------------------------------------------------------------------------------------------------

#define VERIFY_err                                                                                                     \
                                                                                                                       \
if (err)                                                                                                               \
{                                                                                                                      \
    dump_to_console(err, err_line);                                                                                    \
    dump_to_logfile(files.logfile, err, err_line);                                                                     \
    close_files(&files);                                                                                               \
                                                                                                                       \
    return err;                                                                                                        \
}

//----------------------------------------------------------------------------------------------------------------------

#define ASSERT(condition, err)                                                                                         \
                                                                                                                       \
if (!(condition))                                                                                                      \
{                                                                                                                      \
    dump_to_console(err);                                                                                              \
    dump_to_logfile(files.logfile, err);                                                                               \
    close_files(&files);                                                                                               \
                                                                                                                       \
    return err;                                                                                                        \
}

//----------------------------------------------------------------------------------------------------------------------

#define CHECK_Ctor(err)                                                                                                \
                                                                                                                       \
if (err)                                                                                                               \
{                                                                                                                      \
    free_buffer(commands);                                                                                             \
                                                                                                                       \
    code_Dtor(code);                                                                                                   \
    labels_Dtor(&labels);                                                                                              \
    jumps_Dtor(&jumps);                                                                                                \
                                                                                                                       \
    return err;                                                                                                        \
}

//----------------------------------------------------------------------------------------------------------------------

#define VERIFY(err)                                                                                                    \
                                                                                                                       \
if (err)                                                                                                               \
{                                                                                                                      \
    if (syntax_error)                                                                                                  \
        *err_line = i + 1;                                                                                             \
                                                                                                                       \
    free_buffer(commands);                                                                                             \
                                                                                                                       \
    code_Dtor(code);                                                                                                   \
    labels_Dtor(labels);                                                                                               \
    jumps_Dtor(jumps);                                                                                                 \
                                                                                                                       \
    return err;                                                                                                        \
}

//----------------------------------------------------------------------------------------------------------------------

#endif // _ASM_H_