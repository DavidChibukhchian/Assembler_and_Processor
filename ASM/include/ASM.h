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
};

struct jump_struct
{
    int address;
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

void close_files(files_struct* files);

void open_logfile(files_struct* files);

int open_files(files_struct* files, char** argv);

int record_commands_to_buffer(files_struct* files, commands_struct* commands);

int create_code_array(code_struct* code, commands_struct* commands);

int write_code_to_file(code_struct* code, files_struct* files);

//----------------------------------------------------------------------------------------------------------------------

#define OPEN_LOGFILE(files)                                  \
                                                             \
FILE* logfile = fopen("ASM_logfile.txt",  "w");              \
if (logfile == nullptr)                                      \
{                                                            \
    printf("---\nERROR: Failed to create logfile\n---");     \
    return Failed_To_Create_Logfile;                         \
}                                                            \
files.logfile = logfile;

//----------------------------------------------------------------------------------------------------------------------

#define VERIFY_err                                           \
                                                             \
if (err)                                                     \
{                                                            \
    dump_to_console(err);                                    \
    dump_to_logfile(files.logfile, err);                     \
    close_files(&files);                                     \
                                                             \
    return err;                                              \
}

//----------------------------------------------------------------------------------------------------------------------

#define ASSERT(condition, err)                               \
                                                             \
if (!(condition))                                            \
{                                                            \
    dump_to_console(err);                                    \
    dump_to_logfile(files.logfile, err);                     \
    fclose(files.logfile);                                   \
                                                             \
    return err;                                              \
}

//----------------------------------------------------------------------------------------------------------------------

#define VERIFY(err)                                          \
                                                             \
if (err)                                                     \
{                                                            \
    free_buffer(commands);                                   \
                                                             \
    code_Dtor(code);                                         \
    labels_Dtor(labels);                                     \
    jumps_Dtor(jumps);                                       \
                                                             \
    return err;                                              \
}

//----------------------------------------------------------------------------------------------------------------------

#endif // _ASM_H_