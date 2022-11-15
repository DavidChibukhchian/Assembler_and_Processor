#ifndef _ASM_H_
#define _ASM_H_

#include "Logger.h"

#define ASSERT(condition, err, code) {  if (!(condition))           \
                                        {                           \
                                            code;                   \
                                            dump(logfile, err);     \
                                            fclose(logfile);        \
                                            return err;             \
                                        }                           }

#define VERIFY(err) {  if (err)                    \
                       {                           \
                           dump(logfile, err);     \
                           fclose(logfile);        \
                           return err;             \
                       }                           }

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

void write_code_to_file(code_struct* code, FILE* ASM_out);

#endif